#pragma once

#include <boost/signal.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <Network/Bfbc2Packet.h>
#include <Network/Strategy.h>
#include <boost/enable_shared_from_this.hpp>
#include <list>

namespace Framework
{
	namespace Network
	{
		/**
		 * @class TcpConnect
		 * @brief Handles a single client as a session
		 */
		class TcpConnection :
			public boost::enable_shared_from_this<TcpConnection>, private boost::noncopyable
		{
		public:

			typedef boost::shared_ptr<TcpConnection> pointer;
			typedef boost::weak_ptr  <TcpConnection> weak_pointer;

			TcpConnection(boost::asio::io_service& pIoService);
			virtual ~TcpConnection();

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
			boost::asio::ip::tcp::socket& GetSocket();

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

			void Connect(const std::string& pAddress, const std::string& pPort);

			boost::signal<void(const std::string&)> OnError;
			boost::signal<void(bool)>			    OnConnect;

		protected:

			void async_write(const std::string& pPacket);
			void async_read();
			void handle_read_header(const boost::system::error_code& e, size_t transfered);
			void handle_read_data(const boost::system::error_code& e, size_t pBytes);
			void handle_resolve(const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator pEndpointItor);
			void handle_connect(const boost::system::error_code& ec);

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

			boost::asio::ip::tcp::socket	mSocket;
			boost::asio::ip::tcp::resolver  mResolver;
		};
	}
}