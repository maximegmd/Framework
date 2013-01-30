#include <Network/Packet.h>
#include <sstream>

namespace Framework
{
	namespace Network
	{
		//---------------------------------------------------------------------
		Packet::Packet()
			:Opcode(0), ObjectId(0), Type(kMessage)
		{
		}
		//---------------------------------------------------------------------
		Packet::Packet(uint32_t pOpcode, uint8_t pType)
			:Opcode(pOpcode), ObjectId(0), Type(pType)
		{
		}
		//---------------------------------------------------------------------
		Packet::Packet(Packet&& pPacket)
			:Opcode(std::move(pPacket.Opcode)), 
			 ObjectId(std::move(pPacket.ObjectId)), 
			 Type(std::move(pPacket.Type)), 
			 Size(std::move(pPacket.Size)), 
			 mBuffer(std::move(pPacket.mBuffer))
		{
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator=(Packet&& pPacket)
		{
			Opcode = std::move(pPacket.Opcode);
			ObjectId = std::move(pPacket.ObjectId); 
			Type = std::move(pPacket.Type); 
			Size = std::move(pPacket.Size);
			mBuffer = std::move(pPacket.mBuffer);
			
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(bool pData)
		{
			*this << uint8_t(pData ? 1 : 0);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(char pData)
		{
			mBuffer.append((char*)&pData, 1);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const uint8_t& pData)
		{
			mBuffer.append((char*)&pData, 1);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const uint16_t& pData)
		{
			mBuffer.append((char*)&pData, 2);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const uint32_t& pData)
		{
			mBuffer.append((char*)&pData, 4);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const uint64_t& pData)
		{
			mBuffer.append((char*)&pData, 8);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const int8_t& pData)
		{
			mBuffer.append((char*)&pData, 1);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const int16_t& pData)
		{
			mBuffer.append((char*)&pData, 2);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const int32_t& pData)
		{
			mBuffer.append((char*)&pData, 4);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const int64_t& pData)
		{
			mBuffer.append((char*)&pData, 8);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const float& pData)
		{
			mBuffer.append((char*)&pData, 4);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const double& pData)
		{
			mBuffer.append((char*)&pData, 8);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator<<(const std::string& pData)
		{
			*this << (uint32_t)pData.size();
			mBuffer.append(pData.c_str(), pData.size());
			
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(bool& pData)
		{
			uint8_t i;
			*this >> i;
			pData = i ? true : false;
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(char& pData)
		{
			if(mBuffer.size() < 1)
			{
				pData = 0;
				return *this;
			}
			pData = *(uint8_t*)&mBuffer[0];
			mBuffer.erase(0,1);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(uint8_t& pData)
		{
			if(mBuffer.size() < 1)
			{
				pData = 0;
				return *this;
			}
			pData = *(uint8_t*)&mBuffer[0];
			mBuffer.erase(0,1);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(uint16_t& pData)
		{
			if(mBuffer.size() < 2)
			{
				pData = 0;
				return *this;
			}
			pData = *(uint16_t*)&mBuffer[0];
			mBuffer.erase(0,2);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(uint32_t& pData)
		{
			if(mBuffer.size() < 4)
			{
				pData = 0;
				return *this;
			}
			pData = *(uint32_t*)&mBuffer[0];
			mBuffer.erase(0,4);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(uint64_t& pData)
		{
			if(mBuffer.size() < 8)
			{
				pData = 0;
				return *this;
			}
			pData = *(uint64_t*)&mBuffer[0];
			mBuffer.erase(0,8);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(int8_t& pData)
		{
			if(mBuffer.size() < 1)
			{
				pData = 0;
				return *this;
			}
			pData = *(int8_t*)&mBuffer[0];
			mBuffer.erase(0,1);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(int16_t& pData)
		{
			if(mBuffer.size() < 2)
			{
				pData = 0;
				return *this;
			}
			pData = *(int16_t*)&mBuffer[0];
			mBuffer.erase(0,2);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(int32_t& pData)
		{
			if(mBuffer.size() < 4)
			{
				pData = 0;
				return *this;
			}
			pData = *(int32_t*)&mBuffer[0];
			mBuffer.erase(0,4);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(int64_t& pData)
		{
			if(mBuffer.size() < 8)
			{
				pData = 0;
				return *this;
			}
			pData = *(int64_t*)&mBuffer[0];
			mBuffer.erase(0,8);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(float& pData)
		{
			if(mBuffer.size() < 4)
			{
				pData = 0.f;
				return *this;
			}
			pData = *(float*)&mBuffer[0];
			mBuffer.erase(0,4);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(double& pData)
		{
			if(mBuffer.size() < 8)
			{
				pData = 0;
				return *this;
			}
			pData = *(double*)&mBuffer[0];
			mBuffer.erase(0,8);
			return *this;
		}
		//---------------------------------------------------------------------
		Packet& Packet::operator>>(std::string& pData)
		{
			uint32_t size = 0;
			*this >> size;

			// Check for fake string size to prevent memory hacks
			if(size > mBuffer.size())
			{
				std::ostringstream os;
				os << "String size (" << size << ") > packet size (" << mBuffer.size() << ")";
				throw std::out_of_range(os.str());
			}
			if(size == 0)
			{
				pData = "";
				return *this;
			}

			pData = mBuffer.substr(0,size);

			pData.shrink_to_fit();

			mBuffer.erase(0,size);
			return *this;
		}
		//---------------------------------------------------------------------
		void Packet::Write(uint8_t* pData, size_t pSize, int32_t pPos)
		{
			if(pPos < 0)
				mBuffer.append((char*)pData, pSize);
			else if(pPos + pSize <= mBuffer.size())
			{
				memcpy(&mBuffer[pPos], pData, pSize);
			}
		}
		//---------------------------------------------------------------------
		void Packet::Initialize(const std::vector<char>& pData)
		{
			mBuffer.clear();
			mBuffer.append(&pData[0], pData.size());
		}
		//---------------------------------------------------------------------
		void Packet::Initialize(const std::string& pData)
		{
			mBuffer.clear();
			mBuffer.append(&pData[0], pData.size());
		}
		//---------------------------------------------------------------------
		uint32_t Packet::ReadObjectId()
		{
			*this >> ObjectId;
			return ObjectId;
		}
		//---------------------------------------------------------------------
		void Packet::InitializeHeader(const std::string& pData)
		{
			Type = *(uint8_t*)&pData[0];
			Size = *(uint32_t*)&pData[1];
			mChecksum = *(uint8_t*)&pData[5];
		}
		//---------------------------------------------------------------------
		std::string Packet::GetHeader()
		{
			Finalize();

			std::string data;

			data.append((char*)&Type, 1);
			data.append((char*)&Size, 4);
			data.append((char*)&mChecksum, 1);
			data.append((char*)&Opcode, 4);
			data.append((char*)&ObjectId, 4);

			return data;
		}
		//---------------------------------------------------------------------
		int32_t Packet::GetPosition()
		{
			return (int32_t)mBuffer.size();
		}
		//---------------------------------------------------------------------
		const std::string& Packet::GetBuffer()
		{
			return mBuffer;
		}
		//---------------------------------------------------------------------
		void Packet::Finalize()
		{
			mChecksum = Type ^ ((char*)&Size)[0] ^ ((char*)&Size)[1] ^ ((char*)&Size)[2] ^ ((char*)&Size)[3];
		}
		//---------------------------------------------------------------------
	}
}