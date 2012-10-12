#pragma once

#include <map>
#include <string>
#include "Function.h"

#define IOBJECT_CONSTRUCTOR_BEGIN(Class) \
	typedef Class IOBJECT_CLASS_NAME; \
	IOBJECT_CLASS_NAME(Ninjato::Framework::Reflection::PlainUseLessClassFactory* doNotUse) \
		{ \
			((IObject<IOBJECT_CLASS_NAME>)*this)

#define ADD_FUNCTION(name) .AddFunction(#name, __FUNCTION(&IOBJECT_CLASS_NAME##::##name))
#define IOBJECT_CONSTRUCTOR_END ;};

#define _class(name) class name : public Ninjato::Framework::Reflection::IObject<name>

namespace Framework { namespace Reflection
{
	class Field
	{
		void* mAccessor;

	public:

		Field(void* ptr)
			:mAccessor(ptr){}

		template <typename T>
		void Set(T t)
		{
			reinterpret_cast<T>(*mAccessor) = t;
		}

		template <typename T>
		T Get()
		{
			return *static_cast<T*>(mAccessor);
		}
	};

	class Object
	{
	public:

		virtual class Object* NewInstance() = 0;
		virtual IFunction* GetFunction(const std::string&) = 0;
	};

	class ClassFactory
	{
	public:

		static Object* ForName(const std::string& name);
		static void Put(const std::string& name, Object* object);

	private:

		static std::map<std::string,Object*>		nameToClass_;
	};

	template<typename T>
	class IObject : public Object
	{
	public:

		IObject();

		Object* NewInstance();

		IFunction* GetFunction(const std::string& str);

		IObject<T>& AddFunction(const std::string& name, IFunction* function);

	protected:

		static std::map<std::string, IFunction*>	functions;
		static std::map<std::string, size_t>		offsets;

	private:

		static bool initialized_;
		static T    instance_;
	};

	template <typename T>
	IObject<T>::IObject()
	{
		if(IObject<T>::initialized_ == false)
		{
			IObject<T>::initialized_ = true;
			std::string name = typeid(T).name();
			name.replace(name.find("class "),6,"");
			ClassFactory::Put(name, (Object*)&IObject<T>::instance_);
		}
	}

	template <typename T>
	Object* IObject<T>::NewInstance()
	{
		return new T();
	}

	template <typename T>
	IFunction* IObject<T>::GetFunction(const std::string& str)
	{
		return functions[str];
	}

	struct PlainUseLessClassFactory{
		;
	};
	static PlainUseLessClassFactory* PlainUseLessClassFactoryDoNotUse;

	template <class T> bool IObject<T>::initialized_ = false;
	template <class T> std::map<std::string, IFunction*> IObject<T>::functions;
	template <class T> T IObject<T>::instance_(PlainUseLessClassFactoryDoNotUse);

	template <class T> IObject<T>& IObject<T>::AddFunction(const std::string& name, IFunction* function)
	{
		functions[name] = function;
		return *this;
	}
}}