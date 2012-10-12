//
// request_handler.hpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include <string>
#include <map>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>


namespace Framework {
	namespace Network {

		struct Reply;
		struct Request;

		/// The common handler for all incoming requests.
		class RequestHandler
			: private boost::noncopyable
		{
		public:
			/// Construct with a directory containing files to be served.
			explicit RequestHandler();

			/// Handle a request and produce a reply.
			void HandleRequest(const Request& req, Reply& rep);

			boost::function<std::string(std::map<std::string, std::string>&)>& operator[](const std::string& pName);

		private:

			/// Perform URL-decoding on a string. Returns false if the encoding was
			/// invalid.
			static bool UrlDecode(const std::string& in, std::string& out);

			std::map<std::string, boost::function<std::string(std::map<std::string, std::string>&)> > mHandlers;
		};

	} 
} 
