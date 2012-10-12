#if !defined(NO_SSL)

#include <Network/SslServer.h>
#include <System/Log.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		SslServer::SslServer(unsigned short pPort)
			:mPort(pPort),mStarted(false)
		{
		}
		//---------------------------------------------------------------------
		void SslServer::Setup(const uint8_t* pX509, const uint8_t* pRsa, uint32_t pX509Len, uint32_t pRsaLen)
		{
			unsigned int logicThreads = 1;

			mIoPool.reset(new IoServicePool(1));
			
			mAcceptor.reset(new boost::asio::ip::tcp::acceptor(mIoPool->GetIoService(), tcp::endpoint(tcp::v4(), mPort)));
			mContext.reset(new boost::asio::ssl::context(mAcceptor->get_io_service(), boost::asio::ssl::context::sslv23));
			mContext->set_options(SSL_OP_ALL);
			SSL_CTX_set_cipher_list(mContext->native_handle(), "ALL");
			SSL_CTX_use_certificate_ASN1(mContext->native_handle(), pX509Len, pX509);
			SSL_CTX_use_PrivateKey_ASN1(EVP_PKEY_RSA, mContext->native_handle(), pRsa, pRsaLen);
			SSL_CTX_set_verify_depth(mContext->native_handle(), 1);

			System::Log::Print("SSL Server started : 0.0.0.0:" + std::to_string((unsigned long long)mPort));
			System::Log::Print("Running with : - 2 Network thread(s) ");
			System::Log::Print("               - " + std::to_string((unsigned long long)logicThreads) + " Logic thread(s)");
			System::Log::Print("");
		}
		//---------------------------------------------------------------------
		void SslServer::Start(const uint8_t* pX509, const uint8_t* pRsa, uint32_t pX509Len, uint32_t pRsaLen)
		{
			// note : Order matters !
			Setup(pX509, pRsa, pX509Len, pRsaLen);
			Accept();

			System::Log::Print("Waiting for connections.");
			mStarted = true;

			mIoPool->Run();
		}
		//---------------------------------------------------------------------
		void SslServer::Run()
		{
			mTotalElapsed = 0;

			while(mStarted)
			{
				RunOnce();

				if(mTotalElapsed < 50)
				{
					// This thread doesn't need to be very responsive
					boost::this_thread::sleep(boost::posix_time::milliseconds(50 - mTotalElapsed));
				}
			}
		}
		//---------------------------------------------------------------------
		void SslServer::RunOnce()
		{
			mTotalElapsed += uint32_t(mTimer.elapsed() * 1000);
			mTimer.restart();

			OnUpdate(mTotalElapsed);

			System::Log::Flush();

			mTotalElapsed = uint32_t(mTimer.elapsed() * 1000);
			mTimer.restart();
		}
		//---------------------------------------------------------------------
		void SslServer::Stop()
		{
			mStarted = false;
		}
		//---------------------------------------------------------------------
		void SslServer::Accept()
		{
			SslConnection::pointer session = boost::make_shared<SslConnection>(mIoPool->GetIoService(), *mContext);
			mAcceptor->async_accept(session->GetSocket().lowest_layer(),
				boost::bind(&SslServer::HandleAccept, this,
				session, boost::asio::placeholders::error));
		}
		//---------------------------------------------------------------------
		void SslServer::HandleAccept(SslConnection::pointer pSession, const boost::system::error_code& pError)
		{
			if(!pError)
			{
				OnConnection(pSession);
				pSession->Start();
			}
			else
			{
				System::Log::Error(pError.message());
			}
			Accept();
		}
		//---------------------------------------------------------------------
	}
}

#endif