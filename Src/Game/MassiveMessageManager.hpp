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

		MassiveMessageManager();
		~MassiveMessageManager();

		void BeginMultiplayer(bool);

		void SetPort(uint16_t);
		void SetAddress(const std::string&);

		void SetPlayerConstructor(GameServer::PlayerConstructor& ctor);
		void SetGOMServerConstructor(GameServer::GOMServerConstructor& ctor);

		void Connect(const std::string& pAddress, const std::string& pPort);

		void Update();

		Game::Player* GetLocalPlayer();

		void SendMessageAll(Framework::Network::Packet& pPacket);
		void SendMessage(Game::Player::KeyType pKey, Framework::Network::Packet& pPacket);

		bool Server();

	private:

		void OnConnect(bool pConnected);

		
		GameServer::PlayerConstructor playerConstructor;
		GameServer::GOMServerConstructor gomConstructor;

		std::unique_ptr<Player> localPlayer;

		std::unique_ptr<IGOMServer>							gomServer;

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