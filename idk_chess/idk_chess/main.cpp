#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include <SFML/Network.hpp>
#include <iostream>
#include "idkChess.h"

#include "idkRenderer.h"

#include "idkNetwork.h"

class idkTest {

    bool validate(int expected, int actual, std::string test_name) {

    }
};



int main()
{   

    idkChess chess_game = idkChess(false);

    idkChess::MENU_VALUE value = chess_game.display_main_menu();

    if (value == idkChess::EXIT) return 0;

    if (value == idkChess::ONLINE) {

       value = chess_game.display_online_menu();

       idkNetwork::ChessData cd;
       cd.fen = "test_fen";
       cd.ply_move = "e5";
       cd.message = "this is chess data sent by the server";

       sf::Packet chess_packet; 
       chess_packet << cd;
       idkNetwork networked_game = idkNetwork(&chess_game);
       if (value == idkChess::HOST) {
           networked_game.hostServer();
               //std::cout << "starting server..." << std::endl;

               //sf::TcpListener listener;
               //listener.listen(55001);

               //// Wait for a connection
               //sf::TcpSocket socket;
               //listener.accept(socket);
               //std::cout << "New client connected: " << socket.getRemoteAddress().value() << std::endl;
               //
               //while (true) {
           
               //// Receive a message from the client
               //std::array<char, 1024> buffer;
               //std::size_t received = 0;
               //socket.receive(buffer.data(), buffer.size(), received);
               //std::cout << "The client said: " << buffer.data() << std::endl;

               //// Send an answer
               //std::string message;
               //std::getline(std::cin, message);
               //std::cin.clear();

               //socket.send(message.c_str(), message.size() + 1);
               //socket.send(chess_packet);
           //}
       }


       if (value == idkChess::CONNECT) {
            auto host = (sf::IpAddress::resolve("127.0.0.1"));
            sf::IpAddress host_address = *host;
            networked_game.connectClient(host_address);
               //sf::Packet chess_packet;
               //idkNetwork::ChessData rcv_cd;

               //std::cout << "starting client..." << std::endl;

               //auto host = (sf::IpAddress::resolve("127.0.0.1"));
               //sf::IpAddress host_address = *host;
               //sf::TcpSocket socket;
               //socket.connect(*host, 55001);
               //
               //while (true) {
              
               //// Send a message to the connected host
               //std::string message;
               //std::getline(std::cin, message);
               //std::cin.clear();

               //socket.send(message.c_str(), message.size() + 1);

               //// Receive an answer from the server
               //std::array<char, 1024> buffer;
               //std::size_t received = 0;
               //socket.receive(buffer.data(), buffer.size(), received);
               //socket.receive(chess_packet);
               //std::cout << "The server said: " << buffer.data() << std::endl;
               //chess_packet >> rcv_cd;
               //std::cout << "received data:" << rcv_cd.fen << " " << rcv_cd.ply_move << " " << rcv_cd.message << std::endl;
           //}
       }

       return 0;
    }

    chess_game.game_loop(false);
    int i = 0;
    bool b = true;
    while (i < 10) {
        chess_game.game_loop(b);
        b = !b;
        i++;
    }
   
}

