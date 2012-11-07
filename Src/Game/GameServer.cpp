#include "GameServer.hpp"
#include <random>
#include <System/Log.h>
#include <Game/MassiveMessageManager.hpp>

namespace Game
{
	GameServer::GameServer(short port, GameServer::PlayerConstructor playerCtor, GameServer::GOMServerConstructor gomCtor)
		:playerContructor(playerCtor)
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

		Framework::System::Log::Debug("GameServer deleted");
	}

	void GameServer::Update()
	{
		TheMassiveMessageMgr->GetGOMDatabase()->Update();

		if(mTransactionFullTimer.elapsed() > 0.1)
		{
			GOMVisitor visitor;
			TheMassiveMessageMgr->GetGOMDatabase()->VisitDirty(GOMDatabase::kAllGOMServers, kTransactionFull, visitor);
			SendReplicationTransaction(visitor);
			mTransactionFullTimer.restart();
		}
		else if(mTransactionPartialTimer.elapsed() > 1.0)
		{
			GOMVisitor visitor;
			TheMassiveMessageMgr->GetGOMDatabase()->VisitDirty(GOMDatabase::kAllGOMServers, kTransactionPartial, visitor);
			SendReplicationTransaction(visitor);
			mTransactionPartialTimer.restart();
		}

		boost::mutex::scoped_lock _(lock);
		for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
			itor->second->Update();
	}

	void GameServer::SendMessageAll(Framework::Network::Packet& pPacket)
	{
		boost::mutex::scoped_lock _(lock);
		for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
		{
			itor->second->Write(pPacket);
		}
	}

	void GameServer::SendMessageAllSynchronized(Framework::Network::Packet& pPacket)
	{
		boost::mutex::scoped_lock _(lock);
		for(auto itor = players.begin(), end = players.end(); itor != end; ++itor)
		{
			if(itor->second->Synchronized())
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

	void GameServer::OnConnection(Framework::Network::TcpConnection::pointer pConnection)
	{
		boost::mutex::scoped_lock _(lock);

		Player::KeyType key;
		std::random_device rd;
		do{
			key = rd()%std::numeric_limits<int32_t>::max() + 1;
		}while(players.find(key) != players.end());

		Player* player = playerContructor ? playerContructor(key, this) : new Player(key, this);
		player->SetConnection(pConnection);

		std::ostringstream os;
		os << "GameServer : New player with key : " << key;
		Framework::System::Log::Debug(os.str());

		players[key] = player;
	}

	void GameServer::Remove(Player* player)
	{
		boost::mutex::scoped_lock _(lock);
		auto itor = players.find(player->GetKey());
		if(itor != players.end())
		{
			players.erase(itor);
			delete player;
		}
	}

	void GameServer::SendReplicationTransaction(GOMVisitor& visitor)
	{
		// Don't send if we have nothing to send...
		if(visitor.gomEntries.empty() && visitor.gomDeleted.empty())
			return;

		Framework::Network::Packet packet(Player::kReplicationTransaction);

		uint8_t flags = 0;
		packet << flags;
		if(visitor.gomEntries.size())
		{
			flags |= kReplicationUpdate;
			packet << visitor.gomEntries;
		}
		if(visitor.gomDeleted.size())
		{
			Framework::System::Log::Debug("Sending removal");
			flags |= kReplicationRemove;
			packet << visitor.gomDeleted;
		}

		packet.Write(&flags, 1, 0);

		SendMessageAllSynchronized(packet);
	}
}