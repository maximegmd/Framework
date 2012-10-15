#pragma once

#include <string>
#include <functional>

namespace Game
{
	class IAccessor
	{
	public:

		enum
		{
			kDirty = 1 << 0
		};

		virtual void Update() = 0;
		virtual void Synchronize() = 0;
		bool IsDirty();

		virtual std::string Serialize() const = 0;
		virtual void Deserialize(const std::string&) = 0;

		void SetDirty(bool);

	protected:

		template <class T>
		friend struct Verifier;

	private:

		unsigned char flags;
	};

	template <class Model>
	class Accessor : public IAccessor
	{
	public:

		Accessor(Model* m) 
			: model(m)
		{
		}

	protected:

		Model* model;
	};

	template <class T>
	struct Verifier
	{
		typedef typename T type;
		typedef std::function<const type&()>		Getter;

		Verifier(Getter g)
			:getter(g)
		{
			a = getter();
		}

		void operator()(IAccessor* me){
			if(a != getter())
				me->SetDirty(true);
		}

		void operator()(){
			a = getter();
		}

		T get() const{
			return getter();
		}

	private:

		Getter getter;
		T a;
	};
}