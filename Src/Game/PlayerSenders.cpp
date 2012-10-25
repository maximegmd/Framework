#include "Player.hpp"
#include <Game/GOMDatabase.hpp>
#include <System/Log.h>

namespace Game
{
	void Player::SendReplicationTransaction(GOMDatabase* gomDatabase)
	{
		Framework::Network::Packet packet(kReplicationTransaction);

		GOMVisitor visitor;
		gomDatabase->VisitAll(Game::GOMDatabase::kAllGOMServers, visitor);

		visitor.apply([](GOMState& state){state.full = true;});

		uint8_t flags = 0;
		packet << flags;
		if(visitor.gomEntries.size())
		{
			flags |= kReplicationUpdate;
			packet << visitor.gomEntries;
		}
		if(visitor.gomDeleted.size())
		{
			flags |= kReplicationRemove;
			packet << visitor.gomDeleted;
		}
		
		packet.Write(&flags, 1, 0);

		Write(packet);

		synchronized = true;
	}

	void Player::SendAwareness()
	{
		Framework::Network::Packet packet(kAwareness);
		Write(packet);
	}
}