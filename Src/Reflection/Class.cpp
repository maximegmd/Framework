#include "Class.h"

namespace Framework { namespace Reflection
{
	std::map<std::string,Object*> ClassFactory::nameToClass_;

	Object* ClassFactory::ForName(const std::string& name)
	{
		Object* obj = (Object*)nameToClass_[name];

		return obj;
	}

	void ClassFactory::Put(const std::string& name, Object* object)
	{
		if(object != 0)
			nameToClass_[name] = object;
	}
}}