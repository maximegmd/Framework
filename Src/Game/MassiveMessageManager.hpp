#pragma once

#include <Game/Player.hpp>
#include <Game/NetProperties.hpp>
#include <Game/GOMServer.hpp>
#include <Game/GameServer.hpp>
#include <Network/Server.h>
#include <Network/IoServicePool.h>

namespace Game
{
	class MassiveMessageManager
	{
	public:

		/**
		 * @brief Constructs the MassiveMessageManager.
		 */
		MassiveMessageManager();
		/**
		 * @brief Destructs the MassiveMessageManager.
		 */
		~MassiveMessageManager();
		/**
		 * @brief Starts the MassiveMessageManager.
		 * @param mode true for server side, false for client side.
		 */
		void BeginMultiplayer(bool mode);
		/**
		 * @brief Sets the hosting port or the port to connect to.
		 * @param port The port.
		 */
		void SetPort(uint16_t port);
		/**
		 * @brief Sets the address to connect to.
		 * @param address The address.
		 */
		void SetAddress(const std::string& address);
		/**
		 * @brief Sets a custom player constructor.
		 * @param ctor The custom player constructor.
		 */
		void SetPlayerConstructor(GameServer::PlayerConstructor& ctor);
		/**
		 * @brief Sets a custom GOM Server constructor.
		 * @param ctor The custom GOM Server constructor.
		 */
		void SetGOMServerConstructor(GameServer::GOMServerConstructor& ctor);
		/**
		 * @brief Connect to a remove server.
		 * @param pAddress The server's address.
		 * @param pPort The server's port.
		 */
		void Connect(const std::string& pAddress, const std::string& pPort);
		/**
		 * @brief Update the event queue.
		 */
		void Update();
		/**
		 * @brief Get the local player.
		 * @return The local player or nullptr if in server mode.
		 */
		Game::Player* GetLocalPlayer();

		/**
		 * @brief Sends a message to the specified players.
		 * @param pKey The players to send to.
		 * @param pPacket The packet to send.
		 */
		void SendMessageTo(int pKey, Framework::Network::Packet& pPacket);
		/**
		 * @brief Sends a message to everyone, be it only the server or every player.
		 * @param pPacket The packet to send.
		 */
		void SendMessageAll(Framework::Network::Packet& pPacket);

		/**
		 * @brief Get if in server mode.
		 * @return true if in server mode, false if in client mode.
		 */
		bool Server();
		/**
		 * @brief Get the GOM server.
		 * @return The GOM server.
		 */
		GOMDatabase* GetGOMDatabase() const;

	private:

		void OnConnect(bool pConnected);

		
		GameServer::PlayerConstructor playerConstructor;
		GameServer::GOMServerConstructor gomConstructor;

		std::unique_ptr<Player> localPlayer;

		std::unique_ptr<GOMDatabase>	gomDatabase;

		bool host;
		uint16_t port;
		std::string address;

		//! Server side
		std::unique_ptr<GameServer>	gameServer;

		//! Client side
		Framework::Network::IoServicePool ioServicePool;
		Framework::Network::TcpConnection::pointer			connection;
	};

}

extern Game::MassiveMessageManager* TheMassiveMessageMgr;