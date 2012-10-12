#pragma once

namespace Framework
{
	namespace System
	{
		template <class T>
		class Singleton
		{
		public:
			static T& GetSingleton()
			{
				if(mInstance == nullptr)
					mInstance = new T();
				return *mInstance;
			}

			static T* GetSingletonPtr()
			{
				if(mInstance == nullptr)
					mInstance = new T();
				return mInstance;
			}

			static void Kill()
			{
				if(mInstance != nullptr)
					delete mInstance;
				mInstance = nullptr;
			}

		protected:

			static T* mInstance;
		};
	}
}