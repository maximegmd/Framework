#include "Player.hpp"

namespace Game
{
	void Player::SendReplicationTransaction(IGOMServer& gomServer)
	{
		Framework::Network::Packet packet(kReplicationTransaction);

		GOMVisitor visitor;
		gomServer.VisitAll(visitor);

		std::for_each(visitor.gomEntries.begin(), visitor.gomEntries.end(), [](GOMState& state){state.full = true;});

		packet << visitor.gomEntries;

		Write(packet);
	}
}