#pragma once

template <class A, class B = uint32_t>
struct BasicVector : public std::vector<A>
{
	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, BasicVector<A, B>& data)
	{
		p << data.TypeSafeSize();
		for(auto itor = data.begin(), end = data.end(); itor != end; ++itor)
		{
			p << (*itor);
		}
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, BasicVector<A, B>& data)
	{
		size_type s = 0;
		B stmp = 0;
		p >> stmp;
		s = static_cast<size_type>(stmp);
		for(size_t i = 0; i < s; ++i)
		{
			B value;
			p >> value;
			data.push_back(value);
		}
		return p;
	}

	B TypeSafeSize() const
	{
		return static_cast<B>(size());
	}

private:
};
