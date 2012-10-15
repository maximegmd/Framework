#pragma once

#include <map>
#include <Network/Server.h>
#include <Network/Packet.h>

#include <Game/Player.hpp>
#include <Game/GOMServer.hpp>

namespace Game
{
	class GameServer
	{
	public:

		typedef std::function<Player* (Player::KeyType, void*)> PlayerConstructor;
		typedef std::function<IGOMServer* (void*)>				GOMServerConstructor;

		GameServer(short port, PlayerConstructor, GOMServerConstructor);
		~GameServer();

		void Update();

		void SendMessageAll(Framework::Network::Packet& pPacket);

		void SetCellSize(int cellsize);
		int GetCellSize() const;

		IGOMServer& GetGOMServer() const;

		void OnConnection(Framework::Network::TcpConnection::pointer pConnection);

	protected:

		void SendReplicationTransaction();

	private:

		std::map<Player::KeyType, Player*> players;

		std::unique_ptr<Framework::Network::Server>			server;
		std::unique_ptr<IGOMServer>							gomServer;

		int cellSize;

		friend class MassiveMessageManager;

		PlayerConstructor		playerContructor;
		GOMServerConstructor	gomServerConstructor;
	};
}