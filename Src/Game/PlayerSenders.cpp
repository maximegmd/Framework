#include "Player.hpp"

namespace Game
{
	void Player::SendReplicationTransaction(IGOMServer& gomServer)
	{
		Framework::Network::Packet packet(kReplicationTransaction);

		GOMVisitor visitor;
		gomServer.VisitAll(visitor);

		packet << visitor.gomEntries;

		Write(packet);
	}
}