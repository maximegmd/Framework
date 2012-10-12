#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <cstdint>

namespace Framework
{
	namespace Network
	{
		struct IStrategy
		{
			virtual uint32_t GetHeaderSize() = 0;
			virtual uint32_t HandleHeader(const std::string&) = 0;
			virtual bool	 HandleContent(const std::string&) = 0;
			virtual bool	 HasPacket() = 0;
		};

		template <class T>
		struct Strategy : public IStrategy
		{
		public:

			virtual T		PopPacket() = 0;
		};
	}
}