#pragma once

#include <System/EventListener.h>
#include <System/Event.h>

namespace Framework{
	namespace System
	{
		struct EventEmitter
		{
			EventEmitter();
			virtual ~EventEmitter();

			void Add(EventListener::pointer pListener);
			void Remove(EventListener::pointer pListener);
			void Dispatch(Event::pointer pEvent);

		private:

			std::vector<EventListener::pointer> mListeners;
			boost::mutex mGuard;
		};
	}
}