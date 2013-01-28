template <class Type0
#if NUM_PARAMS >= 1
	,class T_Arg0
#endif
#if NUM_PARAMS >= 2
	,class T_Arg1
#endif
#if NUM_PARAMS >= 3
	,class T_Arg2
#endif
#if NUM_PARAMS >= 4
	,class T_Arg3
#endif
#if NUM_PARAMS >= 5
	,class T_Arg4
#endif
#if NUM_PARAMS >= 6
	,class T_Arg5
#endif
#if NUM_PARAMS >= 7
	,class T_Arg6
#endif
#if NUM_PARAMS >= 8
	,class T_Arg7
#endif
#if NUM_PARAMS >= 9
	,class T_Arg8
#endif
#if NUM_PARAMS >= 10
	,class T_Arg9
#endif
>
struct CLASS_NAME
{
public:



};

template <class Type0
#if NUM_PARAMS >= 1
	,class T_Arg0
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 2
	,class T_Arg1 
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 3
	,class T_Arg2 
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 4
	,class T_Arg3 
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 5
	,class T_Arg4 
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 6
	,class T_Arg5
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 7
	,class T_Arg6
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 8
	,class T_Arg7
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 9
	,class T_Arg8
#if NUM_PARAMS >= 10
	= void
#endif
#endif
#if NUM_PARAMS >= 10
	,class T_Arg9
#if NUM_PARAMS >= 10
	= void
#endif
#endif
>
class SwitchedSerializable
#if NUM_PARAMS < 10
	<Type0
#if NUM_PARAMS >= 1
	,T_Arg0
#else 
	,void
#endif
#if NUM_PARAMS >= 2
	,T_Arg1
#else 
	,void
#endif
#if NUM_PARAMS >= 3
	,T_Arg2
#else 
	,void
#endif
#if NUM_PARAMS >= 4
	,T_Arg3
#else 
	,void
#endif
#if NUM_PARAMS >= 5
	,T_Arg4
#else 
	,void
#endif
#if NUM_PARAMS >= 6
	,T_Arg5
#else 
	,void
#endif
#if NUM_PARAMS >= 7
	,T_Arg6
#else 
	,void
#endif
#if NUM_PARAMS >= 8
	,T_Arg7
#else 
	,void
#endif
#if NUM_PARAMS >= 9
	,T_Arg8
#else 
	,void
#endif
#if NUM_PARAMS >= 10
	,T_Arg9
#else 
	,void
#endif
	> 
#endif
	:ISwitchedSerializable
{
public:

	typedef typename SwitchedSerializable
		<Type0
#if NUM_PARAMS >= 1
		,T_Arg0
#else 
		,void
#endif
#if NUM_PARAMS >= 2
		,T_Arg1
#else 
		,void
#endif
#if NUM_PARAMS >= 3
		,T_Arg2
#else 
		,void
#endif
#if NUM_PARAMS >= 4
		,T_Arg3
#else 
		,void
#endif
#if NUM_PARAMS >= 5
		,T_Arg4
#else 
		,void
#endif
#if NUM_PARAMS >= 6
		,T_Arg5
#else 
		,void
#endif
#if NUM_PARAMS >= 7
		,T_Arg6
#else 
		,void
#endif
#if NUM_PARAMS >= 8
		,T_Arg7
#else 
		,void
#endif
#if NUM_PARAMS >= 9
		,T_Arg8
#else 
		,void
#endif
#if NUM_PARAMS >= 10
		,T_Arg9
#else 
		,void
#endif
		> ThisType;

	friend Framework::Network::Packet& operator<<(Framework::Network::Packet& p, ThisType& data)
	{
		p << data.impl.mFlag;
#if NUM_PARAMS >= 1
		if(T_Arg0::Flag & data.impl.mFlag)
			p << data.impl.m0;
#endif
#if NUM_PARAMS >= 2
		if(T_Arg1::Flag & data.impl.mFlag)
			p << data.impl.m1;
#endif
#if NUM_PARAMS >= 3
		if(T_Arg2::Flag & data.impl.mFlag)
			p << data.impl.m2;
#endif
#if NUM_PARAMS >= 4
		if(T_Arg3::Flag & data.impl.mFlag)
			p << data.impl.m3;
#endif
#if NUM_PARAMS >= 5
		if(T_Arg4::Flag & data.impl.mFlag)
			p << data.impl.m4;
#endif
#if NUM_PARAMS >= 6
		if(T_Arg5::Flag & data.impl.mFlag)
			p << data.impl.m5;
#endif
#if NUM_PARAMS >= 7
		if(T_Arg6::Flag & data.impl.mFlag)
			p << data.impl.m6;
#endif
#if NUM_PARAMS >= 8
		if(T_Arg7::Flag & data.impl.mFlag)
			p << data.impl.m7;
#endif
#if NUM_PARAMS >= 9
		if(T_Arg8::Flag & data.impl.mFlag)
			p << data.impl.m8;
#endif
#if NUM_PARAMS >= 10
		if(T_Arg9::Flag & data.impl.mFlag)
			p << data.impl.m9;
#endif
		return p;
	}
	friend Framework::Network::Packet& operator>>(Framework::Network::Packet& p, ThisType& data)
	{
		p >> data.impl.mFlag;
#if NUM_PARAMS >= 1
		if(T_Arg0::Flag & data.impl.mFlag)
			p >> data.impl.m0;
#endif
#if NUM_PARAMS >= 2
		if(T_Arg1::Flag & data.impl.mFlag)
			p >> data.impl.m1;
#endif
#if NUM_PARAMS >= 3
		if(T_Arg2::Flag & data.impl.mFlag)
			p >> data.impl.m2;
#endif
#if NUM_PARAMS >= 4
		if(T_Arg3::Flag & data.impl.mFlag)
			p >> data.impl.m3;
#endif
#if NUM_PARAMS >= 5
		if(T_Arg4::Flag & data.impl.mFlag)
			p >> data.impl.m4;
#endif
#if NUM_PARAMS >= 6
		if(T_Arg5::Flag & data.impl.mFlag)
			p >> data.impl.m5;
#endif
#if NUM_PARAMS >= 7
		if(T_Arg6::Flag & data.impl.mFlag)
			p >> data.impl.m6;
#endif
#if NUM_PARAMS >= 8
		if(T_Arg7::Flag & data.impl.mFlag)
			p >> data.impl.m7;
#endif
#if NUM_PARAMS >= 9
		if(T_Arg8::Flag & data.impl.mFlag)
			p >> data.impl.m8;
#endif
#if NUM_PARAMS >= 10
		if(T_Arg9::Flag & data.impl.mFlag)
			p >> data.impl.m9;
#endif
		return p;
	}


	struct Impl{

		Impl():
			mFlag(0)
#if NUM_PARAMS >= 1
			,m0()
#endif
#if NUM_PARAMS >= 2
		, m1()
#endif
#if NUM_PARAMS >= 3
		, m2()
#endif
#if NUM_PARAMS >= 4
		, m3()
#endif
#if NUM_PARAMS >= 5
		, m4()
#endif
#if NUM_PARAMS >= 6
		, m5()
#endif
#if NUM_PARAMS >= 7
		, m6()
#endif
#if NUM_PARAMS >= 8
		, m7()
#endif
#if NUM_PARAMS >= 9
		, m8()
#endif
#if NUM_PARAMS >= 10
		, m9()
#endif
		{

		}

		Type0 mFlag;
#if NUM_PARAMS >= 1
		T_Arg0 m0;
#endif
#if NUM_PARAMS >= 2
		T_Arg1 m1;
#endif
#if NUM_PARAMS >= 3
		T_Arg2 m2;
#endif
#if NUM_PARAMS >= 4
		T_Arg3 m3;
#endif
#if NUM_PARAMS >= 5
		T_Arg4 m4;
#endif
#if NUM_PARAMS >= 6
		T_Arg5 m5;
#endif
#if NUM_PARAMS >= 7
		T_Arg6 m6;
#endif
#if NUM_PARAMS >= 8
		T_Arg7 m7;
#endif
#if NUM_PARAMS >= 9
		T_Arg8 m8;
#endif
#if NUM_PARAMS >= 10
		T_Arg9 m9;
#endif
	} impl;

	template <int depth>
	struct Resolver;

#if NUM_PARAMS >= 1
	template <>
	struct Resolver<0>
	{
		typedef typename T_Arg0::Type Type;
	};
#endif
#if NUM_PARAMS >= 2
	template <>
	struct Resolver<1>
	{
		typedef typename T_Arg1::Type Type;
	};
#endif
#if NUM_PARAMS >= 3
	template <>
	struct Resolver<2>
	{
		typedef typename T_Arg2::Type Type;
	};
#endif
#if NUM_PARAMS >= 4
	template <>
	struct Resolver<3>
	{
		typedef typename T_Arg3::Type Type;
	};
#endif
#if NUM_PARAMS >= 5
	template <>
	struct Resolver<4>
	{
		typedef typename T_Arg4::Type Type;
	};
#endif
#if NUM_PARAMS >= 6
	template <>
	struct Resolver<5>
	{
		typedef typename T_Arg5::Type Type;
	};
#endif
#if NUM_PARAMS >= 7
	template <>
	struct Resolver<6>
	{
		typedef typename T_Arg6::Type Type;
	};
#endif
#if NUM_PARAMS >= 8
	template <>
	struct Resolver<7>
	{
		typedef typename T_Arg7::Type Type;
	};
#endif
#if NUM_PARAMS >= 9
	template <>
	struct Resolver<8>
	{
		typedef typename T_Arg8::Type Type;
	};
#endif
#if NUM_PARAMS >= 10
	template <>
	struct Resolver<9>
	{
		typedef typename T_Arg9::Type Type;
	};
#endif
};

#undef NUM_PARAMS
#undef CLASS_NAME