#pragma once

template <class A, class B = uint32_t>
struct BasicVector : public std::vector<A>
{
	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, BasicVector<A, B>& data)
	{
		p << data.mSize;
		for(auto itor = data.begin(), end = data.end(); itor != end; ++itor)
		{
			p << (*itor);
		}
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, BasicVector<A, B>& data)
	{
		p >> data.mSize;
		for(size_t i = 0; i < (size_t)data.mSize; ++i)
		{
			B value;
			p >> value;
			data.push_back(value);
		}
		return p;
	}

private:

	B mSize;
};
