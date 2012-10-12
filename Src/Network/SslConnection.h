#if !defined(NO_SSL)

#pragma once

#include <Network/Strategy.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <openssl/ssl.h>
#include <openssl/err.h>
#pragma comment(lib, "libeay32MTd.lib")
#pragma comment(lib, "ssleay32MTd.lib")
#pragma comment(lib, "ssleay32MTd.lib")

#include <boost/thread.hpp>
#include <Network/Bfbc2Packet.h>
#include <boost/enable_shared_from_this.hpp>
#include <list>


namespace Framework
{
	namespace Network
	{
		typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;
		/**
		 * @class TcpConnect
		 * @brief Handles a single client as a session
		 */
		class SslConnection :
			public boost::enable_shared_from_this<SslConnection>, private boost::noncopyable
		{
		public:

			typedef boost::shared_ptr<SslConnection> pointer;

			SslConnection(boost::asio::io_service& pIoService, boost::asio::ssl::context& context);
			virtual ~SslConnection();

			/**
			 * Starts listening to the session
			 */
			void Start();

			/**
			 * Write a packet to the session, non blocking, undefined timing
			 * @param pData The packet to send
			 */
			void Write(const std::string& pData);
			/**
			 * Close the TcpConnect, will trigger scalar destruction
			 */
			void Close();

			/**
			 * Get the session's socket
			 * @return The socket
			 */
			ssl_socket& GetSocket();

			/**
			 * Too complicated to be explained xD
			 */
			bool IsOffline();

			void OnTimeout(const boost::system::error_code& e);

			void SetStrategy(IStrategy* pStrategy);

		protected:

			void async_write(const std::string& pPacket);
			void async_read();
			void handle_read_header(const boost::system::error_code& e, size_t transfered);
			void handle_read_data(const boost::system::error_code& e, size_t pBytes);
			void handle_handshake(const boost::system::error_code& e);

			void HandleRead	(const boost::system::error_code& pError);
			void HandleWrite(const boost::system::error_code& pError);
			void DoWrite(std::string data);

		private:

			std::string mOutboundData; //< Outbound data buffer, kept for reference
			std::vector<char>  mInboundHeader; //< Header buffer
			std::vector<char>  mInboundData; //< Buffer

			IStrategy*	mStrategy;

			std::list<std::string> mToSend;

			boost::asio::deadline_timer mTimeout;

			ssl_socket	mSocket;
		};
	}
}

#endif