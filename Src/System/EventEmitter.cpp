#include <System/EventEmitter.h>

namespace Framework{
	namespace System{
		//---------------------------------------------------------------------
		EventEmitter::EventEmitter()
		{
		}
		//---------------------------------------------------------------------
		EventEmitter::~EventEmitter()
		{
		}
		//---------------------------------------------------------------------
		void EventEmitter::Add(EventListener::pointer pListener)
		{
			boost::mutex::scoped_lock lock(mGuard);
			mListeners.push_back(pListener);
		}
		//---------------------------------------------------------------------
		void EventEmitter::Remove(EventListener::pointer pListener)
		{
			boost::mutex::scoped_lock lock(mGuard);
			auto itor = std::find(mListeners.begin(), mListeners.end(), pListener);
			if(itor != mListeners.end())
			{
				mListeners.erase(itor);
			}
		}
		//---------------------------------------------------------------------
		void EventEmitter::Dispatch(System::Event::pointer pEvent)
		{
			boost::mutex::scoped_lock lock(mGuard);
			std::for_each(mListeners.begin(), mListeners.end(),
				[&](EventListener::pointer pListener)
				{
					pListener->OnEvent(pEvent);
				});
		}
		//---------------------------------------------------------------------
	}
}