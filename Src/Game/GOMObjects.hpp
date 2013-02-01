#pragma once

#include "GOMEntry.hpp"
#include <cstdint>
#include <list>

#include <Network/Packet.h>
#include <Serialization/Serialization.hpp>

namespace Game
{
	struct GOMState
	{
		/**
		 * @brief Constructor.
		 */
		GOMState();
		/**
		 * @brief Constructor.
		 * @param id The id of the GOM Entry in the GOM Server.
		 * @param state The state of the GOM Entry in the GOM Server.
		 * @param gomEntry The GOM Entry.
		 * @param pFull The level of sync.
		 */
		GOMState(int32_t id, int32_t state, IGOMEntry* gomEntry, bool pFull);

		int32_t id, state;
		IGOMEntry* gomEntry;
		bool full;

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
		/**
		 * @brief Visits a GOM Server.
		 * @param id The id of the GOM Entry in the GOM Server.
		 * @param state The state of the GOM Entry in the GOM Server.
		 * @param gomEntry The GOM Entry.
		 * @param full The level of sync.
		 * @return
		 */
		void operator()(int32_t type, int32_t id, int32_t state, IGOMEntry* gomEntry, bool full = false);
		void operator()(int32_t type, int32_t id);

		template <class Op>
		void apply(Op& op)
		{
			for(auto itor = gomEntries.begin(), end = gomEntries.end(); itor != end; ++itor)
				std::for_each(itor->second.begin(), itor->second.end(), op);
		}

		std::map<int32_t, std::list<GOMState> > gomEntries;
		std::map<int32_t, std::list<uint32_t> > gomDeleted;
	};

	struct GOMTransaction : public BasicSerializable<
		SwitchedSerializable<uint16_t,
		SwitchedField<0x0001, std::map<int32_t, std::list<GOMState> > >,
		SwitchedField<0x0002, std::map<int32_t, std::list<uint32_t> > >
		>
	>
	{
		ACCESSOR_2(0, 0, UpdateMap);
		ACCESSOR_2(0, 1, DeleteList);
	};

	struct GOMTransactionRaw : public BasicSerializable<
		SwitchedSerializable<uint16_t,
		SwitchedField<0x0001, std::map<int32_t, std::list<GOMStateRaw> > >,
		SwitchedField<0x0002, std::map<int32_t, std::list<uint32_t> > >
		>
	>
	{
		ACCESSOR_2(0, 0, UpdateMap);
		ACCESSOR_2(0, 1, DeleteList);
	};
}