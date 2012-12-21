#pragma once

#include "GOMObjects.hpp"
#include <cstdint>
#include <memory>
#include <map>

namespace Game
{
	class IGOMServer
	{
	public:

		virtual ~IGOMServer()
		{
		}
		/**
		 * @brief Will query each GOM Entry to see if it's dirty or not.
		 */
		virtual void Update() = 0;
		/**
		 * @brief Remove a GOM Entry from the GOM Server.
		 * @param state The GOM entry's replication state.
		 * @param id The GOM entry's id.
		 */
		virtual void Remove(int32_t state, int32_t id) = 0;
		/**
		 * @brief Get the GOM Server's type.
		 * @return The GOM Server's type.
		 */
		virtual uint32_t GetGroup() const = 0;
		/**
		 * @brief Visits every dirty GOM Entry.
		 * @param op The visitor.
		 * @param pType The type of transaction level to visit.
		 */
		virtual void VisitDirty(GOMVisitor& op, uint32_t pType) = 0;
		/**
		 * @brief Visits all GOM Entries.
		 * @param op The visitor.
		 */
		virtual void VisitAll(GOMVisitor& op) = 0;
		/**
		 * @brief Update an existing entry or add it if it doesn't exist.
		 * @param id The GOM Entry's id.
		 * @param state The GOM Entry's state.
		 * @param serializedData The serialized data associated with it.
		 */
		virtual void UpdateEntry(int32_t id, int32_t state, const std::string& serializedData) = 0;
	};

	enum
	{
		kTransactionNone = 0,
		kTransactionFull = 1 << 0,
		kTransactionPartial = 1 << 1,
		kAllTransactions = (1 << 2) - 1
	};

	template <class EntryType, uint32_t Key >
	class GOMServer : public IGOMServer
	{
	public:

		typedef typename EntryType::Type WrappedType;
		enum{
			kGroupKey = Key
		};

		GOMServer():replicationId(0){}
		virtual ~GOMServer()
		{
			replicationMap[2].clear();
			replicationMap[1].clear();
			replicationMap[0].clear();
		}

		WrappedType* Get(int transaction , int id)
		{
			return replicationMap[transaction][id]->GetData();
		}

		/**
		 * @brief Will query each GOM Entry to see if it's dirty or not.
		 */
		void Update()
		{
			DoUpdate();

			for(auto itor = replicationMap[2].begin(), end = replicationMap[2].end(); itor != end; ++itor)
			{
				if(!itor->second->IsDirty())
					itor->second->Update();
			}

			for(auto itor = replicationMap[1].begin(), end = replicationMap[1].end(); itor != end; ++itor)
			{
				if(!itor->second->IsDirty())
					itor->second->Update();
			}

			for(auto itor = replicationMap[0].begin(), end = replicationMap[0].end(); itor != end; ++itor)
			{
				if(!itor->second->IsDirty())
					itor->second->Update();
			}
		}
		/**
		 * @brief Get the GOM Server's type.
		 * @return The GOM Server's type.
		 */
		uint32_t GetGroup() const
		{
			return Key;
		}

		/**
		 * @brief Add a GOM Entry to the GOM Server.
		 * @param m The Model to add to the GOM Entry.
		 * @param state The GOM entry's replication state.
		 * @param id The GOM entry's id.
		 */
		void Add(WrappedType* m, int32_t state, int32_t id = -1)
		{
			DoAdd(m, state, id);
		}
		/**
		 * @brief Remove a GOM Entry from the GOM Server.
		 * @param state The GOM entry's replication state.
		 * @param id The GOM entry's id.
		 */
		void Remove(int32_t state, int32_t id) override
		{
			removedIds.push_back(id);
			DoRemove(state, id);
		}
		/**
		 * @brief Visits all GOM Entries.
		 * @param op The visitor.
		 */
		void VisitAll(GOMVisitor& op)
		{
			for(auto itor = replicationMap[2].begin(), end = replicationMap[2].end(); itor != end; ++itor)
			{
				op(GetGroup(), itor->first, 2, itor->second.get());
			}

			for(auto itor = replicationMap[1].begin(), end = replicationMap[1].end(); itor != end; ++itor)
			{
				op(GetGroup(),itor->first, 1, itor->second.get());
			}

			for(auto itor = replicationMap[0].begin(), end = replicationMap[0].end(); itor != end; ++itor)
			{
				op(GetGroup(),itor->first, 0, itor->second.get());
			}
		}
		/**
		 * @brief Visits every dirty GOM Entry.
		 * @param op The visitor.
		 */
		void VisitDirty(GOMVisitor& op, uint32_t pType)
		{
			if(pType & kTransactionPartial)
			{
				for(auto itor = replicationMap[kTransactionPartial].begin(), end = replicationMap[kTransactionPartial].end(); itor != end; ++itor)
				{
					if(itor->second->IsDirty())
					{
						op(GetGroup(), itor->first, kTransactionPartial, itor->second.get());
						itor->second->SetDirty(false);
					}
				}
			}

			if(pType & kTransactionFull)
			{
				for(auto itor = replicationMap[kTransactionFull].begin(), end = replicationMap[kTransactionFull].end(); itor != end; ++itor)
				{
					if(itor->second->IsDirty())
					{
						op(GetGroup(), itor->first, kTransactionFull, itor->second.get());
						itor->second->SetDirty(false);
					}
				}
			}

			for(auto itor = removedIds.begin(), end = removedIds.end(); itor != end; ++itor)
				op(GetGroup(), *itor);
			removedIds.clear();
		}

	protected:

		virtual void DoAdd(WrappedType*, int32_t, int32_t = -1) = 0;
		virtual void DoRemove(int32_t, int32_t) = 0;
		virtual void DoUpdate() = 0;

		int32_t							replicationId;
		std::map<int32_t, std::shared_ptr<EntryType> >    replicationMap[3];
		std::list<int32_t>				removedIds;
	};
}