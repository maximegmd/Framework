#include <Network/TcpConnection.h>
#include <Network/Bfbc2Packet.h>
#include <System/Log.h>
#include <System/Tools.h>
#include <fstream>

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		TcpConnection::TcpConnection(boost::asio::io_service& pIoService)
			: mSocket(pIoService), mTimeout(pIoService), mResolver(pIoService), mStrategy(nullptr)
		{
		}
		//---------------------------------------------------------------------
		TcpConnection::~TcpConnection()
		{
		}
		//---------------------------------------------------------------------
		void TcpConnection::Close()
		{
			if(mSocket.is_open())
			{
				boost::system::error_code e;
				mSocket.shutdown(boost::asio::socket_base::shutdown_both, e);
				mSocket.close(e);
			}
		}
		//---------------------------------------------------------------------
		void TcpConnection::OnTimeout(const boost::system::error_code& e)
		{
			if(e != boost::asio::error::operation_aborted)
			{
				Close();
			}
		}
		//---------------------------------------------------------------------
		void TcpConnection::Start()
		{
			mTimeout.expires_from_now(boost::posix_time::minutes(5));
			mTimeout.async_wait(boost::bind(&TcpConnection::OnTimeout, shared_from_this(), _1));
			async_read();
		}
		//---------------------------------------------------------------------
		void TcpConnection::Write(const std::string& pData)
		{
			mSocket.get_io_service().post(boost::bind(&TcpConnection::DoWrite, shared_from_this(), pData));
		}
		//---------------------------------------------------------------------
		void TcpConnection::DoWrite(std::string pData)
		{
			bool write_in_progress = !mToSend.empty();
			mToSend.push_back(pData);
			if (!write_in_progress)
			{
				async_write(mToSend.front());
			}
		}
		//---------------------------------------------------------------------
		void TcpConnection::HandleRead(const boost::system::error_code& pError)
		{
			if(!pError)
			{
				async_read();
			}
			else
			{
				Close();
			}
		}
		//---------------------------------------------------------------------
		void TcpConnection::HandleWrite(const boost::system::error_code& pError)
		{
			if(!pError)
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&TcpConnection::OnTimeout, shared_from_this(), _1));

				mToSend.pop_front();
				if (!mToSend.empty())
				{
					async_write(mToSend.front());
				}
			}
			else
			{
				System::Log::Debug("handle write failed.");
				Close();
			}
		}
		//---------------------------------------------------------------------
		boost::asio::ip::tcp::socket& TcpConnection::GetSocket()
		{
			return mSocket;
		}
		//---------------------------------------------------------------------
		void TcpConnection::async_write(const std::string& pData)
		{
			mOutboundData = pData;

			boost::asio::async_write(mSocket, boost::asio::buffer(mOutboundData), boost::bind(&TcpConnection::HandleWrite,shared_from_this(), boost::asio::placeholders::error));
		}
		//---------------------------------------------------------------------
		void TcpConnection::async_read()
		{
			// Read the header
			mInboundHeader.resize(mStrategy->GetHeaderSize());
			boost::asio::async_read(mSocket, boost::asio::buffer(mInboundHeader), boost::asio::transfer_exactly(mStrategy->GetHeaderSize()),
				boost::bind(&TcpConnection::handle_read_header,shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}
		//---------------------------------------------------------------------
		void TcpConnection::handle_read_header(const boost::system::error_code& e, size_t transfered)
		{
			if (e)
			{
				this->HandleRead(e);
			}
			else
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&TcpConnection::OnTimeout, shared_from_this(), _1));

				std::string data(mInboundHeader.data(), mInboundHeader.size());

				uint32_t size = mStrategy->HandleHeader(data);

				if(size > 1<<16)
				{
					System::Log::Debug("handle read header failed.");
					boost::system::error_code error(boost::asio::error::invalid_argument);
					this->HandleRead(error);
					return;
				}
				else if(size == 0)
				{
					async_read();
					return;
				}

				// Ready up the buffer
				mInboundData.resize(size);

				// Read the buffer
				boost::asio::async_read(mSocket, boost::asio::buffer(mInboundData), boost::asio::transfer_exactly(mInboundData.size()),
					boost::bind(&TcpConnection::handle_read_data, shared_from_this(),
					boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			}
		}
		//---------------------------------------------------------------------
		void TcpConnection::handle_read_data(const boost::system::error_code& e, size_t pBytes)
		{
			// If error tell the caller.
			if (e)
			{
				this->HandleRead(e);
			}
			else
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&TcpConnection::OnTimeout, shared_from_this(), _1));
				// Try to handle the payload
				try
				{
					mStrategy->HandleContent(std::string(mInboundData.data(), pBytes));
				}
				catch (...)
				{
					// Unable to decode data.
					boost::system::error_code error(boost::asio::error::invalid_argument);
					this->HandleRead(error);
					return;
				}

				// Inform caller that data has been received ok.
				this->HandleRead(e);
			}
		}
		//---------------------------------------------------------------------
		bool TcpConnection::IsOffline()
		{
			return !mSocket.is_open();
		}
		//---------------------------------------------------------------------
		void TcpConnection::SetStrategy(IStrategy* pStrategy)
		{
			mStrategy = pStrategy;
		}
		//---------------------------------------------------------------------
		void TcpConnection::Connect(const std::string& pAddress, const std::string& pPort)
		{
			boost::asio::ip::tcp::resolver::query query(pAddress, pPort);
			mResolver.async_resolve(query, boost::bind(&TcpConnection::handle_resolve, this, 
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));
		}
		//---------------------------------------------------------------------
		void TcpConnection::handle_resolve(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator pEndpointItor)
		{
			if(!ec)
			{
				boost::asio::async_connect(mSocket, pEndpointItor, boost::bind(&TcpConnection::handle_connect, this, boost::asio::placeholders::error));
			}
			else
			{
				OnError(ec.message());
			}
		}
		//---------------------------------------------------------------------
		void TcpConnection::handle_connect(const boost::system::error_code& ec)
		{
			if(!ec)
			{
				OnConnect(true);
				//this->Start();
			}
			else
			{
				OnConnect(false);
				OnError(ec.message());
			}
		}
		//---------------------------------------------------------------------
	}
}