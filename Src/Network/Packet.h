#pragma once

#include <cstdint>
#include <vector>
#include <list>
#include <map>
#include <string>

#pragma warning(disable: 4251)

namespace Opcode{
	typedef const uint32_t __opcode;
}

namespace Framework
{
	namespace Network
	{
		class Packet
		{
		public:

			enum{
				kHandshake,
				kMessage
			};

			Packet();
			/**
			 * Construct a packet.
			 * @param pOpcode Opcode to send.
			 * @param pObjectId The object's id.
			 * @param pType The packet's type (ping, handshake or packet).
			 */
			Packet(uint32_t pOpcode, uint8_t pType = kMessage);
			Packet(Packet&& pPacket);
			
			Packet& operator=(Packet&& pPacket);

			Packet& operator<<(const uint8_t& pData);
			Packet& operator<<(const uint16_t& pData);
			Packet& operator<<(const uint32_t& pData);
			Packet& operator<<(const uint64_t& pData);
			Packet& operator<<(const int8_t& pData);
			Packet& operator<<(const int16_t& pData);
			Packet& operator<<(const int32_t& pData);
			Packet& operator<<(const int64_t& pData);
			Packet& operator<<(const float& pData);
			Packet& operator<<(const double& pData);
			Packet& operator<<(const std::string& pData);

			Packet& operator>>(uint8_t& pData);
			Packet& operator>>(uint16_t& pData);
			Packet& operator>>(uint32_t& pData);
			Packet& operator>>(uint64_t& pData);
			Packet& operator>>(int8_t& pData);
			Packet& operator>>(int16_t& pData);
			Packet& operator>>(int32_t& pData);
			Packet& operator>>(int64_t& pData);
			Packet& operator>>(float& pData);
			Packet& operator>>(double& pData);
			Packet& operator>>(std::string& pData);

			template <class K, class U>
			Packet& operator<<(const std::map<K, U>& pData)
			{
				*this << (uint32_t)pData.size();
				for(auto itor = pData.begin(), end = pData.end(); itor != end; ++itor)
				{
					*this << itor->first << itor->second;
				}

				return *this;
			}

			template <class K, class U>
			Packet& operator>>(std::map<K, U>& pData)
			{
				uint32_t size;
				*this >> size;
				if(size > 1000)
					return *this;

				for(uint64_t i = 0; i < size; ++i)
				{
					K key;
					U data;
					*this >> key >> data;
					pData.insert(std::pair<K,U>(key, data));
				}

				return *this;
			}

			template <class U>
			Packet& operator<<(const std::vector<U>& pData)
			{
				*this << (uint32_t)pData.size();
				for(auto itor = pData.begin(), end = pData.end(); itor != end; ++itor)
				{
					*this << *itor;
				}

				return *this;
			}

			template <class U>
			Packet& operator>>(std::vector<U>& pData)
			{
				uint32_t size;
				*this >> size;
				if(size > 1000)
					return *this;

				for(uint64_t i = 0; i < size; ++i)
				{
					U data;
					*this >> data;
					pData.push_back(data);
				}

				return *this;
			}

			template <class U>
			Packet& operator<<(const std::list<U>& pData)
			{
				*this << (uint32_t)pData.size();
				for(auto itor = pData.begin(), end = pData.end(); itor != end; ++itor)
				{
					*this << *itor;
				}

				return *this;
			}

			template <class U>
			Packet& operator>>(std::list<U>& pData)
			{
				uint32_t size;
				*this >> size;
				if(size > 1000)
					return *this;

				for(uint64_t i = 0; i < size; ++i)
				{
					U data;
					*this >> data;
					pData.push_back(data);
				}

				return *this;
			}

			void InitializeHeader	(const std::string& pData);
			void Initialize			(const std::vector<char>& pData);
			void Initialize			(const std::string& pData);
			void Write				(uint8_t* pData, size_t pSize, int32_t pPos = -1);

			uint32_t ReadObjectId();
			int32_t GetPosition();

			std::string			GetHeader();
			const std::string&	GetBuffer();

			void Finalize();

			uint8_t  Type;
			uint32_t Size;
			uint32_t Opcode;
			uint32_t ObjectId;

		protected:

			std::string					mBuffer;
			uint8_t						mChecksum;
		};
	}
}