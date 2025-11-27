#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include "idkBoard.h"

class idkRenderer {

    sf::Color white_square;
    sf::Color black_square;

    bool to_render;
    
    bool render_orientation;

    int window_size_w;
    int window_size_h;

    sf::Texture white_pawn = sf::Texture("images/white_pawn.png");
    sf::Texture white_bishop = sf::Texture("images/white_bishop.png");
    sf::Texture white_knight = sf::Texture("images/white_knight.png");
    sf::Texture white_rook = sf::Texture("images/white_rook.png");
    sf::Texture white_queen = sf::Texture("images/white_queen.png");
    sf::Texture white_king = sf::Texture("images/white_king.png");

    sf::Texture black_pawn = sf::Texture("images/black_pawn.png");
    sf::Texture black_bishop = sf::Texture("images/black_bishop.png");
    sf::Texture black_knight = sf::Texture("images/black_knight.png");
    sf::Texture black_rook = sf::Texture("images/black_rook.png");
    sf::Texture black_queen = sf::Texture("images/black_queen.png");
    sf::Texture black_king = sf::Texture("images/black_king.png");

    public:
    sf::RenderWindow window;
    sf::RectangleShape board_squares[64];
    sf::RectangleShape danger_squares[64];
    sf::RectangleShape possible_moves[64];

    int selected = -1;
    int move_to = -1;
    int moused_over = -1;
    int previous_move = -1;

    void toggle_renderer() { to_render = !to_render; }

    void toggleRenderOrientation() { render_orientation = !render_orientation; }
    
    void drawBoardSquares(idkBoard& chess_board);

    void getMousedOverSquare();

    void getGetFromSquareFromClick(idkBoard& chess_board, int& f);

    void getGetToSquareFromClick(idkBoard& chess_board, int f, int& t);

    void displayWindow() {
        window.display();
    }

    idkRenderer();
    idkRenderer(int w, int h, std::string title, sf::Color xColor, sf::Color yColor, bool srt_ort);
};
