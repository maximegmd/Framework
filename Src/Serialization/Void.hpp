#pragma once

class Void
{
public:

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, Void&)
	{
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, Void&)
	{
		return p;
	}
};