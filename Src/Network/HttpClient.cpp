#include <Network/HttpClient.h>
#include <System/Log.h>
#include <boost/format.hpp>

using boost::asio::ip::tcp;

namespace Framework
{
	namespace Network
	{
		/* Converts a hex character to its integer value */
		char from_hex(char ch) {
			return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
		}

		/* Converts an integer value to its hex character*/
		char to_hex(char code) {
			static char hex[] = "0123456789abcdef";
			return hex[code & 15];
		}


		std::string UrlEncode(const std::string& str) {
			std::string ret;
			for(auto itor = str.begin(), end = str.end(); itor != end; ++itor) {
				if (isalnum(*itor) || *itor == '-' || *itor == '_' || *itor == '.' || *itor == '~') 
					ret += std::string(1,*itor);
				else if (*itor == ' ') 
					ret += std::string(1,'+');
				else 
				{
					ret += std::string("%");
					ret += std::string(1,to_hex(*itor >> 4));
					ret += std::string(1,to_hex(*itor & 15));
				}
			}
			ret += std::string("\0");
			return ret;
		}

		namespace detail
		{
			class HttpRequestState
			{
				public:
					HttpRequestState (std::unique_ptr <boost::signal<void(std::string)>> on_success, std::unique_ptr <boost::signal<void()>> on_complete, boost::asio::io_service& pIoService)
						: OnSuccess(std::move(on_success)),
						  OnComplete(std::move(on_complete)),
						  mResolver(pIoService),
						  mSocket(pIoService)
					{
					}

					tcp::resolver mResolver;
					tcp::socket mSocket;
					boost::asio::streambuf mRequest;
					boost::asio::streambuf mResponse;
					std::ostringstream mContent;

					std::unique_ptr <boost::signal<void(std::string)>> OnSuccess;
					std::unique_ptr <boost::signal<void()>> OnComplete;
				};
		};

		HttpClient::HttpClient ()
		{
		}

		void HttpClient::AsyncRequest(std::unique_ptr <boost::signal<void(std::string)>> on_success, std::unique_ptr <boost::signal<void()>> on_complete, 
					boost::asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData)
		{
			boost::shared_ptr <detail::HttpRequestState> state = boost::make_shared <detail::HttpRequestState> (std::move(on_success), std::move(on_complete), io_service);

			std::ostream requestStream(&state->mRequest);
			requestStream << "POST " << path << " HTTP/1.1\r\n";
			requestStream << "Host: " << server << "\r\n";
			requestStream << "Accept: */*\r\n";
			requestStream << "Content-Length: " << pPostData.size() << "\r\n";
			requestStream << "Content-Type: application/x-www-form-urlencoded\r\n";
			requestStream << "Connection: close\r\n\r\n";
			requestStream << pPostData;

			tcp::resolver::query query(server, "http");
			state->mResolver.async_resolve(query,
				boost::bind(&HttpClient::HandleResolve, state,
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));
		}

		void HttpClient::AsyncRequest(std::unique_ptr <boost::signal<void(std::string)>> on_success, boost::asio::io_service& io_service,
					const std::string& server, const std::string& path, const std::string& pPostData)
		{
			std::unique_ptr <boost::signal <void()>> signal_on_complete(new boost::signal <void()> ());

			HttpClient::AsyncRequest(std::move(on_success), std::move(signal_on_complete), io_service, server, path, pPostData);
		}

		void HttpClient::AsyncRequest (boost::function <void(std::string)> on_success, boost::asio::io_service& io_service, 
					const std::string& server, const std::string& path, const std::string& pPostData)
		{
			std::unique_ptr <boost::signal <void(std::string)>> signal_on_success(new boost::signal <void(std::string)> ());
			std::unique_ptr <boost::signal <void()>> signal_on_complete(new boost::signal <void()> ());

			signal_on_success->connect(on_success);
			HttpClient::AsyncRequest(std::move(signal_on_success), std::move(signal_on_complete), io_service, server, path, pPostData);
		}

		void HttpClient::AsyncRequest (boost::function <void(std::string)> on_success, boost::function <void()> on_complete, 
					boost::asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData)
		{
			std::unique_ptr <boost::signal <void(std::string)>> signal_on_success(new boost::signal <void(std::string)> ());
			std::unique_ptr <boost::signal <void()>> signal_on_complete(new boost::signal <void()> ());

			signal_on_success->connect(on_success);
			signal_on_complete->connect(on_complete);
			HttpClient::AsyncRequest(std::move(signal_on_success), std::move(signal_on_complete), io_service, server, path, pPostData);
		}

		void HttpClient::AsyncRequest (boost::asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData)
		{
			std::unique_ptr <boost::signal <void(std::string)>> signal_on_success(new boost::signal <void(std::string)> ());
			std::unique_ptr <boost::signal <void()>> signal_on_complete(new boost::signal <void()> ());

			HttpClient::AsyncRequest(std::move(signal_on_success), std::move(signal_on_complete), io_service, server, path, pPostData);
		}

		void HttpClient::HandleResolve(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& pError,
			tcp::resolver::iterator pEndpointItor)
		{
			if (!pError)
			{
				boost::asio::async_connect(state->mSocket, pEndpointItor,
					boost::bind(&HttpClient::HandleConnect, state,
					boost::asio::placeholders::error));
			}
			else
			{
				Framework::System::Log::Debug("HttpClient::HandleResolve - Error: " + pError.message());
				state->OnComplete->operator()();
			}
		}

		void HttpClient::HandleConnect(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& pError)
		{
			if (!pError)
			{
				boost::asio::async_write(state->mSocket, state->mRequest,
					boost::bind(&HttpClient::HandleWriteRequest, state,
					boost::asio::placeholders::error));
			}
			else
			{
				Framework::System::Log::Debug("HttpClient::HandleConnect - Error: " + pError.message());
				state->OnComplete->operator()();
			}
		}

		void HttpClient::HandleWriteRequest(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& pError)
		{
			if (!pError)
			{
				boost::asio::async_read_until(state->mSocket, state->mResponse, "\r\n",
					boost::bind(&HttpClient::HandleStatusLine, state,
					boost::asio::placeholders::error));
			}
			else
			{
				Framework::System::Log::Debug("HttpClient::HandleWriteRequest - Error: " + pError.message());
				state->OnComplete->operator()();
			}
		}

		void HttpClient::HandleStatusLine(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& pError)
		{
			if (!pError)
			{
				std::istream responseStream(&state->mResponse);
				std::string httpVersion;
				responseStream >> httpVersion;
				unsigned int statusCode;
				responseStream >> statusCode;
				std::string statusMessage;
				std::getline(responseStream, statusMessage);
				if (!responseStream || httpVersion.substr(0, 5) != "HTTP/")
				{
					Framework::System::Log::Debug("HttpClient::HandleStatusLine - Invalid response\n");
					state->OnComplete->operator()();
					return;
				}
				if (statusCode != 200)
				{
					Framework::System::Log::Debug(str(boost::format("HttpClient::HandleStatusLine - Response returned with status code %i\n") % statusCode));
					state->OnComplete->operator()();
					return;
				}

				boost::asio::async_read_until(state->mSocket, state->mResponse, "\r\n\r\n",
					boost::bind(&HttpClient::HandleHeaders, state,
					boost::asio::placeholders::error));
			}
			else
			{
				Framework::System::Log::Debug("HttpClient::HandleStatusLine - Error: " + pError.message());
				state->OnComplete->operator()();
			}
		}

		void HttpClient::HandleHeaders(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& pError)
		{
			if (!pError)
			{
				std::istream responseStream(&state->mResponse);
				std::string header;
				while (std::getline(responseStream, header) && header != "\r");

				if (state->mResponse.size() > 0)
					state->mContent << &state->mResponse;

				boost::asio::async_read(state->mSocket, state->mResponse,
					boost::asio::transfer_at_least(1),
					boost::bind(&HttpClient::HandleContent, state,
					boost::asio::placeholders::error));
			}
			else
			{
				Framework::System::Log::Debug("HttpClient::HandleHeaders - Error: " + pError.message());
				state->OnComplete->operator()();
			}
		}

		void HttpClient::HandleContent(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& pError)
		{
			if (!pError)
			{
				state->mContent << &state->mResponse;

				boost::asio::async_read(state->mSocket, state->mResponse,
					boost::asio::transfer_at_least(1),
					boost::bind(&HttpClient::HandleContent, state,
					boost::asio::placeholders::error));
			}
			else if (pError != boost::asio::error::eof)
			{
				Framework::System::Log::Debug("HttpClient::HandleContent - Error: " + pError.message());
				state->OnComplete->operator()();
			}
			else
			{
				state->OnSuccess->operator()(state->mContent.str());
				state->OnComplete->operator()();
			}
		}
	}
}