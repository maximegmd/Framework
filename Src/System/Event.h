#pragma once

#include <boost/enable_shared_from_this.hpp>

namespace Framework{
	namespace System
	{
		struct Event : public boost::enable_shared_from_this<Event>
		{
			typedef boost::shared_ptr<Event> pointer;

			Event(){}
			virtual ~Event(){}

			enum Type{
				DATAOBJECT_LOAD,
				DATAOBJECT_LOAD_FAIL,
				DATAOBJECT_SAVE,
				DATAOBJECT_SAVE_FAIL
			};
			virtual Type type() = 0;
			unsigned int userData;
		};
	}
}