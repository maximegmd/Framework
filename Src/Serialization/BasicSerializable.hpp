#pragma once

template <class A, class B = Void, class C = Void, class D = Void, class E = Void, class F = Void, class G = Void, class H = Void, class I = Void>
class BasicSerializable
{
public:

	typedef typename BasicSerializable<A,B,C,D,E,F,G,H,I> Type;

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, BasicSerializable<A,B,C,D,E,F,G,H,I>& data)
	{
		p << data.impl.m0 << data.impl.m1 << data.impl.m2 << data.impl.m3 << data.impl.m4 << data.impl.m5 << data.impl.m6 << data.impl.m7 << data.impl.m8;
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, BasicSerializable<A,B,C,D,E,F,G,H,I>& data)
	{
		p >> data.impl.m0 >> data.impl.m1 >> data.impl.m2 >> data.impl.m3 >> data.impl.m4 >> data.impl.m5 >> data.impl.m6 >> data.impl.m7 >> data.impl.m8;
		return p;
	}

	template <int depth, class A0 = A, class B0 = B, class C0 = C, class D0 = D, class E0 = E, class F0 = F, class G0 = G, class H0 = H, class I0 = I>
	struct Resolver
	{
		typedef typename Resolver<depth - 1, B0, C0, D0, E0, F0, G0, H0, I0, Void>::Type Type;
	};

	template <class A0, class B0, class C0, class D0, class E0, class F0, class G0, class H0, class I0>
	struct Resolver<0, A0, B0, C0, D0, E0, F0, G0, H0, I0>
	{
		typedef typename A0 Type;
	};

	struct Impl
	{
		Impl()
		{
			m0 = A();
			m1 = B();
			m2 = C();
			m3 = D();
			m4 = E();
			m5 = F();
			m6 = G();
			m7 = H();
			m8 = I();
		}

		A m0;
		B m1;
		C m2;
		D m3;
		E m4;
		F m5;
		G m6;
		H m7;
		I m8;
	} impl;
};
