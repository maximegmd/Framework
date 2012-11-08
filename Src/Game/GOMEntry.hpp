#pragma once

#include <string>
#include <functional>
#include <boost/timer.hpp>

namespace Game
{
	class IGOMEntry
	{
	public:

		IGOMEntry()
		{
			mTimer.restart();
		}

		virtual ~IGOMEntry()
		{
		}

		enum
		{
			kDirty = 1 << 0
		};

		/**
		 * @brief Checks if the GOM Entry is dirty or not.
		 */
		virtual void Update() = 0;

		/**
		 * @brief Cleans the GOM Entry by setting all of its previous variables to the current variables.
		 */
		virtual void Synchronize() = 0;
		/**
		 * @brief Tells wether or not the GOM Entry has changed since it was last synchronized.
		 * @return true if ditry, false if not.
		 */
		bool IsDirty();

		/**
		 * @brief Serializes the GOM Entry's state.
		 * @param pFull Returns whether or not the serialization should be complete.
		 * @return The raw serialized data.
		 */
		std::string Serialize(bool pFull)
		{
			std::string data = DoSerialize(pFull);
			Synchronize();
			return data;
		}
		/**
		 * @brief Deserializes the GOM Entry's state.
		 * @param plainData The serialized raw data.
		 */
		void Deserialize(const std::string& plainData)
		{
			DoDeserialize(plainData);
			mTimer.restart();
		}

		/**
		 * @brief Sets whether or not the GOM Entry is dirty.
		 * @param dirty The dirtiness of the GOM Entry.
		 */
		void SetDirty(bool dirty);

		float GetDelta()
		{
			return mTimer.elapsed();
		}

	protected:

		virtual std::string DoSerialize(bool pFull) const = 0;
		virtual void DoDeserialize(const std::string& plainData) = 0;

		template <class T>
		friend struct GOMVariable;

	private:

		boost::timer mTimer;
		unsigned char flags;
	};

	template <class Model>
	class GOMEntry : public IGOMEntry
	{
	public:


		typedef typename Model Type;
		/**
		 * @brief Constructs a GOM Entry with its associated Model.
		 * @param m The Model to store.
		 */
		GOMEntry(Type* d) 
			: data(d), IGOMEntry()
		{
			SetDirty(false);
		}

		virtual ~GOMEntry()
		{
		}

		Type* GetData()
		{
			return data;
		}

	protected:


		Type* data;
	};


}