#include "Player.hpp"
#include <System/Log.h>
#include <Game/MassiveMessageManager.hpp>

namespace Game
{
	void Player::HandleReplicationTransaction(Framework::Network::Packet& pPacket)
	{
		std::ostringstream os;
		os << "\nReplication Transaction Log\n\n";

		std::map<uint32_t, std::list<GOMStateRaw> > states;
		pPacket >> states;

		for(auto itor = states.begin(), end = states.end(); itor != end; ++itor)
		{
			IGOMServer* server = TheMassiveMessageMgr->GetGOMDatabase().Get(itor->first);
			os << "Group : " << server->GetGroup() << std::endl;
			for(auto itor2 = itor->second.begin(), end2 = itor->second.end(); itor2 != end2; ++itor2)
			{
				server->UpdateEntry(itor2->id, itor2->state, itor2->data);
				os << "\tId: " << itor2->id << " , State : " << itor2->state << std::endl;
			}
		}
		Framework::System::Log::Print(os.str());
	}
}