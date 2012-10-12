#include <memory>
#include <boost/thread.hpp>
#include <System/WorkQueue.h>

namespace Framework { namespace System
{
	//---------------------------------------------------------------------
	WorkQueue::WorkQueue(unsigned int pThreadCount)
	{
		for(size_t i = 0; i < pThreadCount; ++i)
		{
			mThreads.push_back(std::make_shared<boost::thread>(boost::bind(&WorkQueue::Run, this)));
		}
	}
	//---------------------------------------------------------------------
	WorkQueue::~WorkQueue()
	{
	}
	//---------------------------------------------------------------------
	void WorkQueue::Run()
	{
		while(1)
		{
			mLock.lock();
			if(!mJobs.empty())
			{
				Job* job = mJobs.front();
				mJobs.pop();

				mLock.unlock();

				job->Work();
			}
			else
			{
				mLock.unlock();
				boost::this_thread::sleep(boost::posix_time::milliseconds(10));
			}
		}
	}
	//---------------------------------------------------------------------
	void WorkQueue::Add(Job* task)
	{
		boost::recursive_mutex::scoped_lock lock(mLock);
		mJobs.push(task);
	}
	//---------------------------------------------------------------------
}
}