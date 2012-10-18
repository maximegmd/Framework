#include "GOMDatabase.hpp"
#include "GOMServer.hpp"

namespace Game
{
	GOMDatabase::GOMDatabase(std::vector<IGOMServer*> servers)
	{
		for(auto itor = servers.begin(), end = servers.end(); itor != end; ++itor)
		{
			gomServers[(*itor)->GetGroup()].reset(*itor);
		}
	}

	void GOMDatabase::VisitAll(uint32_t key, GOMVisitor& op)
	{
		if(key == kAllGOMServers)
		{
			for(auto itor = gomServers.begin(), end = gomServers.end(); itor != end; ++itor)
			{
				itor->second->VisitAll(op);
			}
		}
		else
		{
			auto server = Get(key);
			if(server)
				server->VisitAll(op);
		}
	}

	void GOMDatabase::VisitDirty(uint32_t key, uint32_t pType, GOMVisitor& op)
	{
		if(key == kAllGOMServers)
		{
			for(auto itor = gomServers.begin(), end = gomServers.end(); itor != end; ++itor)
			{
				itor->second->VisitDirty(op, pType);
			}
		}
		else
		{
			auto server = Get(key);
			if(server)
				server->VisitDirty(op, pType);
		}
	}

	IGOMServer* GOMDatabase::Get(uint32_t key)
	{
		return gomServers[key].get();
	}

	void GOMDatabase::Add(IGOMServer* server)
	{
		if(server)
			gomServers[server->GetGroup()].reset(server);
	}
}