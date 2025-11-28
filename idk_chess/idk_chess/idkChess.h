#pragma once
#include "idkBoard.h"
#include "idkRenderer.h"
#include "idkUtils.h"
#include"idkMove.h"

class idkChess
{
	idkBoard board;
	idkRenderer renderer;
	
	//starting orientation... shouldnt change
	bool player_orientation;

public:

	enum MENU_VALUE {
		EXIT = 0,
		START = 1,
		DEFAULT = 2,
		ONLINE = 3,
		HOST = 4,
		CONNECT = 5,
	};

	idkChess(bool orientation);
	
	MENU_VALUE display_main_menu();
	MENU_VALUE display_online_menu();
	MENU_VALUE display_play_again_menu();

	//true for white, false for black
	void game_loop(bool turn);

	//similar to game_loop, except it returns the move made by the player, and some other minor functionality
	idkMove networked_game_loop(bool turn);

	//needed for network functionality...
	void update_board(idkMove move);

	void set_orientation(bool ort);
};