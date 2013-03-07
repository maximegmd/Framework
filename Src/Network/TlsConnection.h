#pragma once

#include <boost/signal.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <openssl/ssl.h>
#include <openssl/err.h>
#pragma comment(lib, "libeay32MTd.lib")
#pragma comment(lib, "ssleay32MTd.lib")
#pragma comment(lib, "ssleay32MTd.lib")

#include <boost/thread.hpp>

#include <Network/Strategy.h>
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
		class TlsConnection :
			public boost::enable_shared_from_this<TlsConnection>, private boost::noncopyable
		{
		public:

			typedef boost::shared_ptr<TlsConnection> pointer;
			typedef boost::weak_ptr  <TlsConnection> weak_pointer;

			TlsConnection(boost::asio::io_service& pIoService, boost::asio::ssl::context& context);
			virtual ~TlsConnection();

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
			 * True if queue > 0, false otherwise
			 * @return
			 */
			bool HasPacket();

			/**
			 * Too complicated to be explained xD
			 */
			bool IsOffline();

			void SetStrategy(IStrategy* pStrategy);

			void OnTimeout(const boost::system::error_code& e);

			boost::signal<void(const std::string&)> OnError;
			boost::signal<void(bool)>			    OnConnect;

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
			std::vector<char>  mInboundHeader;
			std::vector<char>  mInboundData;

			IStrategy* mStrategy;
			boost::mutex mPacketLock;

			std::list<std::string> mToSend;

			boost::asio::deadline_timer mTimeout;

			ssl_socket						mSocket;
			boost::asio::ip::tcp::resolver  mResolver;
		};
	}
}