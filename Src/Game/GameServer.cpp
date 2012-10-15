#include "GameServer.hpp"
#include <random>
#include <System/Log.h>

namespace Game
{
	GameServer::GameServer(short port, GameServer::PlayerConstructor playerCtor, GameServer::GOMServerConstructor gomCtor)
		:playerContructor(playerCtor), gomServerConstructor(gomCtor), gomServer(gomCtor(0))
	{
		server.reset(new Framework::Network::Server(port));
		server->OnConnection.connect(boost::bind(&GameServer::OnConnection, this, _1));
		server->Start();
	}

	GameServer::~GameServer()
	{
		server->Stop();

		for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
			delete itor->second;

		players.clear();
	}

	void GameServer::Update()
	{
		for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
			itor->second->Update();
	}

	void GameServer::SendMessageAll(Framework::Network::Packet& pPacket)
	{
		for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
		{
			itor->second->Write(pPacket);
		}
	}

	void GameServer::SetCellSize(int cellsize)
	{
		this->cellSize = cellsize;
	}

	int GameServer::GetCellSize() const
	{
		return cellSize;
	}

	IGOMServer& GameServer::GetGOMServer() const
	{
		return *gomServer.get();
	}

	void GameServer::OnConnection(Framework::Network::TcpConnection::pointer pConnection)
	{
		Player::KeyType key;
		std::random_device rd;
		do{
			key = rd() + 1;
		}while(players.find(key) != players.end());

		Player* player = playerContructor ? playerContructor(key, this) : new Player(key, this);
		player->SetConnection(pConnection);

		players[key] = player;
	}
}