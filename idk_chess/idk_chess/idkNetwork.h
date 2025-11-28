#pragma once
#include "SFML/Network.hpp"
#include "SFML/Network/Packet.hpp"
#include "string"
#include <iostream>
#include "idkMove.h"
#include "idkChess.h"
class idkNetwork
{
	idkChess* network_game;

public:

	struct ChessData {

		std::string fen;
		std::string ply_move;
		std::string message; //optional

	};


	idkNetwork(idkChess*);
	void hostServer();
	void connectClient(sf::IpAddress hostAddress);
	
	void endServer();
	void disconnectClient();

	
};

sf::Packet& operator <<(sf::Packet& packet, const idkNetwork::ChessData& cd);

sf::Packet& operator >>(sf::Packet& packet, idkNetwork::ChessData& cd);

