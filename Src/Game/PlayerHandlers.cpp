#include "Player.hpp"
#include <System/Log.h>
#include <Game/MassiveMessageManager.hpp>

namespace Game
{
	void Player::HandleReplicationTransaction(Framework::Network::Packet& pPacket)
	{
		std::ostringstream os;
		os << "\nReplication Transaction Log\n\n";

		std::map<int32_t, std::list<GOMStateRaw> > states;
		std::map<int32_t, std::list<int32_t> > deleted;
		pPacket >> states;

		os << "\tUpdate Transaction :";
		for(auto itor = states.begin(), end = states.end(); itor != end; ++itor)
		{
			IGOMServer* server = TheMassiveMessageMgr->GetGOMDatabase()->Get(itor->first);
			os << "\tGroup : " << server->GetGroup() << std::endl;
			for(auto itor2 = itor->second.begin(), end2 = itor->second.end(); itor2 != end2; ++itor2)
			{
				server->UpdateEntry(itor2->id, itor2->state, itor2->data);
				os << "\t\tId: " << itor2->id << " , State : " << itor2->state << std::endl;
			}
		}

		os << "\tDelete transaction :";
		for(auto itor = deleted.begin(), end = deleted.end(); itor != end; ++itor)
		{
			IGOMServer* server = TheMassiveMessageMgr->GetGOMDatabase()->Get(itor->first);
			os << "\tGroup : " << server->GetGroup() << std::endl;
			for(auto itor2 = itor->second.begin(), end2 = itor->second.end(); itor2 != end2; ++itor2)
			{
				server->Remove(kTransactionFull, *itor2);
				server->Remove(kTransactionPartial, *itor2);
				server->Remove(kTransactionNone, *itor2);

				os << "\t\tId: " << *itor2 << std::endl;
			}
		}
		Framework::System::Log::Print(os.str());
	}

	void Player::HandleAwareness(Framework::Network::Packet& pPacket)
	{
		SendReplicationTransaction(gameServer->GetGOMDatabase());
	}
}