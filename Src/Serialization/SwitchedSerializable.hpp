#pragma once

struct ISwitchedSerializable
{

};

template <typename Key, typename... Args>
struct SwitchedSerializable : 
	public ISwitchedSerializable,
	public std::tuple<Args...>
{

public:

	typedef typename SwitchedSerializable<Key, Args...> Type;
	typedef Type RawType;

	template <int N>
	struct Serializer
	{
		static void ser(Framework::Network::Packet& p, SwitchedSerializable<Key, Args...>& data)
		{
			if(data.flag() & std::element_type<N, std::tuple<Args...>>::type::Flag)
				p << std::get<N>(data);
			Serializer<N - 1>::ser(p, data);
		}

		static void deser(Framework::Network::Packet& p, SwitchedSerializable<Key, Args...>& data)
		{
			if(data.flag() & std::element_type<N, std::tuple<Args...>>::type::Flag)
				p >> std::get<N>(data);
			Serializer<N - 1>::deser(p, data);
		}
	};

	template <>
	struct Serializer<0>
	{
		static void ser(Framework::Network::Packet& p, std::tuple<Args...>& data)
		{
		}

		static void deser(Framework::Network::Packet& p, std::tuple<Args...>& data)
		{
		}
	};

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, Type& data)
	{
		Serializer<std::tuple_size<Type>::value>::ser(p, data);
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, Type& data)
	{
		Serializer<std::tuple_size<Type>::value>::deser(p, data);
		return p;
	}


public:

	Key& flag()
	{
		return impl.key;
	}

	template <int N> struct Resolver
	{
		typedef typename std::tuple_element<N, std::tuple<Args...>>::type RawType;
		typedef typename RawType::Type Type;
	};

	template <size_t N>
	typename Resolver<N>::RawType&
		get()
	{
		return std::get<N>(*this);
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
