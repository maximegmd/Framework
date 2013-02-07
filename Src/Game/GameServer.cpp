#include "GameServer.hpp"
#include <random>
#include <System/Log.h>
#include <Game/MassiveMessageManager.hpp>

namespace Game
{
	GameServer::GameServer(short port, GameServer::PlayerConstructor playerCtor, GameServer::GOMServerConstructor gomCtor)
		:mPlayerContructor(playerCtor)
	{
		mServer.reset(new Framework::Network::Server(port));
		mServer->OnConnection.connect(boost::bind(&GameServer::OnConnection, this, _1));
		mServer->Start();
	}

	GameServer::~GameServer()
	{
		mServer->Stop();

		for(auto itor = mPlayers.begin(), end = mPlayers.end(); itor != end; ++itor)
			delete itor->second;

		mPlayers.clear();

		Framework::System::Log::Debug("GameServer deleted");
	}

	void GameServer::Update()
	{
		TheMassiveMessageMgr->GetGOMDatabase()->Update();

		if(mTransactionFullTimer.elapsed() > 0.1)
		{
			GOMVisitor visitor;

			uint32_t flag = kTransactionFull;

			if(mTransactionPartialTimer.elapsed() > 1.0)
			{
				flag |= kTransactionPartial;
				mTransactionPartialTimer.restart();
			}

			TheMassiveMessageMgr->GetGOMDatabase()->VisitDirty(GOMDatabase::kAllGOMServers, flag, visitor);
			SendReplicationTransaction(visitor);
			mTransactionFullTimer.restart();
		}

		boost::recursive_mutex::scoped_lock _(mLock);
		for(auto itor = mPlayers.begin(), end = mPlayers.end(); itor != end; ++itor)
			itor->second->Update();

		for(auto itor = mToRemove.begin(), end = mToRemove.end(); itor != end; ++itor)
		{
			auto itor2 = mPlayers.find((*itor)->GetKey());
			if(itor2 != mPlayers.end())
			{
				mPlayers.erase(itor2);
				delete *itor;
			}
		}
		mToRemove.clear();
	}

	void GameServer::SendMessageAll(Framework::Network::Packet& pPacket)
	{
		boost::recursive_mutex::scoped_lock _(mLock);
		for(auto itor = mPlayers.begin(), end = mPlayers.end(); itor != end; ++itor)
		{
			itor->second->Write(pPacket);
		}
	}

	void GameServer::SendMessageAllSynchronized(Framework::Network::Packet& pPacket)
	{
		boost::recursive_mutex::scoped_lock _(mLock);
		for(auto itor = mPlayers.begin(), end = mPlayers.end(); itor != end; ++itor)
		{
			if(itor->second->Synchronized())
				itor->second->Write(pPacket);
		}
	}

	void GameServer::SetCellSize(int cellsize)
	{
		this->mCellSize = cellsize;
	}

	int GameServer::GetCellSize() const
	{
		return mCellSize;
	}

	void GameServer::OnConnection(Framework::Network::TcpConnection::pointer pConnection)
	{
		boost::recursive_mutex::scoped_lock _(mLock);

		Player::KeyType key;
		std::random_device rd;
		do{
			key = rd()%std::numeric_limits<int32_t>::max() + 1;
		}while(mPlayers.find(key) != mPlayers.end() || key == TheMassiveMessageMgr->GetLocalPlayer()->GetKey());

		Player* player = mPlayerContructor ? mPlayerContructor(key, this) : new Player(key, this);
		player->SetConnection(pConnection);

		std::ostringstream os;
		os << "GameServer : New player with key : " << key;
		Framework::System::Log::Debug(os.str());

		mPlayers[key] = player;
	}

	void GameServer::SendReplicationTransaction(GOMVisitor& visitor)
	{
		// Don't send if we have nothing to send...
		if(visitor.gomEntries.empty() && visitor.gomDeleted.empty())
			return;

		GOMTransaction transaction;

		if(visitor.gomEntries.size())
		{
			transaction.SetUpdateMap(visitor.gomEntries);
		}
		if(visitor.gomDeleted.size())
		{
			transaction.SetDeleteList(visitor.gomDeleted);
		}


		SendMessageAllSynchronized(transaction.ToPacket(Player::kReplicationTransaction));
	}

	void GameServer::Remove(Player* player)
	{
		boost::recursive_mutex::scoped_lock _(mLock);
		mToRemove.push_back(player);
	}

	Player* GameServer::GetPlayer(Player::KeyType pKey)
	{
		boost::recursive_mutex::scoped_lock _(mLock);
		auto itor = mPlayers.find(pKey);
		if(itor != mPlayers.end())
			return itor->second;
		return nullptr;
	}
}