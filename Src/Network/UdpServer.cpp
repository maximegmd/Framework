#include <Network/UdpServer.h>
#include <System/Log.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		UdpServer::UdpServer(unsigned short pPort)
			:mPort(pPort),mStarted(false)
		{
		}
		//---------------------------------------------------------------------
		void UdpServer::Scale()
		{
			unsigned int logicThreads = 1;

			mIoPool.reset(new IoServicePool(2));
			mAcceptor.reset(new boost::asio::ip::tcp::acceptor(mIoPool->GetIoService(), tcp::endpoint(tcp::v4(), mPort)));

			System::Log::Print("Running with : - 2 Network thread(s) ");
			System::Log::Print("               - " + std::to_string((unsigned long long)logicThreads) + " Logic thread(s)");
			System::Log::Print("");
		}
		//---------------------------------------------------------------------
		void UdpServer::Start()
		{
			// note : Order matters !
			Scale();

			System::Log::Print("Waiting for connections.");
			mStarted = true;

			mIoPool->Run();

			Run();
		}
		//---------------------------------------------------------------------
		void UdpServer::Run()
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
		void UdpServer::RunOnce()
		{
			mTotalElapsed += uint32_t(mTimer.elapsed() * 1000);
			mTimer.restart();

			OnUpdate(mTotalElapsed);

			System::Log::Flush();

			mTotalElapsed = uint32_t(mTimer.elapsed() * 1000);
			mTimer.restart();
		}
		//---------------------------------------------------------------------
		void UdpServer::Stop()
		{
			mStarted = false;
		}
		//---------------------------------------------------------------------
	}
}