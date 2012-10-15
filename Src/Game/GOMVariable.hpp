#pragma once

#include <string>
#include <functional>

namespace Game
{
	class IGOMEntry;

	template <class T>
	struct GOMVariable
	{
		typedef typename T type;
		typedef std::function<const type&()>		Getter;

		GOMVariable(Getter g)
			:getter(g)
		{
			a = getter();
		}

		void operator()(IGOMEntry* me){
			if(a != getter())
				me->SetDirty(true);
		}

		void operator()(){
			a = getter();
		}

		const T& get() const{
			return getter();
		}

	private:

		Getter getter;
		T a;
	};
}