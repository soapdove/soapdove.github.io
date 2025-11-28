#include "idkChess.h"

idkChess::idkChess(bool orientation)  {
	renderer = idkRenderer(1420, 1420, std::string("idkCHESS"), sf::Color(123, 34, 13), sf::Color(252 - 123, 252 - 34, 252 - 13), orientation);
	board = idkBoard();
	this->player_orientation = orientation;
}


idkChess::MENU_VALUE idkChess::display_main_menu()
{
	const float x_pos_ratio = 510.0f / 1420.0f;
	const float y_pos_ratio_play = 910.0f / 1420.0f;
	const float y_pos_ratio_exit = 1110.0f / 1420.0f;

	double x_velocity = 0.5;
	double y_velocity = 0.5;
	int acceleration = 0;

	float x_ratio = (400.0f / 1420.0f);
	float y_ratio = 100.0f / 1420.0f;

	sf::RectangleShape title_obj;
	sf::RectangleShape play_button;
	sf::RectangleShape exit_button;
	sf::RectangleShape online_button;

	title_obj.setSize(sf::Vector2f(700, 200));
	play_button.setSize(sf::Vector2f(400, 100));
	exit_button.setSize(sf::Vector2f(400, 100));
	online_button.setSize(sf::Vector2f(400, 100));

	title_obj.setFillColor(sf::Color::Magenta);
	play_button.setFillColor(sf::Color::Blue);
	exit_button.setFillColor(sf::Color::Red);
	online_button.setFillColor(sf::Color::Yellow);

	title_obj.setPosition(sf::Vector2f(310, 110));
	play_button.setPosition(sf::Vector2f(510, 910));
	online_button.setPosition(sf::Vector2f(510, 1110));
	exit_button.setPosition(sf::Vector2f(510, 1210));
	

	play_button.setOutlineColor(sf::Color::Cyan);
	exit_button.setOutlineColor(sf::Color::Cyan);
	online_button.setOutlineColor(sf::Color::Cyan);


	float win_x = 0;
	float win_y = 0;

	int mousePosX = 0;
	int mousePosY = 0;

	bool title_clicked = false;

	while (renderer.window.isOpen())
	{
		win_x = renderer.window.getSize().x;
		win_y = renderer.window.getSize().y;

		while (const std::optional event = renderer.window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				renderer.window.close();

			if (const auto* resized = event->getIf<sf::Event::Resized>()) {

				sf::FloatRect visibleArea({ 0.f, 0.f }, sf::Vector2f(resized->size));
				renderer.window.setView(sf::View(visibleArea));
				title_obj.setPosition(sf::Vector2f(0.5 * win_x - 0.5 * title_obj.getSize().x, 0.3 * win_y - 0.5 * title_obj.getSize().y));
				title_clicked = false;
			}
		}

		play_button.setSize(sf::Vector2f(win_x * x_ratio, win_y * y_ratio));
		exit_button.setSize(sf::Vector2f(win_x * x_ratio, win_y * y_ratio));
		online_button.setSize(sf::Vector2f(win_x * x_ratio, win_y * y_ratio));
		title_obj.setSize(sf::Vector2f(win_x * 0.49f, win_y * 0.14f));


		play_button.setPosition(sf::Vector2f(0.5 * win_x - 0.5 * play_button.getSize().x, 0.6 * win_y - 0.5 * play_button.getSize().y));
		exit_button.setPosition(sf::Vector2f(0.5 * win_x - 0.5 * exit_button.getSize().x, 0.9 * win_y - 0.5 * exit_button.getSize().y));
		online_button.setPosition(sf::Vector2f(0.5 * win_x - 0.5 * online_button.getSize().x, 0.75 * win_y - 0.5 * online_button.getSize().y));

		play_button.setOutlineThickness(0);
		exit_button.setOutlineThickness(0);
		online_button.setOutlineThickness(0);

		mousePosX = sf::Mouse::getPosition(renderer.window).x;
		mousePosY = sf::Mouse::getPosition(renderer.window).y;

		if (mousePosX >= title_obj.getPosition().x && mousePosX < title_obj.getPosition().x + title_obj.getSize().x
			&& mousePosY >= title_obj.getPosition().y && mousePosY < title_obj.getPosition().y + title_obj.getSize().y 
			&& sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

			if (title_clicked) {
				title_obj.setPosition(sf::Vector2f(0.5 * win_x - 0.5 * title_obj.getSize().x, 0.3 * win_y - 0.5 * title_obj.getSize().y));
				title_clicked = false;
			}
			else{title_clicked = true;}
		}

		
		if (title_clicked) {
		title_obj.setPosition(sf::Vector2f(title_obj.getPosition().x + x_velocity, title_obj.getPosition().y + y_velocity));

		if (title_obj.getPosition().x > win_x - title_obj.getSize().x || title_obj.getPosition().x < 0) {
			x_velocity = x_velocity * -1;
			title_obj.setFillColor(sf::Color(abs(title_obj.getFillColor().r - 252), abs(title_obj.getFillColor().g - 252), abs(title_obj.getFillColor().b - 252)));
		}

		if (title_obj.getPosition().y > win_y - title_obj.getSize().y || title_obj.getPosition().y < 0) {
			y_velocity = y_velocity * -1;
			title_obj.setFillColor(sf::Color(abs(title_obj.getFillColor().r - 252), abs(title_obj.getFillColor().g - 252), abs(title_obj.getFillColor().b - 252)));
		}

		}

		else {
			title_obj.setPosition(sf::Vector2f(0.5 * win_x - 0.5 * title_obj.getSize().x, 0.3 * win_y - 0.5 * title_obj.getSize().y));
		}

		if (mousePosX >= play_button.getPosition().x && mousePosX < play_button.getPosition().x + play_button.getSize().x
			&& mousePosY >= play_button.getPosition().y && mousePosY < play_button.getPosition().y + play_button.getSize().y) {
			play_button.setOutlineThickness(-7.0f);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) return START;
		}

		if (mousePosX >= exit_button.getPosition().x && mousePosX <= exit_button.getPosition().x + exit_button.getSize().x
			&& mousePosY >= exit_button.getPosition().y && mousePosY <= exit_button.getPosition().y + exit_button.getSize().y) {
			exit_button.setOutlineThickness(-7.0f);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) return EXIT;
		}

		if (mousePosX >= online_button.getPosition().x && mousePosX < online_button.getPosition().x + online_button.getSize().x
			&& mousePosY >= online_button.getPosition().y && mousePosY < online_button.getPosition().y + online_button.getSize().y) {
			online_button.setOutlineThickness(-7.0f);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) return ONLINE;
		}
		renderer.window.clear(sf::Color::White);

		renderer.window.draw(title_obj);
		renderer.window.draw(play_button);
		renderer.window.draw(exit_button);
		renderer.window.draw(online_button);

		renderer.window.display();

	}
	return DEFAULT;
}

idkChess::MENU_VALUE idkChess::display_online_menu()
{
	int win_x = 0;
	int win_y = 0;

	const float x_ratio = (400.0f / 1420.0f);
	const float y_ratio = 100.0f / 1420.0f;

	int mousePosX = 0;
	int mousePosY = 0;

	sf::RectangleShape host_button;
	sf::RectangleShape connect_button;

	host_button.setSize(sf::Vector2f(400, 100));
	connect_button.setSize(sf::Vector2f(400, 100));

	host_button.setFillColor(sf::Color::Cyan);
	connect_button.setFillColor(sf::Color::Green);

	host_button.setPosition(sf::Vector2f(500, 510));
	connect_button.setPosition(sf::Vector2f(500, 710));

	host_button.setOutlineColor(sf::Color::Black);
	connect_button.setOutlineColor(sf::Color::Black);

	while (renderer.window.isOpen())
	{
		win_x = renderer.window.getSize().x;
		win_y = renderer.window.getSize().y;

		while (const std::optional event = renderer.window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				renderer.window.close();

			if (const auto* resized = event->getIf<sf::Event::Resized>()) {

				sf::FloatRect visibleArea({ 0.f, 0.f }, sf::Vector2f(resized->size));
				renderer.window.setView(sf::View(visibleArea));
			}
		}
		
		host_button.setSize(sf::Vector2f(win_x * x_ratio, win_y * y_ratio));
		connect_button.setSize(sf::Vector2f(win_x * x_ratio, win_y * y_ratio));

		host_button.setPosition(sf::Vector2f(0.5 * win_x - 0.5 * host_button.getSize().x, 0.3 * win_y - 0.5 * host_button.getSize().y));
		connect_button.setPosition(sf::Vector2f(0.5 * win_x - 0.5 * connect_button.getSize().x, 0.45 * win_y - 0.5 * connect_button.getSize().y));

		host_button.setOutlineThickness(0);
		connect_button.setOutlineThickness(0);

		mousePosX = sf::Mouse::getPosition(renderer.window).x;
		mousePosY = sf::Mouse::getPosition(renderer.window).y;

		if (mousePosX >= host_button.getPosition().x && mousePosX < host_button.getPosition().x + host_button.getSize().x
			&& mousePosY >= host_button.getPosition().y && mousePosY < host_button.getPosition().y + host_button.getSize().y) {
			host_button.setOutlineThickness(-7.0f);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) return HOST;
		}

		if (mousePosX >= connect_button.getPosition().x && mousePosX <= connect_button.getPosition().x + connect_button.getSize().x
			&& mousePosY >= connect_button.getPosition().y && mousePosY <= connect_button.getPosition().y + connect_button.getSize().y) {
			connect_button.setOutlineThickness(-7.0f);
			if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) return CONNECT;
		}

		renderer.window.clear(sf::Color::White);

		renderer.window.draw(host_button);
		renderer.window.draw(connect_button);

		renderer.window.display();
	}

	return DEFAULT;
}


void idkChess::game_loop(bool turn) {
	//wait for clicks
	int move_completed = 0;
	int f = -1;
	int t = -1;

	sf::View view1(sf::FloatRect({ 200.f, 200.f }, { 300.f, 200.f }));
	while (renderer.window.isOpen())
	{
		
		while (const std::optional event = renderer.window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				renderer.window.close();

			if (const auto* resized = event->getIf<sf::Event::Resized>()) {
				
				sf::FloatRect visibleArea({ 0.f, 0.f }, sf::Vector2f(resized->size));
				renderer.window.setView(sf::View(visibleArea));
			}
		}

		renderer.getMousedOverSquare();

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) /* && renderer.selected == -1 */) {
			renderer.getGetFromSquareFromClick(board, f);
			renderer.selected = f;
			renderer.move_to = -1;
			t = -1;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && renderer.selected != -1) {
			renderer.getGetToSquareFromClick(board, f, t);
			renderer.move_to = t;
			renderer.selected = -1;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)) {
			renderer.toggleRenderOrientation();
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
			board.resetBoard();

		}

		if (f != -1) {
			//check if piece
			if (board.whatIsAtThisSquare(f)) {
				if (turn && (board.whatIsAtThisSquare(f) & 0b11000) & idkUtils::BLACK) { f = -1; t = -1; }
				
				if (!turn && (board.whatIsAtThisSquare(f) & 0b11000) & idkUtils::WHITE) { f = -1; t = -1; }
			}
		}

		renderer.drawBoardSquares(board);
		renderer.displayWindow();

		//end the wait functions
		//do move calculations
		int move_flags = 0;


		if (f != -1 && t != -1 && idkMove::isLegalMove(f, t, move_flags, board)) {
			idkMove move = idkMove(true, board.whatIsAtThisSquare(f), f, t, board.whatIsAtThisSquare(t), move_flags, idkUtils::NO_PIECE);
			move.applyMove(board);

			//king is now in check
			if (true && idkMove::isKingAttacked(board,turn)) {
				//unapply move
				move.unApplyMove(board);
				//do something to restart move checking
			}

			else {
				f = -1;
				t = -1;
				move_completed = 1;
				//board.en_passant_square = -1;
				if (move_flags & idkUtils::EN_PASSANT) { board.en_passant_square = -1; }
			}
		}
		//call appropiate board functions
		//render board changes
		renderer.drawBoardSquares(board);
		renderer.displayWindow();

		if (move_completed & 1) return;
	}
}

idkMove idkChess::networked_game_loop(bool turn)
{
	//wait for clicks
	int move_completed = 0;
	int f = -1;
	int t = -1;

	sf::View view1(sf::FloatRect({ 200.f, 200.f }, { 300.f, 200.f }));
	while (renderer.window.isOpen())
	{

		while (const std::optional event = renderer.window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				renderer.window.close();

			if (const auto* resized = event->getIf<sf::Event::Resized>()) {

				sf::FloatRect visibleArea({ 0.f, 0.f }, sf::Vector2f(resized->size));
				renderer.window.setView(sf::View(visibleArea));
			}
		}

		renderer.getMousedOverSquare();

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) /* && renderer.selected == -1 */) {
			renderer.getGetFromSquareFromClick(board, f);
			renderer.selected = f;
			renderer.move_to = -1;
			t = -1;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && renderer.selected != -1) {
			renderer.getGetToSquareFromClick(board, f, t);
			renderer.move_to = t;
			renderer.selected = -1;
		}

		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)) {
			renderer.toggleRenderOrientation();
		}

		if (f != -1) {
			//check if piece
			if (board.whatIsAtThisSquare(f)) {
				if (turn && (board.whatIsAtThisSquare(f) & 0b11000) & idkUtils::BLACK) { f = -1; t = -1; }

				if (!turn && (board.whatIsAtThisSquare(f) & 0b11000) & idkUtils::WHITE) { f = -1; t = -1; }
			}
		}

		renderer.drawBoardSquares(board);
		renderer.displayWindow();

		//end the wait functions
		//do move calculations
		int move_flags = 0;

		idkMove move;
		if (f != -1 && t != -1 && idkMove::isLegalMove(f, t, move_flags, board)) {
			move = idkMove(true, board.whatIsAtThisSquare(f), f, t, board.whatIsAtThisSquare(t), move_flags, idkUtils::NO_PIECE);
			move.applyMove(board);

			//king is now in check
			if (false && idkMove::isKingAttacked(board, turn)) {
				//unapply move
				move.unApplyMove(board);
				//do something to restart move checking
			}

			else {}
			f = -1;
			t = -1;
			move_completed = 1;
			//board.en_passant_square = -1;
			if (move_flags & idkUtils::EN_PASSANT) { board.en_passant_square = -1; }
		}
		//call appropiate board functions
		//render board changes
		renderer.drawBoardSquares(board);
		renderer.displayWindow();

		if (move_completed & 1) return move;
	}
}

void idkChess::update_board(idkMove move)
{
	move.applyMove(board);
}

void idkChess::set_orientation(bool ort)
{
	this->player_orientation = ort;
}
