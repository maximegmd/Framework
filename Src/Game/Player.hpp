#pragma once

#include <Network/Packet.h>
#include <Network/TcpConnection.h>
#include <Network/NinjatoStrategy.h>

namespace Game
{
	class Player : public Framework::Network::NinjatoStrategy
	{
	public:

		enum {
			kSynchronize = 'sync'
		};

		typedef int32_t KeyType;

		Player(KeyType);

		void SetConnection(Framework::Network::TcpConnection::pointer pConnection);

		KeyType GetKey() const;

	protected:

		friend class MassiveMessageManager;

		void Write(Framework::Network::Packet& pPacket);
		void SetCipher(Framework::Crypt::Cipher* pCipher);
		void Synchronize();

		void Update();

	private:

		void HandleHandshake(Framework::Network::Packet& pPacket);
		void HandleSynchronize(Framework::Network::Packet& pPacket);

		KeyType key;
		Framework::Network::TcpConnection::pointer connection;
	};
}