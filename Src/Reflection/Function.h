#pragma once

namespace Framework { namespace Reflection
{
class IFunction{};

template <class T>
void Invoke(IFunction* func, T* instance)
{
	if(func == 0 && instance == 0) return 0;

	Function0<T,void>* f = (Function0<T,void>*)func;
	(instance->*(f->func))();
}

template <class T, class A>
void Invoke(IFunction* func, T* instance,A a)
{
	if(func == 0 && instance == 0) return 0;

	Function1<T,void,A>* f = (Function1<T,void,A>*)func;
	(instance->*(f->func))(a);
}

template <class T, class A, class B>
void Invoke(IFunction* func, T* instance,A a,B b)
{
	if(func == 0 && instance == 0) return 0;

	Function2<T,void,A,B>* f = (Function2<T,void,A,B>*)func;
	(instance->*(f->func))(a,b);
}

template <class T, class A, class B, class C>
void Invoke(IFunction* func, T* instance,A a,B b,C c)
{
	if(func == 0 && instance == 0) return 0;

	Function3<T,void,A,B,C>* f = (Function3<T,void,A,B,C>*)func;
	(instance->*(f->func))(a,b,c);
}

template <class Ret, class T>
Ret Invoke(IFunction* func, T* instance)
{
	if(func == 0 && instance == 0) return 0;

	Function0<T,Ret>* f = (Function0<T,Ret>*)func;
	return (Ret)(instance->*(f->func))();
}

template <class Ret, class T, class A>
Ret Invoke(IFunction* func, T* instance, A a)
{
	if(func == 0 && instance == 0) return 0;

	Function1<T,Ret,A>* f = (Function1<T,Ret,A>*)func;
	return (Ret)(instance->*(f->func))(a);
}

template <class Ret, class T, class A, class B>
Ret Invoke(IFunction* func, T* instance, A a, B b)
{
	if(func == 0 && instance == 0) return 0;

	Function2<T,Ret,A,B>* f = (Function2<T,Ret,A,B>*)func;
	return (Ret)(instance->*(f->func))(a,b);
}

template <class Ret, class T, class A, class B, class C>
Ret Invoke(IFunction* func, T* instance, A a, B b, C c)
{
	if(func == 0 && instance == 0) return 0;

	Function3<T,Ret,A,B,C>* f = (Function3<T,Ret,A,B,C>*)func;
	return (Ret)(instance->*(f->func))(a,b,c);
}

template <class T, class K>
IFunction* __FUNCTION(T (K::*func)())
{
	Function0<K, T>* f = new Function0<K, T>;
	f->func = func;
	return f;
}

template <class T, class K, class A>
IFunction* __FUNCTION(T (K::*func)(A))
{
	Function1<K, T, A>* f = new Function1<K, T, A>;
	f->func = func;
	return f;
}

template <class T, class K, class A, class B>
IFunction* __FUNCTION(T (K::*func)(A,B))
{
	Function2<K, T,A,B>* f = new Function2<K, T,A,B>;
	f->func = func;
	return f;
}

template <class T, class K, class A, class B, class C>
IFunction* __FUNCTION(T (K::*func)(A,B,C))
{
	Function3<K, T,A,B,C>* f = new Function3<K, T,A,B,C>;
	f->func = func;
	return f;
}

template <class T, class K = void>
struct Function0 : public IFunction
{
	K (T::*func)();
};

template <class T, class K = void, class A= void>
struct Function1 : public IFunction
{
	K (T::*func)(A);
};

template <class T, class K = void, class A= void, class B= void>
struct Function2 : public IFunction
{
	K (T::*func)(A,B);
};

template <class T, class K = void, class A= void, class B= void, class C= void>
struct Function3 : public IFunction
{
	K (T::*func)(A,B,C);
};
}}