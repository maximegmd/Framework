#include <System/DBWorkQueue.h>
#include <System/Log.h>

namespace Framework{
	namespace System
	{
		//---------------------------------------------------------------------
		DBWorkQueue::DBWorkQueue(const std::string& pDSN, unsigned int pThreadCount)
			:mDSN(pDSN)
		{
			try
			{
				for(size_t i = 0; i < pThreadCount; ++i)
				{
					mThreads.push_back(boost::make_shared<boost::thread>(
						boost::bind(&DBWorkQueue::Run, this)));
				}
			}
			catch(std::exception& e)
			{
				Framework::System::Log::GetInstance()->Error(e.what());
			}
		}
		//---------------------------------------------------------------------
		DBWorkQueue::~DBWorkQueue()
		{
		}
		//---------------------------------------------------------------------
		void DBWorkQueue::Push(boost::shared_ptr<DAO::IDAO> pObject)
		{
			boost::mutex::scoped_lock lock(mLock);
			mJobs.push(pObject);
		}
		//---------------------------------------------------------------------
		void DBWorkQueue::Run()
		{
			try{
				SimpleDB::Database database(mDSN);
				while(1)
				{
					try
					{
						mLock.lock();

						if(!mJobs.empty())
						{
							boost::shared_ptr<DAO::IDAO> task = mJobs.front();
							mJobs.pop();

							mLock.unlock();

							switch(task->GetType())
							{
							case DAO::IDAO::LOAD:
								task->Load(database);break;
							case DAO::IDAO::SAVE:
								task->Save(database);break;
							}
						}
						else
						{
							mLock.unlock();
							boost::this_thread::sleep(boost::posix_time::milliseconds(10));
						}
					}
					catch(...)
					{
					}
				}
			}
			catch(std::exception& e)
			{
				Log::Error(e.what());
			}
		}
		//---------------------------------------------------------------------
	}
}