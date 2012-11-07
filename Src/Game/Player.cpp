#include "Player.hpp"
#include <Game/GameServer.hpp>
#include <Game/MassiveMessageManager.hpp>

#include <System/Log.h>
#include <System/Tools.h>

namespace Game
{
	std::map<int32_t, Player::PacketHandler> Player::handlers;

	void Player::RegisterImpl(int32_t opcode, PacketHandler handler)
	{
		handlers[opcode] = handler;
	}

	Player::Player(Player::KeyType key, GameServer* server)
		: key(key), gameServer(server), synchronized(false)
	{
	}

	Player::~Player()
	{
	}

	void Player::SetConnection(Framework::Network::TcpConnection::pointer pConnection)
	{
		mConnection = pConnection;
		mConnection->SetStrategy(this);
		mConnection->OnError.connect(boost::bind(&Player::OnError, this, _1));
	}

	Player::KeyType Player::GetKey() const
	{
		return key;
	}

	bool Player::Synchronized() const
	{
		return synchronized;
	}

	bool Player::Local() const
	{
		return (gameServer == nullptr);
	}

	void Player::SendSynchronize()
	{
		Framework::Network::Packet pack(kSynchronize);
		pack << (uint32_t)GetKey();
		Write(pack);
	}

	void Player::Update()
	{
		while(HasPacket())
		{
			Framework::Network::Packet data = PopPacket();
			try
			{
				switch(data.Type)
				{
				case Framework::Network::Packet::kMessage:
					switch(data.Opcode)
					{
					case kSynchronize:
						HandleSynchronize(data);
						break;
					case kAwareness:
						HandleAwareness(data);
						break;
					case kReplicationTransaction:
						HandleReplicationTransaction(data);
						break;
					default:
						(this->*handlers.at(data.Opcode))(data);
						break;
					}
					break;
				case Framework::Network::Packet::kHandshake:
					HandleHandshake(data);
					break;
				default:
					if(mConnection)
						mConnection->Close();
					break;
				}
			}
			catch(boost::exception& e)
			{
				Framework::System::Log::Error(boost::diagnostic_information(e));
			}
			catch(std::exception& e)
			{
				std::ostringstream os;
				os << e.what() << " opcode : " << Framework::System::IntToString(data.Opcode) << std::endl;
				Framework::System::Log::Error(os.str());
			}
		}
	}

	void Player::Write(Framework::Network::Packet& pPacket)
	{
		if(!mConnection)
			return;

		pPacket.ObjectId = this->key;
		auto str = Serialize(pPacket);
		mConnection->Write(str);
	}

	void Player::SetCipher(Framework::Crypt::Cipher* pCipher)
	{
		mCipher.reset(pCipher);
	}

	void Player::HandleHandshake(Framework::Network::Packet& pPacket)
	{
		std::string rsaData;

		uint32_t version, flag;
		pPacket >> version >> flag;

		rsaData = Framework::Crypt::RSA::Decrypt(pPacket.GetBuffer());
		pPacket.Initialize(rsaData);

		std::string salsaEnc, salsaDec, salsaEncIV, salsaDecIV;
		pPacket >> salsaEnc >> salsaDec >> salsaEncIV >> salsaDecIV;
		SetCipher(new Framework::Crypt::Cipher(salsaEnc, salsaDec, salsaEncIV, salsaDecIV));

		SendSynchronize();
	}

	void Player::HandleSynchronize(Framework::Network::Packet& pPacket)
	{
		this->key = pPacket.ObjectId;
	}

	void Player::OnError(const std::string& pError)
	{
		if(gameServer)
			gameServer->Remove(this);
	}

	void Player::OnSynchronize()
	{

	}
}