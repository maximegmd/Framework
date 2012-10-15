#pragma once

#include "GOMEntry.hpp"
#include <cstdint>
#include <list>

#include <Network/Packet.h>

namespace Game
{
	struct GOMState
	{
		GOMState();
		GOMState(int32_t a, int32_t b, IGOMEntry* c);

		int32_t id, state;
		IGOMEntry* accessor;	

		friend Framework::Network::Packet& operator<<(Framework::Network::Packet& pPacket, const GOMState& state);
	};

	struct GOMStateRaw
	{
		int32_t id, state;
		std::string data;	

		friend Framework::Network::Packet& operator>>(Framework::Network::Packet& pPacket, GOMStateRaw& state);
	};

	struct GOMVisitor
	{
		void operator()(int32_t id, int32_t state, IGOMEntry* accessor);

		std::list<GOMState> accessors;
	};
}