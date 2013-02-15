#pragma once

#include "SwitchedSerializable.hpp"
#include <type_traits>

template <class A, class Value, class Enabled = void>
struct Accessor1
{
	static void set(A& a, Value value)
	{
		a = value;
	}
};

template <class A, class B, class Value, class Enabled = void>
struct Accessor2
{
	static void set(A& a, B& b, Value value)
	{
		Accessor1<B, Value>::set(b, value);
	}

	static bool isSet(A& a, B& b)
	{
		return true;
	}

};

template <class A, typename B, class Value>
struct Accessor2<A, B, Value, 
		typename std::enable_if
		<
			std::is_base_of<ISwitchedSerializable, typename std::remove_reference<A>::type>::value
		>::type
	>
{
	static void set(A& a, B& b, Value value)
	{
		a.flag() |= B::Flag;
		Accessor1<B, Value>::set(b, value);
	}

	static bool isSet(A& a, B& b)
	{
		return (a.flag() & B::Flag) != 0;
	}
};

template <class A, class B, class C, class Value, class Enabled = void>
struct Accessor3
{
	static void set(A& a, B& b, C& c, Value value)
	{
		Accessor2<B, C, Value>::set(b, c, value);
	}

	static bool isSet(A& a, B& b, C& c)
	{
		return Accessor2<B, C, Value>::isSet(b, c);
	}
};

template <class A, class B, class C, class Value>
struct Accessor3<A, B, C, Value, typename std::enable_if<std::is_base_of<ISwitchedSerializable, A>::value>::type>
{
	static void set(A& a, B& b, C& c, Value value)
	{
		a.flag() |= B::Flag;
		Accessor2<B, C, Value>::set(b, c, value);
	}

	static bool isSet(A& a, B& b, C& c)
	{
		if((a.flag() & B::Flag) != 0)
			return Accessor2<B, C, Value>::isSet(b, c);
		return false;
	}
};

#define ACCESSOR_1(a, name) Resolver<a>::Type Get##name() \
{ \
	return get<a>(); \
} \
bool IsSet##name() \
{ \
	return true; \
} \
void Set##name(Resolver<a>::Type p) \
{ \
	typedef std::remove_reference<decltype(get<a>())>::type Arg0;\
	typedef decltype(p) Arg1;\
	Accessor1<Arg0, Arg1>::set(get<a>(), p); \
}


#define ACCESSOR_2(a, b, Name) Resolver<a>::Type::Resolver<b>::RawType Get##Name() \
{ \
	return get<a>().get<b>(); \
} \
bool IsSet##Name() \
{ \
	typedef std::remove_reference<decltype(get<a>())>::type Arg0; \
	typedef std::remove_reference<decltype(get<a>().get<b>())>::type Arg1; \
	typedef void* Arg2; \
	return Accessor2<Arg0, Arg1, Arg2>::isSet(get<a>(), get<a>().get<b>()); \
} \
void Set##Name(Resolver<a>::Type::Resolver<b>::Type p)\
{ \
	typedef std::remove_reference<decltype(get<a>())>::type Arg0; \
	typedef std::remove_reference<decltype(get<a>().get<b>())>::type Arg1; \
	typedef decltype(p) Arg2; \
	return Accessor2<Arg0, Arg1, Arg2>::set(get<a>(), get<a>().get<b>(), p); \
}

#define ACCESSOR_3(a, b, c, name) Resolver<a>::Type::Resolver<b>::Type::Resolver<c>::Type Get##name() \
{ \
	return impl.m##a.impl.m##b.impl.m0.impl.m##c; \
} \
	bool IsSet##name() \
{ \
	typedef decltype(impl.m##a) Arg0; \
	typedef decltype(impl.m##a.impl.m##b) Arg1; \
	typedef decltype(impl.m##a.impl.m##b.impl.m##c) Arg2; \
	typedef void* Arg3; \
	return Accessor3<Arg0, Arg1, Arg2, Arg3>::isSet(impl.m##a, impl.m##a.impl.m##b, impl.m##a.impl.m##b.impl.m##c); \
} \
	void Set##name(Resolver<a>::Type::Resolver<b>::Type::Resolver<c>::Type p) \
{ \
	typedef decltype(impl.m##a) Arg0; \
	typedef decltype(impl.m##a.impl.m##b) Arg1; \
	typedef decltype(impl.m##a.impl.m##b.impl.m##c) Arg2; \
	typedef decltype(p) Arg3; \
	Accessor3<Arg0, Arg1, Arg2, Arg3>::set(impl.m##a, impl.m##a.impl.m##b, impl.m##a.impl.m##b.impl.m##c, p); \
}
