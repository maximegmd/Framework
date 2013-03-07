#include <Network/TlsConnection.h>
#include <System/Log.h>
#include <System/Tools.h>
#include <fstream>

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		TlsConnection::TlsConnection(boost::asio::io_service& pIoService, boost::asio::ssl::context& context)
			: mSocket(pIoService, context), mTimeout(pIoService), mResolver(pIoService), mStrategy(nullptr)
		{
		}
		//---------------------------------------------------------------------
		TlsConnection::~TlsConnection()
		{
			OnError.disconnect_all_slots();
			OnConnect.disconnect_all_slots();
		}
		//---------------------------------------------------------------------
		void TlsConnection::Close()
		{
			if(mSocket.lowest_layer().is_open())
			{
				boost::system::error_code e;
				mSocket.lowest_layer().shutdown(boost::asio::socket_base::shutdown_both, e);
				mSocket.lowest_layer().close(e);
			}
			OnError("Close");

			OnError.disconnect_all_slots();
		}
		//---------------------------------------------------------------------
		void TlsConnection::OnTimeout(const boost::system::error_code& e)
		{
			if(e != boost::asio::error::operation_aborted)
			{
				Close();
			}
		}
		//---------------------------------------------------------------------
		void TlsConnection::Start()
		{
			mTimeout.expires_from_now(boost::posix_time::minutes(5));
			mTimeout.async_wait(boost::bind(&TlsConnection::OnTimeout, shared_from_this(), _1));

			mSocket.lowest_layer().set_option(boost::asio::ip::tcp::no_delay(false));
			mSocket.set_verify_mode(boost::asio::ssl::context::verify_none);
			mSocket.async_handshake(boost::asio::ssl::stream_base::server,
				boost::bind(&TlsConnection::handle_handshake, shared_from_this(), boost::asio::placeholders::error));
			async_read();
		}
		//---------------------------------------------------------------------
		void TlsConnection::Write(const std::string& pData)
		{
			mSocket.get_io_service().post(boost::bind(&TlsConnection::DoWrite, shared_from_this(), pData));
		}
		//---------------------------------------------------------------------
		void TlsConnection::DoWrite(std::string pData)
		{
			bool write_in_progress = !mToSend.empty();
			mToSend.push_back(pData);
			if (!write_in_progress)
			{
				async_write(mToSend.front());
			}
		}
		//---------------------------------------------------------------------
		void TlsConnection::HandleRead(const boost::system::error_code& pError)
		{
			if(!pError)
			{
				async_read();
			}
			else
			{
				System::Log::Error(pError.message());
				System::Log::Flush();
				Close();
			}
		}
		//---------------------------------------------------------------------
		void TlsConnection::HandleWrite(const boost::system::error_code& pError)
		{
			if(!pError)
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&TlsConnection::OnTimeout, shared_from_this(), _1));

				mToSend.pop_front();
				if (!mToSend.empty())
				{
					async_write(mToSend.front());
				}
			}
			else
			{
				Close();
			}
		}
		//---------------------------------------------------------------------
		ssl_socket& TlsConnection::GetSocket()
		{
			return mSocket;
		}
		//---------------------------------------------------------------------
		void TlsConnection::async_write(const std::string& pData)
		{
			mOutboundData = pData;

			boost::asio::async_write(mSocket, boost::asio::buffer(mOutboundData), boost::bind(&TlsConnection::HandleWrite,shared_from_this(), boost::asio::placeholders::error));
		}
		//---------------------------------------------------------------------
		void TlsConnection::async_read()
		{
			// Read the header
			mInboundHeader.resize(mStrategy->GetHeaderSize());
			boost::asio::async_read(mSocket, boost::asio::buffer(mInboundHeader), boost::asio::transfer_exactly(mStrategy->GetHeaderSize()),
				boost::bind(&TlsConnection::handle_read_header,shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}
		//---------------------------------------------------------------------
		void TlsConnection::handle_read_header(const boost::system::error_code& e, size_t transfered)
		{
			if (e)
			{
				this->HandleRead(e);
			}
			else
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&TlsConnection::OnTimeout, shared_from_this(), _1));

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
					boost::bind(&TlsConnection::handle_read_data, shared_from_this(),
					boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			}
		}
		//---------------------------------------------------------------------
		void TlsConnection::handle_read_data(const boost::system::error_code& e, size_t pBytes)
		{
			// If error tell the caller.
			if (e)
			{
				this->HandleRead(e);
			}
			else
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&TlsConnection::OnTimeout, shared_from_this(), _1));
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
		void TlsConnection::handle_handshake(const boost::system::error_code& e)
		{
			if (e)
			{
				this->HandleRead(e);
			}
			else
			{
				mTimeout.expires_from_now(boost::posix_time::minutes(5));
				mTimeout.async_wait(boost::bind(&TlsConnection::OnTimeout, shared_from_this(), _1));
				async_read();
			}
		}
		//---------------------------------------------------------------------
		bool TlsConnection::IsOffline()
		{
			return !mSocket.lowest_layer().is_open();
		}
		//---------------------------------------------------------------------
		void TlsConnection::SetStrategy(IStrategy* pStrategy)
		{
			mStrategy = pStrategy;
		}
		//---------------------------------------------------------------------
	}
}