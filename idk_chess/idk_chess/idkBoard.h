#pragma once
#include "idkUtils.h"
#include <string>
class idkBoard
{
	int boardSquares[64];

	idkUtils::BoardPiece Black_Pieces[16];
	idkUtils::BoardPiece White_Pieces[16];

	int whiteAttackSpaces[64];
	int blackAttackSpaces[64];

	bool side_to_move;
	bool check;
	int castling_rights;
	
	int game; // checkmate, stalemate, draw
	
	void initNewBoard();
	void initPiecesList();
	

	void setPawns();
	void setRooks();
	void setKnights();
	void setBishop();
	void setQueens();
	void setKings();

public:

	idkBoard();
	int en_passant_square;
	//what is on the board?
	
	//board positioning operations

	void movePieceToAnEmptySpace(int f,int t);
	void movePieceToCapture(int,int);
	void moveKingtoCorrectKingSideCastlingSpace(bool color);
	void moveKingtoCorrectQueenSideCastlingSpace(bool color);
	void moveEnPassant(int fromSq, int toSq, int pawnCapturedSq);
	void movePromo(int fromSq, int toSq, idkUtils::piece_type PROMO);

	void reverseMovePieceToAnEmptySpace(int f, int t);
	void reverseMovePieceToCapture(int f, int t, int cap_piece);
	void reverseMoveKingtoCorrectKingSideCastlingSpace();
	void reverseMoveKingtoCorrectQueenSideCastlingSpace();
	void reverseMoveEnPassant(int fromSq, int toSq, int pawnCapturedSq);
	void reverseMovePromo(int fromSq, int toSq);
	
	void resetBoard() { initNewBoard(); }

	//board information operations 
	
	int whatIsAtThisSquare(int sq){
		if (sq > 63 || sq < 0) return -1;
		return this->boardSquares[sq];
	}

	static int getRankFromSquare(int sq) 
	{
		if (sq > 63 || sq < 0 ) return -1;
		return sq / 8;
	}
	
	static int getFileFromSquare(int sq) {
		if (sq > 63 || sq < 0) return -1; return sq % 8;}

	static int calculateSquareAbove(int sq) 
	{ 
		if (sq > 55 || sq < 0) return -1;
		return sq + 8;
	}

	static int calculateSquareBelow(int sq) {
	if (sq > 63 || sq < 8) return -1;
		return sq - 8; }

	static int calculateSquareLeft(int sq) { 
		if (sq > 63 || sq < 0) return -1;
		if (sq % 8 == 0) return -1;
		return sq - 1; }

	static int calculateSquareRight(int sq) {
		if (sq > 63 || sq < 0) return -1;
		if (sq % 8 == 0) return -1;
		return sq + 1; }

	static int calculateDiagonalSquareUpperLeft(int sq) { 
		if (sq > 63 || sq < 0) return -1;
		if (sq % 8 == 0) return -1;
		if (sq / 8 == 7) return -1;
		return sq + 7; }

	static int calculateDiagonalSquareUpperRight(int sq) { 
		if (sq > 63 || sq < 0) return -1;
		if (sq / 8 == 7) return -1;
		if (sq % 8 == 7) return -1;
		return sq + 9; }

	static int calculateDiagonalSquareBottomLeft(int sq) {
		if (sq > 63 || sq < 0) return -1;
		if (sq / 8 == 0) return -1;
		if (sq % 8 == 0) return -1;
		return sq - 9; }

	static int calculateDiagonalSquareBottomRight(int sq) { 
		if (sq > 63 || sq < 0) return -1;
		if (sq % 8 == 7) return -1;
		if (sq / 8 == 0) return -1;
		return sq - 7; }


	void prettyPrint() {
		std::string board_line;
		for (int i = 63; i >=0; i--) {
			
		}
	}

	std::string parseBoardToFen();
	void parseFenToBoard(std::string fen);

	void doSomeWorkOnBlackPiecesListByAccessingTheList(int p, int newSquare, int updatePiece) {
		int id = p >> 5;
		Black_Pieces[id].square = newSquare;
		Black_Pieces[id].piece = updatePiece;
	}

	void doSomeWorkOnWhitePiecesListByAccessingTheList(int p, int newSquare, int updatePiece) {
		int id = p >> 5;
		White_Pieces[id].square = newSquare;
		White_Pieces[id].piece = updatePiece;
	}

	idkUtils::BoardPiece getBlackBoardPiece(int index) {
		return Black_Pieces[index];
	}
	idkUtils::BoardPiece getWhiteBoardPiece(int index) {
		return White_Pieces[index];
	}

	//state operations
	void toggle_side() { side_to_move = !side_to_move;}

	bool sideToMove() { return side_to_move; }


	void calculateSquaresAttackedByBlack();
	void calculateSquaresAttackedByWhite();
};

