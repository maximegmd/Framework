#include "GOMObjects.hpp"

namespace Game
{
	GOMState::GOMState()
		:id(-1), state(0), gomEntry(nullptr), full(false)
	{
	}

	GOMState::GOMState(int32_t a, int32_t b, IGOMEntry* c, bool pFull)
		:id(a), state(b), gomEntry(c), full(pFull)
	{
	}

	Framework::Network::Packet& operator<<(Framework::Network::Packet& packet, const GOMState& state)
	{
		packet << state.id << state.state << state.gomEntry->Serialize(state.full);
		return packet;
	}

	Framework::Network::Packet& operator>>(Framework::Network::Packet& packet, GOMStateRaw& state)
	{
		packet >> state.id >> state.state >> state.data;
		return packet;
	}

	void GOMVisitor::operator()(int32_t type, int32_t id, int32_t state, IGOMEntry* accessor, bool full)
	{
		gomEntries[type].push_back(GOMState(id, state, accessor, full));
	}

	void GOMVisitor::operator()(int32_t type, int32_t id)
	{
		gomDeleted[type].push_back(id);
	}
}