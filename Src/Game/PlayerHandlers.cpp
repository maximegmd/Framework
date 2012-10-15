#include "Player.hpp"
#include <System/Log.h>
#include <Game/MassiveMessageManager.hpp>

namespace Game
{
	void Player::HandleReplicationTransaction(Framework::Network::Packet& pPacket)
	{
		std::ostringstream os;
		os << "\nReplication Transaction Log\n\n";

		std::list<GOMStateRaw> states;
		pPacket >> states;

		for(auto itor = states.begin(), end = states.end(); itor != end; ++itor)
		{
			TheMassiveMessageMgr->GetGOMServer().UpdateEntry(itor->id, itor->state, itor->data);
			os << "Id: " << itor->id << " , State : " << itor->state << std::endl;
		}
		Framework::System::Log::Debug(os.str());
	}
}