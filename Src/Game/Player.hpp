#pragma once

#include <Network/Packet.h>
#include <Network/TcpConnection.h>
#include <Network/NinjatoStrategy.h>
#include <Game/GOMServer.hpp>

namespace Game
{
	enum{
		kPlayerSelf = -3,
		kPlayerServer = -2,
		kPlayerAll = -1,
	};

	class GameServer;
	class Player : public Framework::Network::NinjatoStrategy
	{
	public:

		enum {
			kSynchronize = 'sync',
			kReplicationTransaction = 'rptr'
		};

		typedef int32_t KeyType;

		Player(KeyType, GameServer* server = nullptr);

		void SetConnection(Framework::Network::TcpConnection::pointer pConnection);

		KeyType GetKey() const;

	protected:

		friend class MassiveMessageManager;
		friend class GameServer;

		void Write(Framework::Network::Packet& pPacket);
		void SetCipher(Framework::Crypt::Cipher* pCipher);
		void Synchronize();

		void SendReplicationTransaction(IGOMServer& gomServer);

		void Update();

	private:

		void HandleHandshake(Framework::Network::Packet& pPacket);
		void HandleSynchronize(Framework::Network::Packet& pPacket);

		KeyType key;
		GameServer* gameServer;
		Framework::Network::TcpConnection::pointer connection;
	};
}