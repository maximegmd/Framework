#if !defined(NO_SSL)

#pragma once

#include <Network/SslConnection.h>
#include <Network/IoServicePool.h>
#include <System/DBWorkQueue.h>
#include <System/WorkQueue.h>
#include <boost/signal.hpp>


namespace Framework
{
	namespace Network
	{
		class SslServer
			: public boost::enable_shared_from_this<SslServer>
			, private boost::noncopyable
		{
		public:

			/**
			 * Instantiate the SslServer
			 * @param pPort The port to start the SslServer on
			 */
			SslServer(uint16_t pPort);
			/**
			 * Start the SslServer, will return on exit
			 */
			void Start(const uint8_t* pX509, const uint8_t* pRsa, uint32_t pX509Len, uint32_t pRsaLen);
			/**
			 * Authentication and sync loop
			 */
			void Run();

			void RunOnce();
			/**
			 * Stops the SslServer, Run will return
			 */
			void Stop();

			boost::signal<void(SslConnection::pointer)>	OnConnection;
			boost::signal<void(uint32_t)>				OnUpdate;

		protected:

			void Accept();
			void HandleAccept(SslConnection::pointer pSession, const boost::system::error_code& pError);

			/**
			 * Scale the thread count according to the runtime infos.
			 */
			void Setup(const uint8_t* pX509, const uint8_t* pRsa, uint32_t pX509Len, uint32_t pRsaLen);

		private:

			std::unique_ptr<boost::asio::ip::tcp::acceptor>			mAcceptor;
			std::unique_ptr<IoServicePool>							mIoPool;
			uint16_t												mPort;
			uint32_t												mTotalElapsed;

			boost::mutex							mGuard;
			boost::timer							mTimer;

			std::unique_ptr<boost::asio::ssl::context>				mContext;

			bool mStarted;
		};
	}
}

#endif