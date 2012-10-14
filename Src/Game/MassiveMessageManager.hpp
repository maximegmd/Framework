#pragma once

#include <Game/Player.hpp>
#include <Game/NetProperties.hpp>
#include <Game/GOMServer.hpp>
#include <Network/Server.h>
#include <Network/IoServicePool.h>

namespace Game
{
	class MassiveMessageManager
	{
	public:

		enum{
			kPlayerSelf = -3,
			kPlayerServer = -2,
			kPlayerAll = -1,
		};

		typedef std::function<Player* (Player::KeyType, void*)> PlayerConstructor;
		typedef std::function<IGOMServer* (void*)>				GOMServerConstructor;

		MassiveMessageManager();
		~MassiveMessageManager();

		void BeginMultiplayer(bool);

		void SetPort(uint16_t);
		void SetAddress(const std::string&);

		void SetPlayerConstructor(PlayerConstructor& ctor);
		void SetGOMServerConstructor(GOMServerConstructor& ctor);

		void Connect(const std::string& pAddress, const std::string& pPort);

		void Query();

		void OnConnection(Framework::Network::TcpConnection::pointer pConnection);

		Game::Player* GetLocalPlayer();

		void SendMessageAll(Framework::Network::Packet& pPacket);
		void SendMessage(Game::Player::KeyType pKey, Framework::Network::Packet& pPacket);

		bool Server();

	private:

		void OnConnect(bool pConnected);

		PlayerConstructor		playerContructor;
		GOMServerConstructor	gomServerConstructor;

		std::map<Player::KeyType, Player*> players;
		std::unique_ptr<Player> localPlayer;

		std::unique_ptr<IGOMServer>							gomServer;

		bool host;
		uint16_t port;
		std::string address;

		//! Server side
		std::unique_ptr<Framework::Network::Server>			server;

		//! Client side
		Framework::Network::IoServicePool ioServicePool;
		Framework::Network::TcpConnection::pointer			connection;
	};

}

extern Game::MassiveMessageManager* TheMassiveMessageMgr;