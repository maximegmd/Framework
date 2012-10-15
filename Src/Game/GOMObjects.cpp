#include "GOMObjects.hpp"

namespace Game
{
	GOMState::GOMState()
		:id(-1), state(0), accessor(nullptr)
	{
	}

	GOMState::GOMState(int32_t a, int32_t b, IAccessor* c)
		:id(a), state(b), accessor(c)
	{
	}

	Framework::Network::Packet& operator<<(Framework::Network::Packet& packet, const GOMState& state)
	{
		packet << state.id << state.state << state.accessor->Serialize();
		return packet;
	}

	Framework::Network::Packet& operator>>(Framework::Network::Packet& packet, GOMStateRaw& state)
	{
		packet >> state.id >> state.state >> state.data;
		return packet;
	}

	void GOMVisitor::operator()(int32_t id, int32_t state, IAccessor* accessor)
	{
		accessors.push_back(GOMState(id, state, accessor));
	}
}