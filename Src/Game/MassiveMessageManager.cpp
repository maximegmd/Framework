
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
		server(nullptr)
	{
		
	}

	MassiveMessageManager::~MassiveMessageManager()
	{
		if(server)
			server->Stop();
		
		for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
			delete itor->second;

		players.clear();
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

	void MassiveMessageManager::SetPlayerConstructor(PlayerConstructor& ctor)
	{
		playerContructor = ctor;
	}

	void MassiveMessageManager::SetGOMServerConstructor(GOMServerConstructor& ctor)
	{
		gomServerConstructor = ctor;
	}

	void MassiveMessageManager::BeginMultiplayer(bool pHost)
	{
		if(!gomServerConstructor)
		{
			throw std::runtime_error("No GOM Server constructor set.");
		}

		host = pHost;
		//if(host)
		{
			server.reset(new Framework::Network::Server(port));
			server->OnConnection.connect(boost::bind(&MassiveMessageManager::OnConnection, this, _1));
			server->Start();
		}
		//else
		{
			ioServicePool.Run();
			connection.reset(::new Framework::Network::TcpConnection(ioServicePool.GetIoService()));
			Connect(address, std::to_string((long long)port));
		}

		gomServer.reset(gomServerConstructor(nullptr));
	}

	void MassiveMessageManager::Connect(const std::string& pAddress, const std::string& pPort)
	{
		connection->OnConnect.connect(boost::bind(&MassiveMessageManager::OnConnect, this, _1));
		connection->Connect(pAddress,pPort);
	}

	void MassiveMessageManager::Query()
	{
		for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
		{
			itor->second->Update();
		}

		if(localPlayer)
			localPlayer->Update();

		Framework::System::Log::Flush();
	}

	Player* MassiveMessageManager::GetLocalPlayer()
	{
		return localPlayer.get();
	}

	void MassiveMessageManager::OnConnection(Framework::Network::TcpConnection::pointer pConnection)
	{
		Player::KeyType key;
		std::random_device rd;
		do{
			key = rd() + 1;
		}while(players.find(key) != players.end());

		Player* player = playerContructor ? playerContructor(key, nullptr) : new Player(key);
		player->SetConnection(pConnection);

		players[key] = player;
	}

	void MassiveMessageManager::OnConnect(bool pConnected)
	{
		if(pConnected)
		{
			Player* player = playerContructor ? playerContructor(kPlayerSelf, nullptr) : new Player(kPlayerSelf);
			localPlayer.reset(player);
			localPlayer->SetConnection(connection);

			std::string decKey = Framework::System::RandomData(32);
			std::string encKey = Framework::System::RandomData(32);
			std::string decIV = Framework::System::RandomData(8);
			std::string encIV = Framework::System::RandomData(8);

			Framework::Network::Packet packet(1, Framework::Network::Packet::kHandshake);
			packet << decKey << encKey << decIV << encIV;

			player->SetCipher(new Framework::Crypt::Cipher(encKey, decKey, encIV, decIV));
			player->Write(packet);

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
			for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
			{
				itor->second->Write(pPacket);
			}
			if(localPlayer)
				localPlayer->Write(pPacket);
		}
	}

	bool MassiveMessageManager::Server()
	{
		return host;
	}
}