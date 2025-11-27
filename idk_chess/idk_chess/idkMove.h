#pragma once
#include "idkUtils.h"
#include "idkBoard.h"
#include "SFML/Network.hpp"
#include "vector"
class idkMove
{
	int piece;
	int fromSq;
	int toSq;
	bool color; // true if white, false is black
	int capture_piece; //option: -1 if no capture, otherwise same format as any other piece [0000] id [00] color [000] piece
	int flags;
	idkUtils::piece_type PROMO = idkUtils::NO_PIECE;
	
	static int en_passant;
	static bool legalPawnMove(int color, int capture, int fromSq, int toSq, int & flags, idkBoard &current_board);
	static bool legalBishopMove(int fromSq, int toSq, int& flags, idkBoard& current_board);
	static bool legalKnightMove(int fromSq, int toSq, int& flags, idkBoard& current_board);
	static bool legalRookMove(int fromSq, int toSq, int& flags, idkBoard& current_board);
	static bool legalQueenMove(int fromSq, int toSq, int& flags, idkBoard& current_board);
	static bool legalKingMove(int fromSq, int toSq, int& flags, idkBoard& current_board);

	
public:
	idkMove();
	idkMove(bool clr, int piece, int from, int to, int capture_piece,int flags, idkUtils::piece_type promo);

	static std::vector<idkMove> generateMoves(idkUtils::BoardPiece piece, idkBoard& current_board);
	static bool isLegalMove(int fromSq, int toSq, int&flags, idkBoard &current_board);
	static bool isKingAttacked(idkBoard& current_board,bool);

	bool applyMove(idkBoard &current_board);
	bool unApplyMove(idkBoard &current_board);

	

	friend sf::Packet& operator <<(sf::Packet& packet, const idkMove& move);

	friend sf::Packet& operator >>(sf::Packet& packet, idkMove& move);

};


