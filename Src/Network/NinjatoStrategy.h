#pragma once

#include <Crypt/Cipher.h>
#include <Crypt/RSA.h>
#include <Network/Strategy.h>
#include <Network/Packet.h>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/timer.hpp>
#include <cstdint>


namespace Framework
{
	namespace Network
	{
		struct NinjatoStrategy : public Strategy<Framework::Network::Packet>
		{
			NinjatoStrategy()
			{
			}

			uint32_t GetHeaderSize()
			{
				return 6;
			}

			uint32_t HandleHeader(const std::string& pData)
			{
				std::string data = pData;

				if(mCipher)
					data = mCipher->Decrypt(data);

				mReceivingPacket.InitializeHeader(data);

				return mReceivingPacket.Size - 6;
			}

			bool	 HandleContent(const std::string& pData)
			{
				if(mCipher)
				{

					// Decrypt then decompress the payload
					std::string data = mCipher->Decompress(mCipher->Decrypt(pData));

					if(!data.empty())
					{
						//System::PrintBinary((unsigned char*)&data[0], data.size(), "Client -> Server");
						// Initialize the packet buffer
						mReceivingPacket.Initialize(data);
						// Extract the header
						mReceivingPacket >> mReceivingPacket.Opcode;
						mReceivingPacket.ReadObjectId();
					}
				}
				else
				{
					// Initialize the packet buffer
					mReceivingPacket.Initialize(pData);
				}

				ReceivePacket(mReceivingPacket);

				return true;
			}

			void ReceivePacket(Framework::Network::Packet& pPacket)
			{
				boost::mutex::scoped_lock locker(mLock);
				mPackets.push_back(pPacket);
			}

			std::string Serialize(Framework::Network::Packet& pData)
			{
				std::string outboundData;
				if(pData.Type == Framework::Network::Packet::kHandshake)
				{
					std::string rsaData = Crypt::RSA::Encrypt(pData.GetBuffer());
					pData.Size = 14 + rsaData.size();
					outboundData = pData.GetHeader() + rsaData;
				}
				else if(mCipher && pData.Type != Framework::Network::Packet::kHandshake)
				{
					std::string data = pData.GetHeader().substr(6) + pData.GetBuffer();

					std::string compressedPayload = mCipher->Compress(data);
					pData.Size = (uint32_t)compressedPayload.size() + 6;

					outboundData = pData.GetHeader().substr(0,6) + compressedPayload;
					outboundData = mCipher->Encrypt(outboundData);
				}
				else
				{
					pData.Size = (uint32_t)pData.GetBuffer().size() + 14;
					outboundData = pData.GetHeader() + pData.GetBuffer();
				}
				return outboundData;
			}

			bool HasPacket()
			{
				return !mPackets.empty();
			}

			Framework::Network::Packet PopPacket()
			{
				boost::mutex::scoped_lock locker(mLock);
				Framework::Network::Packet packet = mPackets.front();
				mPackets.pop_front();
				return packet;				
			}

		protected:

			std::unique_ptr<Framework::Crypt::Cipher> mCipher;

		private:

			boost::mutex mLock;
			Framework::Network::Packet mReceivingPacket;
			std::list<Framework::Network::Packet> mPackets;
		};
	}
}