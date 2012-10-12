//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "RequestHandler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "MimeTypes.hpp"
#include "Reply.hpp"
#include "Request.hpp"
#include <System/Tools.h>

namespace Framework {
	namespace Network {

		RequestHandler::RequestHandler()
		{
		}

		void RequestHandler::HandleRequest(const Request& req, Reply& rep)
		{
			// Decode url to path.
			std::string request_path;
			if (!UrlDecode(req.uri, request_path))
			{
				rep = Reply::StockReply(Reply::bad_request);
				return;
			}

			// Request path must be absolute and not contain "..".
			if (request_path.empty() || request_path[0] != '/'
				|| request_path.find("..") != std::string::npos)
			{
				rep = Reply::StockReply(Reply::bad_request);
				return;
			}

			// If path ends in slash (i.e. is a directory) then add "index.html".
			if (request_path[request_path.size() - 1] == '/')
			{
				request_path += "index.html";
			}

			// Determine the file extension.
			std::size_t last_slash_pos = request_path.find_last_of("/");
			std::size_t last_dot_pos = request_path.find_last_of(".");
			std::string extension;
			if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
			{
				extension = request_path.substr(last_dot_pos + 1);
			}

			// Open the file to send back.
			size_t pos = request_path.find("?");
			std::string full_path = request_path;
			std::string args = "";
			if(pos != std::string::npos)
			{
				full_path = request_path.substr(0, pos);
				args = request_path.substr(pos + 1);
			}
			if (mHandlers.find(full_path) == mHandlers.end())
			{
				rep = Reply::StockReply(Reply::not_found);
				rep.content += std::string("\n<br />File : ") + full_path + std::string(" is not binded to any native function.");
				rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
				return;
			}

			std::map<std::string, std::string> data;

			char *line	= NULL;
			char *dup = strdup(args.c_str());
			line	  = strtok( dup, "&" );

			while( line != NULL )
			{
				std::string tmp(line);
				size_t const loc = tmp.find('=');

				if (loc != std::string::npos)
				{
					data[tmp.substr(0, loc)] = tmp.substr(loc+1, tmp.size());
				}

				line			= strtok( NULL, "&" );
			}
			free(dup);

			// Fill out the reply to be sent to the client.
			rep.status = Reply::ok;
			char buf[512];
			rep.content = mHandlers[full_path](data);
			rep.headers.resize(2);
			rep.headers[0].name = "Content-Length";
			rep.headers[0].value = boost::lexical_cast<std::string>(rep.content.size());
			rep.headers[1].name = "Content-Type";
			rep.headers[1].value = MimeTypes::extension_to_type(extension);
		}

		bool RequestHandler::UrlDecode(const std::string& in, std::string& out)
		{
			out.clear();
			out.reserve(in.size());
			for (std::size_t i = 0; i < in.size(); ++i)
			{
				if (in[i] == '%')
				{
					if (i + 3 <= in.size())
					{
						int value = 0;
						std::istringstream is(in.substr(i + 1, 2));
						if (is >> std::hex >> value)
						{
							out += static_cast<char>(value);
							i += 2;
						}
						else
						{
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else if (in[i] == '+')
				{
					out += ' ';
				}
				else
				{
					out += in[i];
				}
			}
			return true;
		}

		boost::function<std::string(std::map<std::string, std::string>&)>& RequestHandler::operator[](const std::string& pName)
		{
			return mHandlers[pName];
		}

	} 
} 