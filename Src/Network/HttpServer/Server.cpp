//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "Server.hpp"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <vector>

namespace Framework {
	namespace Network {

		HttpServer::HttpServer(unsigned short pPort, std::size_t thread_pool_size)
			: 
			mIoPool(new IoServicePool(thread_pool_size)),
			mNewConnection()
		{

			// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
			mAcceptor.reset(new boost::asio::ip::tcp::acceptor(mIoPool->GetIoService()));
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), pPort);
			mAcceptor->open(endpoint.protocol());
			mAcceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			mAcceptor->bind(endpoint);
			mAcceptor->listen();

			Start();
		}

		void HttpServer::Run()
		{
			mIoPool->Run();
		}

		void HttpServer::Start()
		{
			mNewConnection = boost::make_shared<Connection>(mIoPool->GetIoService(), mRequestHandler);
			mAcceptor->async_accept(mNewConnection->socket(),
				boost::bind(&HttpServer::handle_accept, this,
				boost::asio::placeholders::error));
		}

		RequestHandler& HttpServer::GetRequestHandler()
		{
			return mRequestHandler;
		}

		void HttpServer::handle_accept(const boost::system::error_code& e)
		{
			if (!e)
			{
				mNewConnection->start();
			}

			Start();
		}

		void HttpServer::HandleStop()
		{
			mIoPool->Stop();
		}

	} 
} 