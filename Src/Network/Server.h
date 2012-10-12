#pragma once

#include <Network/TcpConnection.h>
#include <Network/IoServicePool.h>
#include <System/DBWorkQueue.h>
#include <System/WorkQueue.h>
#include <boost/signal.hpp>


namespace Framework
{
	namespace Network
	{
		class Server
			: public boost::enable_shared_from_this<Server>
			, private boost::noncopyable
		{
		public:

			/**
			 * Instantiate the server
			 * @param pPort The port to start the server on
			 */
			Server(uint16_t pPort);

			~Server();
			/**
			 * Start the server, will return on exit
			 */
			void Start();
			/**
			 * Authentication and sync loop
			 */
			void Run();

			void RunOnce();
			/**
			 * Stops the server, Run will return
			 */
			void Stop();

			boost::signal<void(TcpConnection::pointer)>	OnConnection;
			boost::signal<void(uint32_t)>				OnUpdate;

		protected:

			void Accept();
			void HandleAccept(TcpConnection::pointer pSession, const boost::system::error_code& pError);

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