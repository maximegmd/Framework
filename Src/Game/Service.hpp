#pragma once

#include <string>
#include <functional>
#include <cstdint>

namespace Game
{
	class IService
	{
	public:

		virtual ~IService()
		{
		}

		/**
		 * @brief Checks if the GOM Entry is dirty or not.
		 */
		virtual void Update() = 0;

		/**
		 * @brief Serializes the GOM Entry's state.
		 * @param pFull Returns whether or not the serialization should be complete.
		 * @return The raw serialized data.
		 */
		std::string Serialize(bool pFull)
		{
			std::string data = DoSerialize(pFull);
			return data;
		}
		/**
		 * @brief Deserializes the Service's state.
		 * @param plainData The serialized raw data.
		 */
		void Deserialize(const std::string& plainData)
		{
			DoDeserialize(plainData);
		}

		virtual int32_t GetKey() const = 0;

	protected:

		virtual std::string DoSerialize(bool pFull) const = 0;
		virtual void DoDeserialize(const std::string& plainData) = 0;
	};

	template <int32_t Type>
	class Service : public IService
	{
	public:

		static const int32_t Key = Type;

		/**
		 * @brief Constructs a GOM Entry with its associated Model.
		 * @param m The Model to store.
		 */
		Service() 
		{
		}

		virtual ~Service()
		{
		}

		int32_t GetKey() const
		{
			return Key;
		}

	protected:
	};


}