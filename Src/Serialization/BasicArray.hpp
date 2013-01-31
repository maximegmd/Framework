#pragma once

#include <array>
#include <initializer_list>

template <size_t A, class B>
struct BasicArray : public std::array<B, A>
{
	BasicArray()
	{

	}

	BasicArray(std::initializer_list<B> l)
	{
		assert(l.size() == A);
		size_t index = 0;
		for(auto itor = l.begin(), end = l.end(); itor != end; ++itor, ++index)
		{
			(*this)[index] = *itor;
		}
	}


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
