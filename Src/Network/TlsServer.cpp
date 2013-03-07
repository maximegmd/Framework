#include <Network/TlsServer.h>
#include <System/Log.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		TlsServer::TlsServer(unsigned short pPort)
			:mPort(pPort),mStarted(false)
		{
		}
		//---------------------------------------------------------------------
		std::string get_password() {
			return "test";
		}
		//---------------------------------------------------------------------
		void TlsServer::Scale()
		{
			unsigned int logicThreads = boost::thread::hardware_concurrency();

			mIoPool.reset(new IoServicePool(1));
			mAcceptor.reset(new boost::asio::ip::tcp::acceptor(mIoPool->GetIoService(), tcp::endpoint(tcp::v4(), mPort)));
			mContext.reset(new boost::asio::ssl::context(boost::asio::ssl::context::tlsv1));
			try {
				mContext->set_options(boost::asio::ssl::context::default_workarounds |
					boost::asio::ssl::context::no_sslv2);
				mContext->set_password_callback(boost::bind(get_password));
				mContext->use_certificate_chain_file("ssl/server.pem");
				mContext->use_private_key_file("ssl/server.pem", boost::asio::ssl::context::pem);
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
			}

			System::Log::Print("Server started : 0.0.0.0:" + std::to_string((unsigned long long)mPort));
			System::Log::Print("Running with : " + std::to_string((unsigned long long)logicThreads) + " Network thread(s)");
			System::Log::Print("");
		}
		//---------------------------------------------------------------------
		TlsServer::~TlsServer()
		{
			mAcceptor->cancel();
			mAcceptor.reset(nullptr);
		}
		//---------------------------------------------------------------------
		void TlsServer::Start()
		{
			// note : Order matters !
			Scale();
			Accept();

			System::Log::Print("Waiting for connections.");
			mStarted = true;

			mIoPool->Run();
		}
		//---------------------------------------------------------------------
		void TlsServer::Run()
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
		void TlsServer::RunOnce()
		{
			mTotalElapsed += uint32_t(mTimer.elapsed() * 1000);
			mTimer.restart();

			OnUpdate(mTotalElapsed);

			System::Log::Flush();

			mTotalElapsed = uint32_t(mTimer.elapsed() * 1000);
			mTimer.restart();
		}
		//---------------------------------------------------------------------
		void TlsServer::Stop()
		{
			mStarted = false;
		}
		//---------------------------------------------------------------------
		void TlsServer::Accept()
		{
			TlsConnection::pointer session = boost::make_shared<TlsConnection>(mIoPool->GetIoService(), *mContext);
			mAcceptor->async_accept(session->GetSocket().lowest_layer(),
				boost::bind(&TlsServer::HandleAccept, this,
				session, boost::asio::placeholders::error));
		}
		//---------------------------------------------------------------------
		void TlsServer::HandleAccept(TlsConnection::pointer pSession, const boost::system::error_code& pError)
		{
			if(!pError)
			{
				OnConnection(pSession);
				pSession->Start();

				Accept();
			}
			else
			{
				System::Log::Error(pError.message());
				System::Log::Flush();
			}
		}
		//---------------------------------------------------------------------
	}
}