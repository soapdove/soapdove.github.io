#include "idkRenderer.h"



idkRenderer::idkRenderer(){
    to_render = false;
    window_size_h = 1420;
    window_size_w = 1420;
}

idkRenderer::idkRenderer(int w, int h, std::string title, sf::Color xColor, sf::Color yColor, bool srt_ort) {
    to_render = false;
    window = sf::RenderWindow(sf::VideoMode({ static_cast<unsigned int>(w),static_cast<unsigned int>(h) }), title);
    white_square = xColor;
    black_square = yColor;
    window_size_h = h;
    window_size_w = w;
    selected = -1;
    move_to = -1;
    this->render_orientation = srt_ort;
 
    int alt = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            alt = i * 8 + (7-j);
            sf::RectangleShape boardSquare;
            boardSquare.setSize(sf::Vector2f(w / 8, h / 8));
            ((i + j) & 1) == 0 ? boardSquare.setFillColor(xColor) : boardSquare.setFillColor(yColor);

            boardSquare.setPosition(sf::Vector2f((w / 8) * j, (h / 8) * i));
            board_squares[alt] = boardSquare;
        }
    }
}

void idkRenderer::drawBoardSquares(idkBoard& chess_board) {
    window.clear();

    int square_size = std::min(window.getSize().x, window.getSize().y);
    square_size /= 8;
    const sf::IntRect sprite_rect({ 10,10 }, { square_size,square_size });

    for (int i = 0; i < 64; i++) {

        int fixed_index = render_orientation ? i : 63-i;
        int s_i = i / 8;
        int s_j = i % 8;
        ((s_i + s_j) & 1) == 0 ? board_squares[fixed_index].setFillColor(white_square) : board_squares[fixed_index].setFillColor(black_square);
        int piece = chess_board.whatIsAtThisSquare(fixed_index);
        int color = ((0b11000 & piece) & 0b10000) * 252;
        piece &= 0b111;
        board_squares[fixed_index].setOutlineThickness(0);
        board_squares[fixed_index].setTexture(0);

        // render according to window


        board_squares[fixed_index].setSize(sf::Vector2f(window.getSize().x / 8, window.getSize().y / 8));

        board_squares[fixed_index].setPosition(sf::Vector2f((window.getSize().x / 8) * (7 - s_j), (window.getSize().y / 8) * s_i));
        //

        if (moused_over == fixed_index) {
            board_squares[fixed_index].setOutlineColor(sf::Color::Blue);
            board_squares[fixed_index].setOutlineThickness(-7.0f);
        }

        if (selected == fixed_index) {
            //b_squares[i].setFillColor(sf::Color::Green);
            board_squares[fixed_index].setOutlineColor(sf::Color::Green);
            board_squares[fixed_index].setOutlineThickness(-7.0f);

        }
        else if (move_to == fixed_index) {
            //b_squares[i].setFillColor(sf::Color::Red);
            board_squares[fixed_index].setOutlineColor(sf::Color::Red);
            board_squares[fixed_index].setOutlineThickness(-7.0f);

        }
        if (piece == idkUtils::PAWN) {
            board_squares[fixed_index].setFillColor(sf::Color(abs(60 - color), abs(22 - color), abs(252 - color)));
            ((0b11000 & chess_board.whatIsAtThisSquare(fixed_index)) & 0b01000) ? board_squares[fixed_index].setTexture(&white_pawn, true) : board_squares[fixed_index].setTexture(&black_pawn, true);
            board_squares[fixed_index].setTextureRect(sprite_rect);
        }
        else if (piece == idkUtils::BISHOP) {
            board_squares[fixed_index].setFillColor(sf::Color(abs(67 - color), abs(200 - color), abs(25 - color)));
            ((0b11000 & chess_board.whatIsAtThisSquare(fixed_index)) & 0b01000) ? board_squares[fixed_index].setTexture(&white_bishop, true) : board_squares[fixed_index].setTexture(&black_bishop, true);
            board_squares[fixed_index].setTextureRect(sprite_rect);
        }
        else if (piece == idkUtils::KNIGHT) {
            board_squares[fixed_index].setFillColor(sf::Color(abs(49 - color), abs(23 - color), abs(100 - color)));
            ((0b11000 & chess_board.whatIsAtThisSquare(fixed_index)) & 0b01000) ? board_squares[fixed_index].setTexture(&white_knight) : board_squares[fixed_index].setTexture(&black_knight, true);
            board_squares[fixed_index].setTextureRect(sprite_rect);
        }
        else if (piece == idkUtils::ROOK) {
            board_squares[fixed_index].setFillColor(sf::Color(abs(50 - color), abs(200 - color), abs(90 - color)));
            ((0b11000 & chess_board.whatIsAtThisSquare(fixed_index)) & 0b01000) ? board_squares[fixed_index].setTexture(&white_rook) : board_squares[fixed_index].setTexture(&black_rook, true);
            board_squares[fixed_index].setTextureRect(sprite_rect);
        }
        else if (piece == idkUtils::QUEEN) {
            board_squares[fixed_index].setFillColor(sf::Color(abs(10 - color), abs(100 - color), abs(87 - color)));
            ((0b11000 & chess_board.whatIsAtThisSquare(fixed_index)) & 0b01000) ? board_squares[fixed_index].setTexture(&white_queen) : board_squares[fixed_index].setTexture(&black_queen, true);
            board_squares[fixed_index].setTextureRect(sprite_rect);
        }
        else if (piece == idkUtils::KING) {
            board_squares[fixed_index].setFillColor(sf::Color(abs(200 - color), abs(2 - color), abs(100 - color)));
            ((0b11000 & chess_board.whatIsAtThisSquare(fixed_index)) & 0b01000) ? board_squares[fixed_index].setTexture(&white_king) : board_squares[fixed_index].setTexture(&black_king, true);
            board_squares[fixed_index].setTextureRect(sprite_rect);
        }

        window.draw(board_squares[fixed_index]);
    }
    //window.display();
}

void idkRenderer::getMousedOverSquare() {

    int mousePosX = sf::Mouse::getPosition(window).x;
    int mousePosY = sf::Mouse::getPosition(window).y;
    //std::cout << mousePosX;
    //std::cout << "\n";
    //std::cout << mousePosY;
    //std::cout << "\n\n";

    int fixed_edge_x = window.getSize().x / 8;
    fixed_edge_x *= 8;

    int fixed_edge_y = window.getSize().y / 8;
    fixed_edge_y *= 8;

    if (mousePosX > fixed_edge_x - 1 || mousePosY > fixed_edge_y - 1 || mousePosX < 0 || mousePosY < 0) return;

    int mouseToGridI = mousePosX / (window.getSize().x / 8);
    int mouseToGridJ = mousePosY / (window.getSize().y / 8);

    int squareIndex = mouseToGridJ * 8 + (7 - mouseToGridI);
    if (squareIndex > -1 && squareIndex < 64) moused_over = render_orientation ? squareIndex: 63 - squareIndex;
    std::cout << squareIndex;
    std::cout << "\n";
}

void idkRenderer::getGetFromSquareFromClick(idkBoard& chess_board, int& f) {

    int mousePosX = sf::Mouse::getPosition(window).x;
    int mousePosY = sf::Mouse::getPosition(window).y;

    int mouseToGridI = mousePosX / (window.getSize().x / 8);
    int mouseToGridJ = mousePosY / (window.getSize().y / 8);

    int squareIndex = mouseToGridJ * 8 + (7 - mouseToGridI);
    if (squareIndex > -1 && squareIndex < 64 && chess_board.whatIsAtThisSquare(render_orientation ? squareIndex : 63 - squareIndex) != 0) {
        selected = render_orientation ? squareIndex : 63 - squareIndex;
        // board_squares[squareIndex].setFillColor(sf::Color::Green);
        f = render_orientation ? squareIndex : 63 - squareIndex;
        //window.draw(board_squares[squareIndex]);
        //move_to = -1;
    }
}

void idkRenderer::getGetToSquareFromClick(idkBoard& chess_board, int f, int& t) {

    int mousePosX = sf::Mouse::getPosition(window).x;
    int mousePosY = sf::Mouse::getPosition(window).y;

    int mouseToGridI = mousePosX / (window.getSize().x / 8);
    int mouseToGridJ = mousePosY / (window.getSize().y / 8);

    int squareIndex = mouseToGridJ * 8 + (7 - mouseToGridI);
    if (squareIndex > -1 && squareIndex < 64 && render_orientation ? squareIndex : 63 - squareIndex != f) {
        move_to = render_orientation ? squareIndex : 63 - squareIndex;
        //board_squares[squareIndex].setFillColor(sf::Color::Red);
        t = render_orientation ? squareIndex : 63 - squareIndex;
        //move the piece on the actual ChessBoard
        //chess_board.movePieceToAnEmptySpace(f, t);
        //window.draw(board_squares[squareIndex]);
        //selected = -1;
    }
}