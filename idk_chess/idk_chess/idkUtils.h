#pragma once
#include <cstdint>
class idkUtils {

public:

	enum piece_type {
		PAWN = 1, // 001
		BISHOP = 2, // 010
		KNIGHT = 3, // 011
		ROOK = 4, // 100
		QUEEN = 5, // 101
		KING = 6, // 111
		NO_PIECE = 0
		};

		enum color {
			WHITE = 8,
			BLACK = 16
		};

		enum castle_rights {
			WHITE_KING = 1,
			WHITE_QUEEN = 2,
			BLACK_KING = 4,
			BLACK_QUEEN = 8
		};

		enum move_flags {
			CASTLE = 1,
			DOUBLE_PUSH = 2,
			PROMO = 4,
			EN_PASSANT = 8,
			CAPTURE = 16,
			EMPTY = 32,
			ILLEGAL_MOVE = 0
		};

		enum game_end {
			CHECKMATE = 1,
			STALEMATE = 2,
			DRAW = 4,
		};

	struct CaptureRecord {
		int turn_number;
		int piece; 
	};

	struct BoardPiece {
		int piece; 
		int square;
	};

};