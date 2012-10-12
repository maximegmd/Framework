#pragma once

#include <DAO/DAO.h>
#include <boost/thread.hpp>
#include <queue>

namespace Framework{
	namespace System
	{
		class DBWorkQueue
		{
		public:

			enum Type{
				SAVE,
				LOAD
			};

			DBWorkQueue(const std::string& pDSN, unsigned int pThreadCount);
			~DBWorkQueue();

			void Run();

			void Push(boost::shared_ptr<DAO::IDAO> pObject);

		private:

			boost::mutex mLock;
			const std::string mDSN;
			std::queue<boost::shared_ptr<DAO::IDAO>> mJobs;
			std::list<boost::shared_ptr<boost::thread>> mThreads;
		};
	}
}