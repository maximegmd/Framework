#pragma once

template <uint32_t flag, class A>
struct SwitchedField
{
	static const uint32_t Flag = flag;

	typedef typename A Type;
	typedef Type RawType;

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, SwitchedField<flag, A>& data)
	{
		p << data.impl.head;
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, SwitchedField<flag, A>& data)
	{
		p >> data.impl.head;
		return p;
	}

	operator A()
	{
		return impl.head;
	}

	void operator=(A arg)
	{
		impl.head = arg;
	}

	template <int N>
	A& get()
	{
		return impl.head;
	}

private:

	struct Impl{

		Impl()
		{
			head = A();
		}

		A head;
	} impl;
};
