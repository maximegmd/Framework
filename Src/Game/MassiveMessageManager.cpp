
#include "MassiveMessageManager.hpp"
#include <Game/Player.hpp>
#include <System/Tools.h>
#include <System/Log.h>
#include <random>

Game::MassiveMessageManager* TheMassiveMessageMgr = ::new Game::MassiveMessageManager;

namespace Game
{
	MassiveMessageManager::MassiveMessageManager() :
		host(false), 
		ioServicePool(1),
		gameServer(nullptr)
	{
		
	}

	MassiveMessageManager::~MassiveMessageManager()
	{
		localPlayer.reset(nullptr);
	}

	void MassiveMessageManager::SetPort(uint16_t pPort)
	{
		port = pPort;
	}

	void MassiveMessageManager::SetAddress(const std::string& pAddress)
	{
		address = pAddress;
	}

	void MassiveMessageManager::SetPlayerConstructor(GameServer::PlayerConstructor& ctor)
	{
		playerConstructor = ctor;
	}

	void MassiveMessageManager::SetGOMServerConstructor(GameServer::GOMServerConstructor& ctor)
	{
		gomConstructor = ctor;
	}

	void MassiveMessageManager::BeginMultiplayer(bool pHost)
	{
		if(!gomConstructor)
		{
			throw std::runtime_error("No GOM Server constructor set.");
		}

		host = pHost;
		//if(host)
		{
			gameServer.reset(nullptr);
			gameServer.reset(new GameServer(port, playerConstructor, gomConstructor));
		}
		//else
		{
			gomDatabase.reset(new GOMDatabase(gomConstructor(nullptr)));	
			Player* player = playerConstructor ? playerConstructor(kPlayerSelf, nullptr) : new Player(kPlayerSelf);
			localPlayer.reset(player);

			ioServicePool.Run();
			connection.reset(::new Framework::Network::TcpConnection(ioServicePool.GetIoService()));
			Connect(address, std::to_string((long long)port));
					
		}
	}

	void MassiveMessageManager::Connect(const std::string& pAddress, const std::string& pPort)
	{
		Framework::System::Log::Debug(std::string("MassiveMessageManager : Connect to ") + pAddress + std::string(" at ") + pPort);
		connection->OnConnect.connect(boost::bind(&MassiveMessageManager::OnConnect, this, _1));
		connection->Connect(pAddress,pPort);
	}

	void MassiveMessageManager::Update()
	{
		if(gameServer)
			gameServer->Update();

		if(localPlayer)
			localPlayer->Update();

		Framework::System::Log::Flush();
	}

	Player* MassiveMessageManager::GetLocalPlayer()
	{
		return localPlayer.get();
	}

	void MassiveMessageManager::OnConnect(bool pConnected)
	{
		if(pConnected)
		{
			localPlayer->SetConnection(connection);

			std::string decKey = Framework::System::RandomData(32);
			std::string encKey = Framework::System::RandomData(32);
			std::string decIV = Framework::System::RandomData(8);
			std::string encIV = Framework::System::RandomData(8);

			Framework::Network::Packet packet(1, Framework::Network::Packet::kHandshake);
			packet << decKey << encKey << decIV << encIV;

			localPlayer->SetCipher(new Framework::Crypt::Cipher(encKey, decKey, encIV, decIV));
			localPlayer->Write(packet);

			connection->Start();
		}
	}

	void MassiveMessageManager::SendMessageAll(Framework::Network::Packet& pPacket)
	{
		SendMessage(kPlayerAll, pPacket);
	}

	void MassiveMessageManager::SendMessage(Player::KeyType pKey, Framework::Network::Packet& pPacket)
	{
		if(localPlayer)
		{
			if(pKey == kPlayerServer)
			{
				localPlayer->Write(pPacket);
			}
			else if(pKey == kPlayerSelf)
			{
				localPlayer->ReceivePacket(pPacket);
			}
		}
		if(pKey == kPlayerAll)
		{
			if(gameServer)
				gameServer->SendMessageAll(pPacket);
			if(localPlayer)
				localPlayer->Write(pPacket);
		}
	}

	bool MassiveMessageManager::Server()
	{
		return host;
	}

	GOMDatabase& MassiveMessageManager::GetGOMDatabase() const
	{
		if(gomDatabase)
			return *gomDatabase;
		return gameServer->GetGOMDatabase();
	}
}