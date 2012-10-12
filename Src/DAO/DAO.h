#pragma once

#include <DataBase/Database.h>
#include <DataBase/Column.h>
#include <DataBase/Query.h>
#include <boost/make_shared.hpp>
#include <boost/signal.hpp>

using namespace std;

namespace Framework{
	//---------------------------------------------------------------------
	namespace DAO
	{
		/**
		 * @class IDAO
		 * DAO Interface, abstracts template parameters
		 */
		class IDAO
		{
		public:

			/**
			 * @brief Constructor
			 */
			IDAO(){}
			/**
			 * @brief Destructor
			 */
			virtual ~IDAO(){}

			/**
			 * @brief Load a DAO from connector
			 * @param pDatabase The database to use when loading
			 * @return
			 */
			virtual void Load(SimpleDB::Database& pDatabase) = 0;
			/**
			 * @brief Save a DAO from connector
			 * @param pDatabase The database to use when saving
			 * @return
			 */
			virtual void Save(SimpleDB::Database& pDatabase) = 0;
			

			enum Type
			{
				LOAD,SAVE
			};

			/**
			 * @brief Returns the DAO type
			 * @return The DAO type
			 */
			virtual IDAO::Type GetType() = 0;

			unsigned int userData;

			boost::signal<void(Type, bool)> OnEvent;

		protected:

		};
		/**
		 * @brief DAO implementation
		 */
		template <class T>
		class DAO : public IDAO
		{
			T& mEntity;
			IDAO::Type mType;
		public:

			/**
			 * @brief Construct a data object access
			 * @param pEntity The entity we are working on
			 * @param pType The type of request (save or load)
			 */
			DAO(T& pEntity, IDAO::Type pType)
				:mEntity(pEntity),mType(pType)
			{
			}

			virtual ~DAO(){}

			IDAO::Type GetType()
			{
				return mType;
			}
			/**
			 * @brief Save a DAO from connector
			 * @param pDatabase The database to use when saving
			 * @return
			 */
			void Load(SimpleDB::Database& pDatabase);
			/**
			 * @brief Save a DAO from connector
			 * @param pDatabase The database to use when saving
			 * @return
			 */
			void Save(SimpleDB::Database& pDatabase);
		};

		/**
		 * @brief Create a load DAO
		 * @param pObject The object to load
		 * @return The DAO's smart pointer
		 */
		template <class T>
		boost::shared_ptr<IDAO> Load(T& pObject)
		{
			return boost::shared_ptr<IDAO>(new DAO<T>(pObject, IDAO::LOAD));
		}

		/**
		 * @brief Create a save DAO
		 * @param pObject The object to save
		 * @return The DAO's smart pointer
		 */
		template <class T>
		boost::shared_ptr<IDAO> Save(T& pObject)
		{
			return boost::shared_ptr<IDAO>(new DAO<T>(pObject, IDAO::SAVE));
		}
	}
}