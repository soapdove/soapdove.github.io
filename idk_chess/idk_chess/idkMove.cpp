#include "idkMove.h"
#include <iostream>
sf::Packet& operator <<(sf::Packet& packet, const idkMove& move)
{
    return packet << move.piece << move.fromSq << move.toSq << move.color << move.capture_piece << move.flags;
}

sf::Packet& operator >>(sf::Packet& packet, idkMove& move)
{
    return packet >> move.piece >> move.fromSq >> move.toSq >> move.color >> move.capture_piece >> move.flags;
}

static int en_passant = -1;

idkMove::idkMove(){
    this->color = false;
    this->piece = -1;
    this->fromSq = -1;
    this->toSq = -1;
    this->capture_piece = -1;
    this->flags = -1;
    this->PROMO = idkUtils::NO_PIECE;
}

idkMove::idkMove(bool clr, int piece, int from, int to, int capture_piece, int flags, idkUtils::piece_type promo)
{
    this->color = clr;
    this->piece = piece;
    this->fromSq = from;
    this->toSq = to;
    this->capture_piece = capture_piece;
    this->flags = flags;
    this->PROMO = promo;
}

bool idkMove::legalPawnMove(int color, int capture_piece, int fromSq, int toSq, int& flags, idkBoard& current_board)
{
    std::cout << "this is a pawn\n" << std::endl;

    //enpassant
    //BUG:: enpassant captures same color pawn as well
    std::cout << "enpassant square: " << current_board.en_passant_square << std::endl;
    if (current_board.en_passant_square != -1 && idkBoard::calculateSquareLeft(fromSq) == current_board.en_passant_square
        || idkBoard::calculateSquareRight(fromSq) == current_board.en_passant_square) {
        bool en = false;
        std::cout << "in enpassant" << std::endl;
        std::cout << "color: " << color << std::endl;
        std::cout << "toSq: " << toSq << std::endl;

        en = color&idkUtils::WHITE ? toSq == idkBoard::calculateSquareAbove(current_board.en_passant_square)
            : toSq == idkBoard::calculateSquareBelow(current_board.en_passant_square);
        std::cout << en << std::endl;

        if (en) flags |= idkUtils::EN_PASSANT;
        return en;
    }

    //double push from start
    if (idkBoard::getRankFromSquare(fromSq) == 6 || idkBoard::getRankFromSquare(fromSq) == 1 && abs(toSq - fromSq) == 16) {
            if (current_board.whatIsAtThisSquare(toSq + (fromSq - toSq) / 2) == 0) {
                
                std::cout << "start double push";
                flags |= idkUtils::DOUBLE_PUSH;
                flags |= idkUtils::EMPTY;
                current_board.en_passant_square = toSq;
                return true;
            }
            else {
                //current_board.en_passant_square = -1;
            }

    }
    else {
         //current_board.en_passant_square = -1;
    }

    //single push
    if (!capture_piece && idkBoard::getFileFromSquare(fromSq) == idkBoard::getFileFromSquare(toSq) && (toSq - fromSq) * (color & idkUtils::WHITE ? 1 : -1) == 8) {
        std::cout << "single push";
        flags |= idkUtils::EMPTY;

        //promo
        if (toSq == 7 || toSq == 0) flags |= idkUtils::PROMO;
        return true;
    }

    //capture
    int dis = toSq - fromSq;
    bool black_cap = color&idkUtils::WHITE && (dis == 7 || dis == 9) && capture_piece;
    bool white_cap = color&idkUtils::BLACK && (dis == -7 || dis == -9) && capture_piece;
    if (black_cap || white_cap) {
        flags |= idkUtils::CAPTURE;
        
        //promo
        if (toSq == 7 || toSq == 0) flags |= idkUtils::PROMO;
       return true;
    
    }


    return false;
}

bool idkMove::legalBishopMove(int fromSq, int toSq, int& flags, idkBoard& current_board)
{
    std::cout << "this is a bishop\n";

    if (idkBoard::getFileFromSquare(fromSq) == idkBoard::getFileFromSquare(toSq) ||
        idkBoard::getRankFromSquare(fromSq) == idkBoard::getRankFromSquare(toSq))
        return false;

    int start = 0;
    int end = 0;
    if (fromSq < toSq) { start = fromSq; end = toSq; }
    else { start = toSq; end = fromSq; }

    int file_move = idkBoard::getFileFromSquare(toSq) - idkBoard::getFileFromSquare(fromSq);
    int rank_move = idkBoard::getRankFromSquare(toSq) - idkBoard::getRankFromSquare(fromSq);

    if (file_move == rank_move) {
        int check = 0;

        start += 9;
        for (; start < end; start += 9) { check += current_board.whatIsAtThisSquare(start); }
        if (check != 0) return false;

        return true;
    }

    else if (file_move == rank_move * -1) {
        int check = 0;

        start += 7;
        for (; start < end; start += 7) { check += current_board.whatIsAtThisSquare(start); }
        if (check != 0) return false;

        return true;
    }
    return false;
}

bool idkMove::legalKnightMove(int fromSq, int toSq, int& flags, idkBoard& current_board)
{
    std::cout << "this is a knight\n";

    int move_dis = abs(toSq - fromSq);
    if (move_dis == 6 || move_dis == 10 || move_dis == 15 || move_dis == 17) return true;
    return false;
}

bool idkMove::legalRookMove(int fromSq, int toSq, int& flags, idkBoard& current_board)
{
    std::cout << "this is a rook\n";

    if (idkBoard::getRankFromSquare(fromSq) != idkBoard::getRankFromSquare(toSq) &&
        idkBoard::getFileFromSquare(fromSq) != idkBoard::getFileFromSquare(toSq))
        return false;

    int start = 0;
    int end = 0;
    if (fromSq < toSq) { start = fromSq; end = toSq; }
    else { start = toSq; end = fromSq; }

    //file move
    if (idkBoard::getRankFromSquare(fromSq) == idkBoard::getRankFromSquare(toSq)) {
        int check = 0;

        start += 1;
        for (; start < end; start += 1) { check += current_board.whatIsAtThisSquare(start); }
        if (check != 0) return false;

        return true;
    }

    //rank move
    else if (idkBoard::getFileFromSquare(toSq) == idkBoard::getFileFromSquare(fromSq)) {
        int check = 0;

        start += 8;
        for (; start < end; start += 8) { check += current_board.whatIsAtThisSquare(start); }
        if (check != 0) return false;
        return true;
    }
    return false;
}

bool idkMove::legalQueenMove(int fromSq, int toSq, int& flags, idkBoard& current_board)
{
    std::cout << "this is a queen\n";
    if (legalBishopMove(fromSq, toSq, flags, current_board)) return true;
    if (legalRookMove(fromSq, toSq, flags, current_board)) return true;
    return false;
}

bool idkMove::legalKingMove(int fromSq, int toSq, int& flags, idkBoard& current_board)
{
    std::cout << "this is a king\n";
    
    int move_dis = abs(toSq - fromSq);
    if (move_dis == 8 || move_dis == 1 || move_dis == 9 || move_dis == 7) return true;

    return false;
}

bool idkMove::isKingAttacked(idkBoard& current_board,bool color)
{   
    int t_flags = 0;
    int sq = -1;
    color ? sq = current_board.getWhiteBoardPiece(15).square : sq = current_board.getBlackBoardPiece(15).square;
    int attacker = -1;
    for (int i = 0; i < 16; i++) {
        color ? attacker = current_board.getBlackBoardPiece(i).square : attacker = current_board.getWhiteBoardPiece(i).square;
        if (isLegalMove(sq, attacker, t_flags, current_board))
            return true;
   }
    return false;
}



bool idkMove::isLegalMove(int fromSq, int toSq, int& flags, idkBoard& current_board)
{
    int piece_info = current_board.whatIsAtThisSquare(fromSq);
    int capture_piece = current_board.whatIsAtThisSquare(toSq);

    int piece = piece_info & 0b111;
    int color = piece_info & 0b11000; //10000 //01000

    if (capture_piece != -1) {
        int cap_color = capture_piece & 0b11000;
        if (cap_color == color)
            return false;
    }
    bool legality = false;

    //non-sliding pieces
    if (piece == idkUtils::PAWN) {
       legality =  legalPawnMove(color, capture_piece, fromSq, toSq, flags, current_board);
    }
    if (piece == idkUtils::KNIGHT){
        if (capture_piece) { flags |= idkUtils::CAPTURE; }
        else { flags |= idkUtils::EMPTY; }
        legality = legalKnightMove(fromSq, toSq, flags, current_board);

    }
    if (piece == idkUtils::KING){
        if (capture_piece) { flags |= idkUtils::CAPTURE; }
        else { flags |= idkUtils::EMPTY; }
        legality = legalKingMove(fromSq, toSq, flags, current_board);
    }

    //sliding pieces 
    if (piece == idkUtils::BISHOP){
        if (capture_piece) { flags |= idkUtils::CAPTURE; }
        else { flags |= idkUtils::EMPTY; }
        legality = legalBishopMove(fromSq, toSq, flags, current_board);
    }

    if (piece == idkUtils::ROOK) {
        if (capture_piece) { flags |= idkUtils::CAPTURE; }
        else { flags |= idkUtils::EMPTY; }
        legality = legalRookMove(fromSq, toSq, flags, current_board);
    }
    if (piece == idkUtils::QUEEN) {
        if (capture_piece) { flags |= idkUtils::CAPTURE; }
        else { flags |= idkUtils::EMPTY; }
        legality = legalQueenMove(fromSq, toSq, flags, current_board);
    }

    return legality;
}

bool idkMove::applyMove(idkBoard& current_board)
{
    if (this->flags & idkUtils::EMPTY) {
        current_board.movePieceToAnEmptySpace(this->fromSq, this->toSq);
        return true;
    }

    else if (this->flags & (idkUtils::CAPTURE & idkUtils::PROMO)) {

    
    }

    else if (this->flags & idkUtils::CAPTURE) {
        current_board.movePieceToCapture(this->fromSq, this->toSq);
        return true;
    }

    else if (this->flags & idkUtils::PROMO) {
        current_board.movePromo(this->fromSq, this->toSq, this->PROMO);
        return true;
    }

    else if (this->flags & idkUtils::EN_PASSANT) {
        current_board.moveEnPassant(this->fromSq, this->toSq, current_board.en_passant_square);
        return true;
    }

    else if (this->flags & idkUtils::CASTLE) {

    }
    return false;
}

bool idkMove::unApplyMove(idkBoard& current_board)
{
    if (this->flags & idkUtils::EMPTY) {
        current_board.reverseMovePieceToAnEmptySpace(this->fromSq, this->toSq);
    }

    
    else if (this->flags & (idkUtils::CAPTURE & idkUtils::PROMO)) {


    }

    else if (this->flags & idkUtils::CAPTURE) {
        current_board.reverseMovePieceToCapture(this->fromSq, this->toSq, this->capture_piece);
    }

    else if (this->flags & idkUtils::PROMO) {
        current_board.reverseMovePromo(this->fromSq, this->toSq);
    }

    else if (this->flags & idkUtils::CASTLE) {
       
    }

    return false;
}
