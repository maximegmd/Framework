#pragma once

#include "SwitchedSerializable.hpp"
#include <type_traits>

template <class A, class Value, class Enabled = void>
struct Setter1
{
	static void set(A& a, Value value)
	{
		a = value;
	}
};

template <class A, class Value>
struct Setter1<A, Value, typename std::enable_if<std::is_base_of<ISwitchedSerializable, A>::value>::type>
{
	static void set(A& a, Value value)
	{
		a = value;
	}
};

template <class A, class B, class Value, class Enabled = void>
struct Setter2
{
	static void set(A& a, B& b, Value value)
	{
		Setter1<B, Value>::set(b, value);
	}
};

template <class A, class B, class Value>
struct Setter2<A, B, Value, typename std::enable_if<std::is_base_of<ISwitchedSerializable, A>::value>::type>
{
	static void set(A& a, B& b, Value value)
	{
		a.impl.mFlag |= B::Flag;
		Setter1<B, Value>::set(b, value);
	}
};

template <class A, class B, class C, class Value, class Enabled = void>
struct Setter3
{
	static void set(A& a, B& b, C& c, Value value)
	{
		Setter2<B, C, Value>::set(b, c, value);
	}
};

template <class A, class B, class C, class Value>
struct Setter3<A, B, C, Value, typename std::enable_if<std::is_base_of<ISwitchedSerializable, A>::value>::type>
{
	static void set(A& a, B& b, C& c, Value value)
	{
		a.impl.mFlag |= B::Flag;
		Setter2<B, C, Value>::set(b, c, value);
	}
};

template <class A, class B, class C, class D, class Value, class Enabled = void>
struct Setter4
{
	static void set(A& a, B& b, C& c, D& d, Value value)
	{
		Setter3<B, C, D, Value>::set(b, c, d, value);
	}
};

template <class A, class B, class C, class D, class Value>
struct Setter4<A, B, C, D, Value, typename std::enable_if<std::is_base_of<ISwitchedSerializable, A>::value>::type>
{
	static void set(A& a, B& b, C& c, D& d, Value value)
	{
		a.impl.mFlag |= B::Flag;
		Setter3<B, C, D, Value>::set(b, c, d, value);
	}
};

#define ACCESSOR_1(a, name) Resolver<a>::Type Get##name() \
{ \
	return impl.m##a; \
} \
void Set##name(Resolver<a>::Type p) \
{ \
	typedef decltype(impl.m##a) Arg0;\
	typedef decltype(p) Arg1;\
	Setter1<Arg0, Arg1>::set(impl.m##a, p); \
}

#define ACCESSOR_2(a, b, Name) Resolver<a>::Type::Resolver<b>::Type Get##Name() \
{ \
	return impl.m##a.impl.m##b; \
} \
void Set##Name(Resolver<a>::Type::Resolver<b>::Type p)\
{ \
	typedef decltype(impl.m##a) Arg0; \
	typedef decltype(impl.m##a.impl.m##b) Arg1; \
	typedef decltype(p) Arg2; \
	Setter2<Arg0, Arg1, Arg2>::set(impl.m##a, impl.m##a.impl.m##b, p); \
}

#define ACCESSOR_3(a, b, c, name) Resolver<a>::Type::Resolver<b>::Type::Resolver<c>::Type Get##name() \
{ \
	return impl.m##a.impl.m##b.impl.m0.impl.m##c; \
} \
	void Set##name(Resolver<a>::Type::Resolver<b>::Type::Resolver<c>::Type p) \
{ \
	typedef decltype(impl.m##a) Arg0; \
	typedef decltype(impl.m##a.impl.m##b) Arg1; \
	typedef decltype(impl.m##a.impl.m##b.impl.m##c) Arg2; \
	typedef decltype(p) Arg3; \
	Setter3<Arg0, Arg1, Arg2, Arg3>::set(impl.m##a, impl.m##a.impl.m##b, impl.m##a.impl.m##b.impl.m##c, p); \
}
