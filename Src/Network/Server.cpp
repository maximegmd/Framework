#include <Network/Server.h>
#include <System/Log.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		Server::Server(unsigned short pPort)
			:mPort(pPort),mStarted(false)
		{
		}
		//---------------------------------------------------------------------
		void Server::Scale()
		{
			unsigned int logicThreads = boost::thread::hardware_concurrency();

			mIoPool.reset(new IoServicePool(logicThreads));
			mAcceptor.reset(new boost::asio::ip::tcp::acceptor(mIoPool->GetIoService(), tcp::endpoint(tcp::v4(), mPort)));

			System::Log::Print("Server started : 0.0.0.0:" + std::to_string((unsigned long long)mPort));
			System::Log::Print("Running with : " + std::to_string((unsigned long long)logicThreads) + " Network thread(s)");
			System::Log::Print("");
		}
		//---------------------------------------------------------------------
		Server::~Server()
		{
			mAcceptor->cancel();
			mAcceptor.reset(nullptr);
		}
		//---------------------------------------------------------------------
		void Server::Start()
		{
			// note : Order matters !
			Scale();
			Accept();

			System::Log::Print("Waiting for connections.");
			mStarted = true;

			mIoPool->Run();
		}
		//---------------------------------------------------------------------
		void Server::Run()
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
		void Server::RunOnce()
		{
			mTotalElapsed += uint32_t(mTimer.elapsed() * 1000);
			mTimer.restart();

			OnUpdate(mTotalElapsed);

			System::Log::Flush();

			mTotalElapsed = uint32_t(mTimer.elapsed() * 1000);
			mTimer.restart();
		}
		//---------------------------------------------------------------------
		void Server::Stop()
		{
			mStarted = false;
		}
		//---------------------------------------------------------------------
		void Server::Accept()
		{
			TcpConnection::pointer session = boost::make_shared<TcpConnection>(mIoPool->GetIoService());
			mAcceptor->async_accept(session->GetSocket(),
				boost::bind(&Server::HandleAccept, this,
				session, boost::asio::placeholders::error));
		}
		//---------------------------------------------------------------------
		void Server::HandleAccept(TcpConnection::pointer pSession, const boost::system::error_code& pError)
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