#pragma once


struct ISwitchedSerializable
{

};

template <typename... Args> struct SwitchedSerializable;

template <typename... Args> struct SwitchedSerializableImpl;
template <> struct SwitchedSerializableImpl<> 
{
	void Serialize(Framework::Network::Packet& p, uint32_t pFlag)
	{
	}
	void Deserialize(Framework::Network::Packet& p, uint32_t pFlag)
	{
	}
};

template <int N>
struct getValueSS {
	template <typename ReturnType, typename Head, typename... TailArgs>
	static ReturnType get(SwitchedSerializableImpl<Head, TailArgs...>& t) {                                                                          
		return getValueSS<N - 1>::template get<ReturnType, TailArgs...>(t.next());
	}                                                                                                                                                
};                                                                           

template <>
struct getValueSS<0> {
	template <typename ReturnType, typename... Args>
	static ReturnType get(SwitchedSerializableImpl<Args...>& t) {                                                                         
		return t.head();
	}                                                                                                                                                
};


template <typename T, typename... Args>
struct SwitchedSerializableImpl<T, Args...> : private SwitchedSerializableImpl<Args...>
{
public:

	typedef typename SwitchedSerializableImpl<T, Args...> Type;

	void Serialize(Framework::Network::Packet& p, uint32_t pFlag)
	{
		if((pFlag & T::Flag) != 0)
			p << head();
		next().Serialize(p, pFlag);
	}
	void Deserialize(Framework::Network::Packet& p, uint32_t pFlag)
	{
		if((pFlag & T::Flag) != 0)
			p >> head();
		next().Deserialize(p, pFlag);
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
		typedef typename Resolver_impl<N, T, Args...>::Type RawType;
		typedef typename RawType::Type Type;
	};

	template <int N>
	typename Resolver<N>::RawType&
		get() 
	{
		return getValueSS<N>::get<typename Resolver<N>::RawType&, T, Args...>(*this);
	}

	SwitchedSerializableImpl<Args...>& next()
	{
		return *this;
	}

	T& head()
	{
		return impl.head;
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

template <typename Key, typename... Args>
struct SwitchedSerializable<Key, Args...> : 
	public ISwitchedSerializable,
	public SwitchedSerializableImpl<Args...>
{
public:

	typedef typename SwitchedSerializable<Key, Args...> Type;

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, Type& data)
	{
		p << data.impl.key;
		data.Serialize(p, data.impl.key);
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, Type& data)
	{
		p >> data.impl.key;
		data.Deserialize(p, data.impl.key);
		return p;
	}

public:

	Key& flag()
	{
		return impl.key;
	}

private:

	struct Impl
	{
		Impl()
		{
			key = Key();
		}

		Key key;
	} impl;
};
