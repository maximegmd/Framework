#include "NetProperties.hpp"

namespace Game
{
	ReplicationStateProperty::ReplicationStateProperty()
		:theState(ReplicationStateProperty::kStaticReplication)
	{
	}

	ReplicationStateProperty::ReplicationStateProperty(const ReplicationStateProperty& state)
		:theState(state.theState)
	{
	}

	ReplicationStateProperty::ReplicationStateProperty(const ReplicationStateProperty::State& state)
		:theState(state)
	{
	}

	ReplicationStateProperty::~ReplicationStateProperty()
	{

	}
}