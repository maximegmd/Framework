#pragma once


template <typename... Args>
struct BasicSerializable: private std::tuple<Args...>
{
private:

	template <int N>
	struct Serializer
	{
		static void ser(Framework::Network::Packet& p, std::tuple<Args...>& data)
		{
			p << std::get<N>(data);
			Serializer<N - 1>::ser(p, data);
		}

		static void deser(Framework::Network::Packet& p, std::tuple<Args...>& data)
		{
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

public:

	typedef typename BasicSerializable<Args...> Type;
	typedef Type RawType;

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

	template <int N> struct Resolver
	{
		typedef typename std::tuple_element<N, std::tuple<Args...>>::type Type;
	};

	template <size_t N>
	typename Resolver<N>::Type&
	get()
	{
		return std::get<N>(*this);
	}

	Framework::Network::Packet ToPacket(uint32_t opcode = 0)
	{
		Framework::Network::Packet packet;

		if(opcode != 0)
			packet.Opcode = opcode;

		packet << *this;
		return std::move(packet);
	}
};
