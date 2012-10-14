#pragma once

namespace Game
{
	class IGOMServer
	{
	public:

		IGOMServer(){

		}

		virtual ~IGOMServer(){

		}

		virtual void Update() = 0;
	};

	template <class Model>
	class GOMServer : public IGOMServer
	{
	public:

		GOMServer():replicationId(0){

		}

		virtual ~GOMServer(){

		}

		virtual void Add(Model*, int32_t, int32_t = -1) = 0;
		virtual void Remove(int32_t, int32_t) = 0;
		virtual void Update() = 0;

	protected:

		int32_t					  replicationId;
		std::map<int32_t, Model*> replicationMap[3];
	};
}