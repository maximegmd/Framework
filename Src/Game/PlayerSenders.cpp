#include "Player.hpp"

namespace Game
{
	void Player::SendReplicationTransaction(IGOMServer& gomServer)
	{
		Framework::Network::Packet packet(kReplicationTransaction);
		GOMVisitor visitor;
		gomServer.VisitAll(visitor);
		for(auto itor = visitor.accessors.begin(), end = visitor.accessors.end(); itor != end; ++itor)
		{
			GOMState& state = *itor;
			packet << state.id << state.state << state.accessor->Serialize();
		}

		Write(packet);
	}
}