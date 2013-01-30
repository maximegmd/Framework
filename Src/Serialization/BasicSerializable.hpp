#pragma once


template <typename... Args> struct BasicSerializable;
template <> struct BasicSerializable<> 
{

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, BasicSerializable<>& data)
	{
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, BasicSerializable<>& data)
	{
		return p;
	}
};

template <int N>
struct getValue {
	template <typename ReturnType, typename Head, typename... TailArgs>
	static ReturnType get(BasicSerializable<Head, TailArgs...>& t) {                                                                          
		return getValue<N - 1>::template get<ReturnType, TailArgs...>(t.next());
	}                                                                                                                                                    
};                                                                           

template <>
struct getValue<0> {
	template <typename ReturnType, typename... Args>
	static ReturnType get(BasicSerializable<Args...>& t) {                                                                         
		return t.head();
	}                                                                                                                                                
};

template <typename T, typename... Args>
struct BasicSerializable<T, Args...> : private BasicSerializable<Args...>
{
public:

	typedef typename BasicSerializable<T, Args...> Type;
	typedef Type RawType;

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, Type& data)
	{
		p << data.impl.head << (BasicSerializable<Args...>&)(data);
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, Type& data)
	{
		p >> data.impl.head >> (BasicSerializable<Args...>&)(data);
		return p;
	}

private:

	template <int N, typename... Arg> struct Resolver_impl;

	template <typename A0, typename... Arg>
	struct Resolver_impl<0, A0, Arg...> {
		typedef A0 Type;
	};

	template <int N, typename A0, typename... Arg>
	struct Resolver_impl<N, A0, Arg...> {
		typedef typename Resolver_impl<N - 1, Arg...>::Type Type;
	};

public:

	template <int N> struct Resolver
	{
		typedef typename Resolver_impl<N, T, Args...>::Type Type;
	};

	template <int N>
	typename Resolver<N>::Type&
	get() 
	{
		return getValue<N>::get<typename Resolver<N>::Type&, T, Args...>(*this);
	}

	BasicSerializable<Args...>& next()
	{
		return *this;
	}

	T& head()
	{
		return impl.head;
	}

	Framework::Network::Packet ToPacket(uint32_t opcode = 0)
	{
		Framework::Network::Packet packet;

		if(opcode != 0)
			packet.Opcode = opcode;

		packet << *this;
		return std::move(packet);
	}

private:

	struct Impl
	{
		Impl()
		{
			head = T();
		}

		T head;
	} impl;
};
