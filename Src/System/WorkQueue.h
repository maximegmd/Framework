#pragma once

#include <System/Job.h>
#include <boost/thread.hpp>
#include <list>
#include <queue>


namespace Framework{
	namespace System
	{
		class WorkQueue
		{
		public:

			WorkQueue(unsigned int pThreadCount);
			~WorkQueue();

			void Run();

			void Add(Job* task);

		private:
			boost::recursive_mutex mLock;
			std::queue<Job*> mJobs;
			std::list<std::shared_ptr<boost::thread>> mThreads;
		};
	}
}