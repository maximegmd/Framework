#pragma once

#include <cstdint>

namespace Game
{
	class ReplicationStateProperty {
	public:

		enum State{
			kStaticReplication = 0,
			kPartialReplication = 1,
			kFullReplication = 2,
			kCount
		};

		ReplicationStateProperty();
		ReplicationStateProperty(const State& state);
		~ReplicationStateProperty();

		int32_t GetState() const
		{
			return theState;	
		}

	protected:

		int32_t theState;

		ReplicationStateProperty(const ReplicationStateProperty& state);
	};
}