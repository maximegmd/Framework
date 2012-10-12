#if !defined(NO_SSL)

#include <Network/SslConnection.h>
#include <Network/Bfbc2Packet.h>
#include <System/Log.h>
#include <System/Tools.h>
#include <fstream>

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		SslConnection::SslConnection(boost::asio::io_service& pIoService, boost::asio::ssl::context& context)
			: mSocket(pIoService, context), mTimeout(pIoService), mStrategy(nullptr)
		{
		}
		//---------------------------------------------------------------------
		SslConnection::~SslConnection()
		{
		}
		//---------------------------------------------------------------------
		void SslConnection::Close()
		{
			if(mSocket.lowest_layer().is_open())
			{
				boost::system::error_code e;
				mSocket.lowest_layer().shutdown(boost::asio::socket_base::shutdown_both, e);
				mSocket.lowest_layer().close(e);
			}
		}
		//---------------------------------------------------------------------
		void SslConnection::OnTimeout(const boost::system::error_code& e)
		{
			if(e != boost::asio::error::operation_aborted)
			{
				Close();
			}
		}
		//---------------------------------------------------------------------
		void SslConnection::Start()
		{
			mTimeout.expires_from_now(boost::posix_time::minutes(5));
			mTimeout.async_wait(boost::bind(&SslConnection::OnTimeout, shared_from_this(), _1));

			mSocket.lowest_layer().set_option(boost::asio::ip::tcp::no_delay(false));
			mSocket.async_handshake(boost::asio::ssl::stream_base::server, 
				boost::bind(&SslConnection::handle_handshake, shared_from_this(), boost::asio::placeholders::error));
		}
		//---------------------------------------------------------------------
		void SslConnection::Write(const std::string& pData)
		{
			mSocket.get_io_service().post(boost::bind(&SslConnection::DoWrite, shared_from_this(), pData));
		}
		//---------------------------------------------------------------------
		void SslConnection::DoWrite(std::string pData)
		{
			bool write_in_progress = !mToSend.empty();
			mToSend.push_back(pData);
			if (!write_in_progress)
			{
				async_write(mToSend.front());
			}
		}
		//---------------------------------------------------------------------
		void SslConnection::HandleRead(const boost::system::error_code& pError)
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
		void SslConnection::HandleWrite(const boost::system::error_code& pError)
		{
			if(!pError)
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&SslConnection::OnTimeout, shared_from_this(), _1));

				mToSend.pop_front();
				if (!mToSend.empty())
				{
					async_write(mToSend.front());
				}
			}
			else
			{
				System::Log::Debug("Handle write failed.");
				Close();
			}
		}
		//---------------------------------------------------------------------
		ssl_socket& SslConnection::GetSocket()
		{
			return mSocket;
		}
		//---------------------------------------------------------------------
		void SslConnection::async_write(const std::string& pPacket)
		{
			mOutboundData = pPacket;

			boost::asio::async_write(mSocket, boost::asio::buffer(mOutboundData), boost::bind(&SslConnection::HandleWrite,shared_from_this(), boost::asio::placeholders::error));
		}
		//---------------------------------------------------------------------
		void SslConnection::async_read()
		{
			// Read the header
			mInboundHeader.clear();
			mInboundHeader.resize(mStrategy->GetHeaderSize());
			boost::asio::async_read(mSocket, boost::asio::buffer(mInboundHeader), boost::asio::transfer_exactly(mStrategy->GetHeaderSize()),
				boost::bind(&SslConnection::handle_read_header,shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}
		//---------------------------------------------------------------------
		void SslConnection::handle_read_header(const boost::system::error_code& e, size_t transfered)
		{
			if (e)
			{
				// Socket Error
				System::Log::Debug("Handle read header failed (SE).");
				this->HandleRead(e);
			}
			else
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&SslConnection::OnTimeout, shared_from_this(), _1));

				std::string data(mInboundHeader.data(), mInboundHeader.size());

				uint32_t size = mStrategy->HandleHeader(data);

				if(size > 1<<16)
				{
					System::Log::Debug("Handle read header failed.");
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
					boost::bind(&SslConnection::handle_read_data, shared_from_this(),
					boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			}
		}
		//---------------------------------------------------------------------
		void SslConnection::handle_handshake(const boost::system::error_code& e)
		{
			if (e)
			{
				this->HandleRead(e);
			}
			else
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&SslConnection::OnTimeout, shared_from_this(), _1));
				async_read();
			}
		}
		//---------------------------------------------------------------------
		void SslConnection::handle_read_data(const boost::system::error_code& e, size_t pBytes)
		{
			// If error tell the caller.
			if (e)
			{
				this->HandleRead(e);
			}
			else
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&SslConnection::OnTimeout, shared_from_this(), _1));
				// Try to handle the payload
				try
				{
					mStrategy->HandleContent(std::string(mInboundData.data(), pBytes));
					mInboundData.clear();
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
		bool SslConnection::IsOffline()
		{
			return !mSocket.lowest_layer().is_open();
		}
		//---------------------------------------------------------------------
		void SslConnection::SetStrategy(IStrategy* pStrategy)
		{
			mStrategy = pStrategy;
		}
		//---------------------------------------------------------------------
	}
}
#endif
