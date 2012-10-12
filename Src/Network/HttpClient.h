#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/signal.hpp>

using boost::asio::ip::tcp;

namespace Framework
{
	namespace Network
	{
		namespace detail 
		{
			class HttpRequestState;
		};

		char from_hex(char ch);
		char to_hex(char code);
		std::string UrlEncode(const std::string& str);

		class HttpClient
		{
			public:
				static void AsyncRequest (std::unique_ptr <boost::signal<void(std::string)>> on_success, std::unique_ptr <boost::signal<void()>> on_complete, 
					boost::asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData = "");

				static void AsyncRequest (std::unique_ptr <boost::signal<void(std::string)>> on_success, boost::asio::io_service& io_service,
					const std::string& server, const std::string& path, const std::string& pPostData = "");

				static void AsyncRequest (boost::function <void(std::string)> on_success, boost::function <void()> on_complete, 
					boost::asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData = "");

				static void AsyncRequest (boost::function <void(std::string)> on_success, boost::asio::io_service& io_service,
					const std::string& server, const std::string& path, const std::string& pPostData = "");

				static void AsyncRequest (boost::asio::io_service& io_service, const std::string& server, const std::string& path, const std::string& pPostData = "");

			private:
				HttpClient();

				static void HandleResolve(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& err, tcp::resolver::iterator endpoint_iterator);
				static void HandleConnect(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& err);
				static void HandleWriteRequest(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& err);
				static void HandleStatusLine(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& err);
				static void HandleHeaders(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& err);
				static void HandleContent(boost::shared_ptr <detail::HttpRequestState> state, const boost::system::error_code& err);
		};
	}
}