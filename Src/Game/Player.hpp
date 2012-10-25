#pragma once

#include <Network/Packet.h>
#include <Network/TcpConnection.h>
#include <Network/NinjatoStrategy.h>
#include <Game/GOMDatabase.hpp>

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
			kReplicationTransaction = 'rptr',
			kAwareness = 'awar'
		};

		typedef int KeyType;
		typedef void (Player::*PacketHandler)(Framework::Network::Packet& pPacket);

		/**
		 * @brief Constructs a Player.
		 * @param id The Player's id.
		 * @param server The GameServer associated with the Player, nullptr if none.
		 */
		Player(KeyType id, GameServer* server = nullptr);

		virtual ~Player();
		/**
		 * @brief Set the connection associated with the player.
		 * @param pConnection The connection associated with the player.
		 */
		void SetConnection(Framework::Network::TcpConnection::pointer pConnection);
		/**
		 * @brief Get the player's id.
		 * @return The player's id.
		 */
		KeyType GetKey() const;

		bool Synchronized() const;

		void SendAwareness();

		void OnError(const std::string&);

	protected:

		friend class MassiveMessageManager;
		friend class GameServer;

		/**
		 * @brief Write a packet to the player.
		 * @param pPacket The packet to write.
		 * @return
		 */
		void Write(Framework::Network::Packet& pPacket);
		/**
		 * @brief The a custom cipher.
		 * @param pCipher The cipher.
		 */
		void SetCipher(Framework::Crypt::Cipher* pCipher);
		/**
		 * @brief Send a Synchronization packet from the server to the client.
		 */
		void SendSynchronize();
		/**
		 * @brief Send a replication transaction to the client.
		 * @param gomServer The GOM server to replicate.
		 */
		void SendReplicationTransaction(Game::GOMDatabase* gomServer);
		/**
		 * @brief Pump the events.
		 */
		void Update();

		/**
		 * @brief Register a handler to an opcode.
		 * @param opcode The opcode.
		 * @param handler The handler to use.
		 */
		template <class T>
		static void Register(int32_t opcode, void (T::*handler)(Framework::Network::Packet& pPacket))
		{
			RegisterImpl(opcode, (PacketHandler)handler);
		}

		GameServer* gameServer;

	private:

		static void RegisterImpl(int32_t opcode, PacketHandler handler);

		void HandleReplicationTransaction(Framework::Network::Packet& pPacket);
		void HandleHandshake(Framework::Network::Packet& pPacket);
		void HandleSynchronize(Framework::Network::Packet& pPacket);
		void HandleAwareness(Framework::Network::Packet& pPacket);

		bool synchronized;
		KeyType key;

		Framework::Network::TcpConnection::pointer connection;

		static std::map<int32_t, PacketHandler> handlers;
	};
}