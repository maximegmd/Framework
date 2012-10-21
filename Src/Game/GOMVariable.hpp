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
		typedef std::function<type()>		Getter;

		/**
		 * @brief Constructs a GOM Variable with a getter.
		 * @param g The getter functor.
		 */
		GOMVariable(Getter g)
			:getter(g)
		{
			a = getter();
		}
		/**
		 * @brief Checks if the current state and previous state mismatch, they do, sets the GOM Entry to dirty.
		 * @param me The GOM Variable's GOM Entry parent.
		 */
		void operator()(IGOMEntry* me){
			if(a != getter())
				me->SetDirty(true);
		}
		/**
		 * @brief Checks if the current value and the previous value match.
		 * @return true if they match, false if they don't.
		 */
		operator bool() const{
			return (a == getter());
		}
		/**
		 * @brief Sets the previous value to the current value.
		 */
		void operator()(){
			a = get();
		}

		/**
		 * @brief Gets the current value of the GOM variable.
		 * @return The current value.
		 */
		T get() const{
			return getter();
		}

	private:

		Getter getter;
		T a;
	};
}