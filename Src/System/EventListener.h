#pragma once

#include <System/Event.h>
#include <boost/thread.hpp>
#include <queue>

namespace Framework{
	namespace System
	{
		struct EventListener :
			public virtual boost::enable_shared_from_this<EventListener>
		{
			typedef boost::shared_ptr<EventListener> pointer;

			EventListener(){}
			virtual ~EventListener(){}

			virtual void OnEvent(System::Event::pointer pEvent)
			{
				boost::mutex::scoped_lock lock(mLock);
				mEvents.push(pEvent);
			}
		protected:
			boost::mutex mLock;
			std::queue<System::Event::pointer> mEvents;
		};
	}
}