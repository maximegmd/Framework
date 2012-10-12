#pragma once

#include <Network/IoServicePool.h>
#include <System/DBWorkQueue.h>
#include <System/WorkQueue.h>
#include <boost/signal.hpp>

namespace Framework
{
	namespace Network
	{
		class UdpServer
			: public boost::enable_shared_from_this<UdpServer>
			, private boost::noncopyable
		{
		public:

			/**
			 * Instantiate the UdpServer
			 * @param pPort The port to start the UdpServer on
			 */
			UdpServer(uint16_t pPort);
			/**
			 * Start the UdpServer, will return on exit
			 */
			void Start();
			/**
			 * Authentication and sync loop
			 */
			void Run();

			void RunOnce();
			/**
			 * Stops the UdpServer, Run will return
			 */
			void Stop();

			boost::signal<void(uint32_t)>				OnUpdate;

		protected:
			/**
			 * Scale the thread count according to the runtime infos.
			 */
			void Scale();

		private:

			std::unique_ptr<boost::asio::ip::tcp::acceptor>			mAcceptor;
			std::unique_ptr<IoServicePool>							mIoPool;
			uint16_t												mPort;
			uint32_t												mTotalElapsed;

			boost::mutex							mGuard;
			boost::timer							mTimer;

			bool mStarted;
		};
	}
}