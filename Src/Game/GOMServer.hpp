#pragma once

#include "Accessors.hpp"
#include <list>

namespace Game
{
	struct GOMState{
		GOMState(int32_t a, int32_t b, IAccessor* c)
			:id(a), state(b), accessor(c){}
		int32_t id, state;
		IAccessor* accessor;	
	};

	struct GOMVisitor{
		void operator()(int32_t id, int32_t state, IAccessor* accessor){
			accessors.push_back(GOMState(id, state, accessor));
		}

		std::list<GOMState> accessors;
	};

	class IGOMServer
	{
	public:
		virtual void Update() = 0;

		virtual void VisitDirty(GOMVisitor& op) = 0;
		virtual void VisitAll(GOMVisitor& op) = 0;
	};

	template <class Model, class Acc = Accessor<Model> >
	class GOMServer : public IGOMServer
	{
	public:

		GOMServer():replicationId(0){}
		virtual ~GOMServer(){}

		void Update()
		{
			DoUpdate();

			for(auto itor = replicationMap[2].begin(), end = replicationMap[2].end(); itor != end; ++itor){
				if(!itor->second.IsDirty())
					itor->second.Update();
			}

			for(auto itor = replicationMap[1].begin(), end = replicationMap[1].end(); itor != end; ++itor){
				if(!itor->second.IsDirty())
					itor->second.Update();
			}

			for(auto itor = replicationMap[0].begin(), end = replicationMap[0].end(); itor != end; ++itor){
				if(!itor->second.IsDirty())
					itor->second.Update();
			}
		}

		void Add(Model* m, int32_t state, int32_t id = -1)
		{
			DoAdd(m, state, id);
		}

		void Remove(int32_t state, int32_t id)
		{
			DoRemove(state, id);
		}

		void VisitAll(GOMVisitor& op)
		{
			for(auto itor = replicationMap[2].begin(), end = replicationMap[2].end(); itor != end; ++itor){
				op(itor->first, 2, &itor->second);
			}

			for(auto itor = replicationMap[1].begin(), end = replicationMap[1].end(); itor != end; ++itor){
				op(itor->first, 1, &itor->second);
			}

			for(auto itor = replicationMap[0].begin(), end = replicationMap[0].end(); itor != end; ++itor){
				op(itor->first, 0, &itor->second);
			}
		}

		void VisitDirty(GOMVisitor& op)
		{
			for(auto itor = replicationMap[2].begin(), end = replicationMap[2].end(); itor != end; ++itor){
				if(itor->second.IsDirty()){
					op(itor->first, 2, &itor->second);
					itor->second.SetDirty(false);
				}
			}
			
			for(auto itor = replicationMap[1].begin(), end = replicationMap[1].end(); itor != end; ++itor){
				if(itor->second.IsDirty()){
					op(itor->first, 1, &itor->second);
					itor->second.SetDirty(false);
				}
			}
		}

	protected:

		virtual void DoAdd(Model*, int32_t, int32_t = -1) = 0;
		virtual void DoRemove(int32_t, int32_t) = 0;
		virtual void DoUpdate() = 0;

		int32_t					  replicationId;
		std::map<int32_t, Acc>    replicationMap[3];
	};
}