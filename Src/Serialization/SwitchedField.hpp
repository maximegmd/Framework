#pragma once

template <uint32_t flag, class A>
struct SwitchedField
{
	static const uint32_t Flag = flag;

	typedef typename A Type;

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, SwitchedField<flag, A>& data)
	{
		p << data.impl.m0;
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, SwitchedField<flag, A>& data)
	{
		p >> data.impl.m0;
		return p;
	}

	operator A()
	{
		return impl.m0;
	}

	void operator=(A arg)
	{
		impl.m0 = arg;
	}

	struct Impl{

		Impl()
		{
			m0 = A();
		}

		A m0;
	} impl;
};
