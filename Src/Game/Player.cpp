#include "Player.hpp"
#include <Game/GameServer.hpp>

#include <System/Log.h>

namespace Game
{
	void Player::Register(uint32_t opcode, PacketHandler handler)
	{
		handlers[opcode] = handler;
	}

	Player::Player(Player::KeyType key, GameServer* server)
		: key(key), gameServer(server), synchronized(false)
	{
	}

	void Player::SetConnection(Framework::Network::TcpConnection::pointer pConnection)
	{
		connection = pConnection;
		connection->SetStrategy(this);
	}

	Player::KeyType Player::GetKey() const
	{
		return key;
	}

	bool Player::Synchronized() const
	{
		return synchronized;
	}

	void Player::SendSynchronize()
	{
		Framework::Network::Packet pack(kSynchronize);
		pack << (uint32_t)GetKey();
		Write(pack);
	}

	void Player::Update()
	{
		if(connection)
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
						connection->Close();
						break;
					}
				}
				catch(boost::exception& e)
				{
					Framework::System::Log::Error(boost::diagnostic_information(e));
				}
				catch(std::exception& e)
				{
					Framework::System::Log::Error(e.what());
				}
			}
	}

	void Player::Write(Framework::Network::Packet& pPacket)
	{
		pPacket.ObjectId = this->key;
		auto str = Serialize(pPacket);
		connection->Write(str);
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
}