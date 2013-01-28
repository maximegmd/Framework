#pragma once

#include <array>

template <size_t A, class B>
struct BasicArray : public std::array<B, A>
{
	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, BasicArray<A, B>& data)
	{
		for(size_t i = 0; i != (size_t)A; ++i)
		{
			p << data[i];
		}
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, BasicArray<A, B>& data)
	{
		for(size_t i = 0; i < (size_t)A; ++i)
		{
			p >> data[i];
		}
		return p;
	}
};
