//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <Network/IoServicePool.h>
#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "Connection.hpp"
#include "RequestHandler.hpp"

namespace Framework {
	namespace Network {

		/// The top-level class of the HTTP server.
		class HttpServer
			: private boost::noncopyable
		{
		public:
			/// Construct the server to listen on the specified TCP address and port, and
			/// serve up files from the given directory.
			explicit HttpServer(unsigned short pPort, std::size_t thread_pool_size);

			/// Run the server's io_service loop.
			void Run();

			RequestHandler& GetRequestHandler();

		private:
			/// Initiate an asynchronous accept operation.
			void Start();

			/// Handle completion of an asynchronous accept operation.
			void handle_accept(const boost::system::error_code& e);

			/// Handle a request to stop the server.
			void HandleStop();

			std::unique_ptr<IoServicePool> mIoPool;

			/// Acceptor used to listen for incoming connections.
			std::unique_ptr<boost::asio::ip::tcp::acceptor> mAcceptor;

			/// The next connection to be accepted.
			connection_ptr mNewConnection;

			/// The handler for all incoming requests.
			RequestHandler mRequestHandler;
		};

	} 
} 
