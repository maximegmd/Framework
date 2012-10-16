#pragma once

#include <map>
#include <vector>
#include <cstdint>

#include <Game/GOMObjects.hpp>
#include <Game/GOMServer.hpp>

namespace Game
{
	class GOMDatabase
	{
	public:

		enum{
			kAllGOMServers = -1
		};

		GOMDatabase(std::vector<IGOMServer*> servers);

		IGOMServer* Get(uint32_t key);

		void VisitAll(uint32_t key, GOMVisitor& op);
		void VisitDirty(uint32_t key, GOMVisitor& op);

	protected:

		friend class GameServer;

		void Add(IGOMServer* server);

	private:

		std::map<uint32_t, std::unique_ptr<IGOMServer>> gomServers;
	};
}