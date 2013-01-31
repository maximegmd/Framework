#include "Player.hpp"
#include <System/Log.h>
#include <Game/MassiveMessageManager.hpp>
#include <System/Tools.h>

namespace Game
{
	void Player::HandleReplicationTransaction(Framework::Network::Packet& pPacket)
	{
		std::ostringstream os;
		os << "\nReplication Transaction Log\n\n";

		GOMTransactionRaw transaction;
		pPacket >> transaction;

		if(transaction.IsSetUpdateMap())
		{
			std::map<int32_t, std::list<GOMStateRaw> > states = transaction.GetUpdateMap();
			pPacket >> states;

			os << "\tUpdate Transaction :";
			for(auto itor = states.begin(), end = states.end(); itor != end; ++itor)
			{
				IGOMServer* server = TheMassiveMessageMgr->GetGOMDatabase()->Get(itor->first);
				os << "\tGroup : " << Framework::System::IntToString(itor->first) << std::endl;
				for(auto itor2 = itor->second.begin(), end2 = itor->second.end(); itor2 != end2; ++itor2)
				{
					if(server)
						server->UpdateEntry(itor2->id, itor2->state, itor2->data);
					os << "\t\tId: " << itor2->id << " , State : " << itor2->state << std::endl;
				}
			}
		}
		if(transaction.IsSetDeleteList())
		{
			std::map<int32_t, std::list<uint32_t> > deleted = transaction.GetDeleteList();
		
			os << "\tDelete transaction :";
			for(auto itor = deleted.begin(), end = deleted.end(); itor != end; ++itor)
			{
				IGOMServer* server = TheMassiveMessageMgr->GetGOMDatabase()->Get(itor->first);
				os << "\tGroup : " << Framework::System::IntToString(itor->first) << std::endl;
				for(auto itor2 = itor->second.begin(), end2 = itor->second.end(); itor2 != end2; ++itor2)
				{
					if(server)
					{
						server->Remove(kTransactionFull, *itor2);
						server->Remove(kTransactionPartial, *itor2);
						server->Remove(kTransactionNone, *itor2);
					}
					
					os << "\t\tId: " << *itor2 << std::endl;
				}
			}
		}
		Framework::System::Log::Print(os.str());
	}

	void Player::HandleAwareness(Framework::Network::Packet& pPacket)
	{
		SendReplicationTransaction(TheMassiveMessageMgr->GetGOMDatabase());
	}
}