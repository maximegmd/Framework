#pragma once

template <class A>
struct Packed
{
	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, Packed<A>& data)
	{
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, Packed<A>& data)
	{
		return p;
	}
};