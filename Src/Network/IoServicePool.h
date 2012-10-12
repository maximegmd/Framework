#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <cstdint>

namespace Framework
{
	namespace Network
	{
		class IoServicePool
			: private boost::noncopyable
		{
		public:
			explicit IoServicePool(std::size_t pool_size);
			~IoServicePool();

			void Run();

			void Stop();

			boost::asio::io_service& GetIoService();

		private:
			typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
			typedef boost::shared_ptr<boost::asio::io_service::work> work_ptr;

			std::vector<io_service_ptr> mIoServices;
			std::vector<work_ptr> mWork;
			std::vector<boost::shared_ptr<boost::thread> > mThreads;

			// Using std::atomic <bool> here (or boost equivalent) would be preferable
			std::vector<bool> mRunning;

			std::size_t mNextIndex;
		};
	}
}