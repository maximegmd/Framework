#pragma once

#include <map>
#include <vector>

#include <Network/Server.h>
#include <Network/Packet.h>

#include <Game/Player.hpp>
#include <Game/GOMDatabase.hpp>

namespace Game
{
	class GameServer
	{
	public:

		typedef std::function<Player* (Player::KeyType, GameServer*)> PlayerConstructor;
		typedef std::function<std::vector<IGOMServer*> (void*)>	GOMServerConstructor;

		/**
		 * @brief Constructs a GameServer instance
		 * @param port The port on which the server will listen
		 * @param The functor handling player construction
		 * @param The functor handling GOM Server construction
		 */
		GameServer(short port, PlayerConstructor, GOMServerConstructor);
		~GameServer();

		/**
		 * @brief Update's the GameServer state.
		 */
		void Update();

		/**
		 * @brief Send a message to every player currently connected to the game server.
		 * @param pPacket The packet to send.
		 */
		void SendMessageAll(Framework::Network::Packet& pPacket);

		/**
		 * @brief Send a message to every player currently connected to the game server and synchronized.
		 * @param pPacket The packet to send.
		 */
		void SendMessageAllSynchronized(Framework::Network::Packet& pPacket);

		/**
		 * @brief Sets the size of the GOM's view distance.
		 * @param cellsize The size of the GOM's view distance.
		 */
		void SetCellSize(int cellsize);

		/**
		 * @brief Gets the size of the GOM's view distance.
		 * @return The size of the GOM's view distance.
		 */
		int GetCellSize() const;

		/**
		 * @brief Obtain the GOM Server associated with the GameServer.
		 * @return The GOM Server.
		 */
		GOMDatabase& GetGOMDatabase() const;

		/**
		 * @brief Callback handling new client connections to the GameServer.
		 * @param pConnection The new connection.
		 */
		void OnConnection(Framework::Network::TcpConnection::pointer pConnection);

	private:

		void SendReplicationTransaction(GOMVisitor& visitor);

		std::map<Player::KeyType, Player*> players;

		std::unique_ptr<Framework::Network::Server>			server;
		std::unique_ptr<GOMDatabase>						gomDatabase;

		boost::timer mTransactionFullTimer;
		boost::timer mTransactionPartialTimer;

		int cellSize;

		friend class MassiveMessageManager;

		PlayerConstructor		playerContructor;
		GOMServerConstructor	gomServerConstructor;
	};
}