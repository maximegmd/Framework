#pragma once

#include <string>
#include <functional>

namespace Game
{
	class IGOMEntry
	{
	public:

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
		 * @return The raw serialized data.
		 */
		virtual std::string Serialize() const = 0;
		/**
		 * @brief Deserializes the GOM Entry's state.
		 * @param plainData The serialized raw data.
		 */
		virtual void Deserialize(const std::string& plainData) = 0;

		/**
		 * @brief Sets wether or not the GOM Entry is dirty.
		 * @param dirty The dirtiness of the GOM Entry.
		 */
		void SetDirty(bool dirty);

	protected:

		template <class T>
		friend struct GOMVariable;

	private:

		unsigned char flags;
	};

	template <class Model>
	class GOMEntry : public IGOMEntry
	{
	public:

		/**
		 * @brief Constructs a GOM Entry with its associated Model.
		 * @param m The Model to store.
		 */
		GOMEntry(Model* m) 
			: model(m)
		{
		}

	protected:

		Model* model;
	};


}