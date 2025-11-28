#include "idkBoard.h"

idkBoard::idkBoard()
{
	initNewBoard();
	initPiecesList();
}

void idkBoard::initNewBoard()
{
	for (int i = 0; i < 64; i++) { boardSquares[i] = 0; }

	setPawns();
	setBishop();
	setKnights();
	setRooks();
	setQueens();
	setKings();

	castling_rights = idkUtils::WHITE_KING | idkUtils::WHITE_QUEEN | idkUtils::BLACK_KING | idkUtils::BLACK_QUEEN;
	side_to_move = true;
	check = false;
	en_passant_square = -1;
}

void idkBoard::initPiecesList()
{
	//White Pawns
	White_Pieces[0].piece = idkUtils::PAWN | idkUtils::WHITE;
	White_Pieces[0].square = 8;
	White_Pieces[1].piece = idkUtils::PAWN | idkUtils::WHITE | (1 << 5);
	White_Pieces[1].square = 9;
	White_Pieces[2].piece = idkUtils::PAWN | idkUtils::WHITE | (2 << 5);
	White_Pieces[2].square = 10;
	White_Pieces[3].piece = idkUtils::PAWN | idkUtils::WHITE | (3 << 5);
	White_Pieces[3].square = 11;
	White_Pieces[4].piece = idkUtils::PAWN | idkUtils::WHITE | (4 << 5);
	White_Pieces[4].square = 12;
	White_Pieces[5].piece = idkUtils::PAWN | idkUtils::WHITE | (5 << 5);
	White_Pieces[5].square = 13;
	White_Pieces[6].piece = idkUtils::PAWN | idkUtils::WHITE | (6 << 5);
	White_Pieces[6].square = 14;
	White_Pieces[7].piece = idkUtils::PAWN | idkUtils::WHITE | (7 << 5);
	White_Pieces[7].square = 15;

	//Black Pawns
	Black_Pieces[0].piece = idkUtils::PAWN | idkUtils::BLACK;
	Black_Pieces[0].square = 48;
	Black_Pieces[1].piece = idkUtils::PAWN | idkUtils::BLACK | (1 << 5);
	Black_Pieces[1].square = 49;
	Black_Pieces[2].piece = idkUtils::PAWN | idkUtils::BLACK | (2 << 5);
	Black_Pieces[2].square = 50;
	Black_Pieces[3].piece = idkUtils::PAWN | idkUtils::BLACK | (3 << 5);
	Black_Pieces[3].square = 51;
	Black_Pieces[4].piece = idkUtils::PAWN | idkUtils::BLACK | (4 << 5);
	Black_Pieces[4].square = 52;
	Black_Pieces[5].piece = idkUtils::PAWN | idkUtils::BLACK | (5 << 5);
	Black_Pieces[5].square = 53;
	Black_Pieces[6].piece = idkUtils::PAWN | idkUtils::BLACK | (6 << 5);
	Black_Pieces[6].square = 54;
	Black_Pieces[7].piece = idkUtils::PAWN | idkUtils::BLACK | (7 << 5);
	Black_Pieces[7].square = 55;

	//White Bishop
	White_Pieces[8].piece = idkUtils::BISHOP | idkUtils::WHITE | (8 << 5);
	White_Pieces[8].square = 2;
	White_Pieces[9].piece = idkUtils::BISHOP | idkUtils::WHITE | (9 << 5);
	White_Pieces[9].square = 5;

	//Black Bishop
	Black_Pieces[8].piece = idkUtils::BISHOP | idkUtils::BLACK | (8 << 5);
	Black_Pieces[8].square = 58;
	Black_Pieces[9].piece = idkUtils::BISHOP | idkUtils::BLACK | (9 << 5);
	Black_Pieces[9].square = 61;
	
	//White Night
	White_Pieces[10].piece = idkUtils::KNIGHT | idkUtils::WHITE | (10 << 5);
	White_Pieces[10].square = 1;
	White_Pieces[11].piece = idkUtils::KNIGHT | idkUtils::WHITE | (11 << 5);
	White_Pieces[11].square = 6;

	//Black Night
	Black_Pieces[10].piece = idkUtils::KNIGHT | idkUtils::BLACK | (10 << 5);
	Black_Pieces[10].square = 57;
	Black_Pieces[11].piece = idkUtils::KNIGHT | idkUtils::BLACK | (11 << 5);
	Black_Pieces[11].square = 62;


	//White Rook
	White_Pieces[12].piece = idkUtils::ROOK | idkUtils::WHITE | (12 << 5);
	White_Pieces[12].square = 0;
	White_Pieces[13].piece = idkUtils::ROOK | idkUtils::WHITE | (13 << 5);
	White_Pieces[13].square = 7;

	//Black Rook
	Black_Pieces[12].piece = idkUtils::ROOK | idkUtils::BLACK | (12 << 5);
	Black_Pieces[12].square = 56;
	Black_Pieces[13].piece = idkUtils::ROOK | idkUtils::BLACK | (13 << 5);
	Black_Pieces[13].square = 63;

	//White Queen
	White_Pieces[14].piece = idkUtils::QUEEN | idkUtils::WHITE | (14 << 5);
	White_Pieces[14].square = 3;

	//Black Queen 
	Black_Pieces[14].piece = idkUtils::QUEEN | idkUtils::BLACK | (14 << 5);
	Black_Pieces[14].square = 59;

	//White King
	White_Pieces[15].piece = idkUtils::KING | idkUtils::WHITE | (15 << 5);
	White_Pieces[15].square = 4;

	//Black King
	Black_Pieces[15].piece = idkUtils::KING | idkUtils::BLACK | (15 << 5);
	Black_Pieces[15].square = 60;
}


void idkBoard::setPawns()
{

	for (int i = 8; i < 16; i++) {
		boardSquares[i] = idkUtils::PAWN | idkUtils::WHITE | abs(8 - i) << 5;
	}
	for (int i = 48; i < 56; i++) {
		boardSquares[i] = idkUtils::PAWN | idkUtils::BLACK | abs(48 - i) << 5;
	}
}

void idkBoard::setRooks()
{
		boardSquares[0] = idkUtils::ROOK | idkUtils::WHITE | (12 << 5);
		boardSquares[7] = idkUtils::ROOK | idkUtils::WHITE | (13 << 5);
		boardSquares[56] = idkUtils::ROOK | idkUtils::BLACK | (12 << 5);
		boardSquares[63] = idkUtils::ROOK | idkUtils::BLACK | (13 << 5);
}

void idkBoard::setBishop()
{
	boardSquares[2] = idkUtils::BISHOP | idkUtils::WHITE | (8 << 5);
	boardSquares[5] = idkUtils::BISHOP | idkUtils::WHITE | (9 << 5);
	boardSquares[58] = idkUtils::BISHOP | idkUtils::BLACK | (8 << 5);
	boardSquares[61] = idkUtils::BISHOP | idkUtils::BLACK | (9 << 5);

}

void idkBoard::setKnights()
{
	boardSquares[1] = idkUtils::KNIGHT | idkUtils::WHITE | (10 << 5);
	boardSquares[6] = idkUtils::KNIGHT | idkUtils::WHITE | (11 << 5);
	boardSquares[57] = idkUtils::KNIGHT | idkUtils::BLACK | (10 << 5);
	boardSquares[62] = idkUtils::KNIGHT | idkUtils::BLACK | (11 << 5);
}

void idkBoard::setQueens()
{
	boardSquares[3] = idkUtils::QUEEN | idkUtils::WHITE | (14 << 5);
	boardSquares[59] = idkUtils::QUEEN | idkUtils::BLACK | (14 << 5);

}

void idkBoard::setKings()
{
	boardSquares[4] = idkUtils::KING | idkUtils::WHITE | (15 << 5);
	boardSquares[60] = idkUtils::KING | idkUtils::BLACK | (15 << 5);
}

void idkBoard::movePieceToAnEmptySpace(int fromSq, int toSq)
{
	// [0000] - [00] - [000] 
	// id      color   piece 
	int fromPiece = boardSquares[fromSq];
	int color = fromPiece >> 3;

	//if (color & 1) { doSomeWorkOnWhitePiecesListByAccessingTheList(fromPiece, toSq, fromPiece); }
	//else if (color & 2) { doSomeWorkOnBlackPiecesListByAccessingTheList(fromPiece, toSq, fromPiece);}

	color & 1 ? doSomeWorkOnWhitePiecesListByAccessingTheList(fromPiece, toSq, fromPiece) : 
		doSomeWorkOnBlackPiecesListByAccessingTheList(fromPiece, toSq, fromPiece);
	
	boardSquares[toSq] = fromPiece;
	boardSquares[fromSq] = 0;
	
}

void idkBoard::movePieceToCapture(int fromSq, int toSq)
{
	//check the color of the piece on the toSq
	int capture_piece = boardSquares[toSq];
	int color = capture_piece >> 3;

	//can use the doSomeWork... functions to set the capture piece sq to -1 or -2 to indicate it has been captured
	color & 1 ? doSomeWorkOnWhitePiecesListByAccessingTheList(capture_piece, -1, capture_piece):
	  doSomeWorkOnBlackPiecesListByAccessingTheList(capture_piece, -1, capture_piece);
	//then call movePieceToAnEmptySpace(fromSq, toSq);
	movePieceToAnEmptySpace(fromSq, toSq);

}

void idkBoard::moveKingtoCorrectKingSideCastlingSpace(bool color)
{
	int kingPiece, rookPiece = -1;

	color ? kingPiece = boardSquares[4] : kingPiece = boardSquares[60];
	color ? rookPiece = boardSquares[7] : rookPiece = boardSquares[63];

	color ? boardSquares[6] = kingPiece : boardSquares[62] = kingPiece;
	color ? boardSquares[5] = rookPiece : boardSquares[61] = rookPiece;

	color ? boardSquares[4] = 0 : boardSquares[60] = 0;
	color ? boardSquares[7] = 0 : boardSquares[63] = 0;

	color ?  White_Pieces[15].square = 6 : Black_Pieces[15].square = 62;
	color ?  White_Pieces[7].square = 5 : Black_Pieces[7].square = 61;

}

void idkBoard::moveKingtoCorrectQueenSideCastlingSpace(bool color)
{
	int kingPiece, rookPiece = -1;


	if (color) {
		kingPiece = boardSquares[4];
		rookPiece = boardSquares[0];
		boardSquares[2] = kingPiece;
		boardSquares[3] = rookPiece;
		boardSquares[4] = 0;
		boardSquares[0] = 0;
		White_Pieces[15].square = 2;
		White_Pieces[7].square = 3;

	}

	else {
		kingPiece = boardSquares[60];
		rookPiece = boardSquares[56];
		boardSquares[58] = kingPiece;
		boardSquares[59] = rookPiece;
		boardSquares[60] = 0;
		boardSquares[56] = 0;
		Black_Pieces[15].square = 58;
		Black_Pieces[7].square = 59;
	}
}

void idkBoard::moveEnPassant(int fromSq, int toSq, int pawnCapturedSq)
{
	int cur_piece = boardSquares[fromSq];
	int cap_piece = boardSquares[pawnCapturedSq];

	int cur_color = cur_piece >> 3; 
	int cap_color = cap_piece >> 3;
	const int mask = 0b111111000;

	cur_color & 1 ? doSomeWorkOnWhitePiecesListByAccessingTheList(cur_piece, toSq, cur_piece) :
		doSomeWorkOnBlackPiecesListByAccessingTheList(cur_piece, toSq, cur_piece);

	cap_color & 1 ? doSomeWorkOnWhitePiecesListByAccessingTheList(cap_piece, -1, cap_piece) :
		doSomeWorkOnBlackPiecesListByAccessingTheList(cap_piece, -1, cap_piece);

	boardSquares[toSq] = cur_piece;
	boardSquares[fromSq] = 0;
	boardSquares[pawnCapturedSq] = 0;
}

void idkBoard::movePromo(int fromSq, int toSq, idkUtils::piece_type PROMO)
{
	int cur_piece = boardSquares[fromSq];
	int color = cur_piece >> 3;
	color &= 3;
	const int mask = 0b111111000;
	//can use the doSomeWork... functions to set the capture piece sq to -1 or -2 to indicate it has been captured
	if (color & 1) { doSomeWorkOnWhitePiecesListByAccessingTheList(cur_piece, -1, (mask & cur_piece) | PROMO); }
	else if (color & 2) { doSomeWorkOnBlackPiecesListByAccessingTheList(cur_piece, -1, (mask & cur_piece) | PROMO); }

	boardSquares[toSq] = (mask & cur_piece) | PROMO;
}

void idkBoard::reverseMovePieceToAnEmptySpace(int f, int t)
{
	int fromPiece = boardSquares[f];
	int color = fromPiece >> 3;

	//if (color & 1) { doSomeWorkOnWhitePiecesListByAccessingTheList(fromPiece, toSq, fromPiece); }
	//else if (color & 2) { doSomeWorkOnBlackPiecesListByAccessingTheList(fromPiece, toSq, fromPiece);}

	color & 1 ? doSomeWorkOnWhitePiecesListByAccessingTheList(fromPiece, t, fromPiece) :
		doSomeWorkOnBlackPiecesListByAccessingTheList(fromPiece, t, fromPiece);

	boardSquares[t] = 0;
	boardSquares[f] = fromPiece;
}

void idkBoard::reverseMovePieceToCapture(int f, int t, int cap_piece)
{
}

void idkBoard::reverseMoveKingtoCorrectKingSideCastlingSpace()
{
}

void idkBoard::reverseMoveKingtoCorrectQueenSideCastlingSpace()
{
}

void idkBoard::reverseMoveEnPassant(int fromSq, int toSq, int pawnCapturedSq)
{
}

void idkBoard::reverseMovePromo(int fromSq, int toSq)
{
}

void idkBoard::calculateSquaresAttackedByBlack()
{

}

void idkBoard::calculateSquaresAttackedByWhite()
{
}
