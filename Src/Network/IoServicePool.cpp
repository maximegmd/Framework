#include <Network/IoServicePool.h>
#include <System/Log.h>
#include <boost/asio.hpp>
#include <boost/timer.hpp>

using boost::asio::ip::tcp;

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		IoServicePool::IoServicePool(std::size_t pool_size)
			: mNextIndex(0)
		{
			if (pool_size == 0)
				throw std::runtime_error("IoServicePool size is 0");

			for (std::size_t i = 0; i < pool_size; ++i)
			{
				io_service_ptr io_service(new boost::asio::io_service);
				mIoServices.push_back(io_service);
				mRunning.push_back(false);
			}
		}
		//---------------------------------------------------------------------
		IoServicePool::~IoServicePool()
		{
			Stop();
		}
		//---------------------------------------------------------------------
		void IoServicePool::Run()
		{
			// Create a pool of threads to run all of the io_services.
			for (std::size_t i = 0; i < mIoServices.size(); ++i)
			{
				mRunning[i] = true;

				mThreads.push_back(boost::shared_ptr<boost::thread> (new boost::thread(
					[this,i]()
				{
					boost::timer timer;
					while(mRunning[i])
					{
						timer.restart();

						mIoServices[i]->poll();
						mIoServices[i]->reset();

						uint32_t const elapsed = std::uint32_t(timer.elapsed() * 1000);

						if(elapsed < 50 && mRunning[i]){
							Sleep(50 - elapsed);
						}
					}

				})));
			}
		}
		//---------------------------------------------------------------------
		void IoServicePool::Stop()
		{
			// Explicitly Stop all io_services.
			for (std::size_t i = 0; i < mIoServices.size(); ++i){
				mRunning[i] = false;
				mIoServices[i]->stop();
			}

			for (std::size_t i = 0; i < mThreads.size(); ++i)
				mThreads[i]->join();

			mThreads.clear();
			mIoServices.clear();
		}
		//---------------------------------------------------------------------
		boost::asio::io_service& IoServicePool::GetIoService()
		{
			// Use a round-robin scheme to choose the next io_service to use.
			boost::asio::io_service& io_service = *mIoServices[mNextIndex];
			++mNextIndex;
			if (mNextIndex == mIoServices.size())
				mNextIndex = 0;
			return io_service;
		}
		//---------------------------------------------------------------------
	}
}