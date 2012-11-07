#pragma once

#include <string>
#include <functional>
#include <list>

#include <Game/Service.hpp>

namespace Game
{
	class ServiceDatabase
	{
	public:

		/**
		 * @brief Constructs a Service database.
		 * @param m The Model to store.
		 */
		ServiceDatabase();
		virtual ~ServiceDatabase();

		void AddService(IService*);
		void Update();

	protected:

		std::list<std::shared_ptr<IService> > mServices;
	};
}