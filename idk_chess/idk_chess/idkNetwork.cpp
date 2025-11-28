#include "idkNetwork.h"

sf::Packet& operator <<(sf::Packet& packet, const idkNetwork::ChessData& cd)
{
	return packet << cd.fen << cd.ply_move << cd.message;
}

sf::Packet& operator >>(sf::Packet& packet, idkNetwork::ChessData& cd)
{
	return packet >> cd.fen >> cd.ply_move >> cd.message;
}



idkNetwork::idkNetwork(idkChess * game)
{
    this->network_game = game;
}

void idkNetwork::hostServer()
{
    std::cout << "starting server..." << std::endl;

    sf::TcpListener listener;
    listener.listen(55001);

    // Wait for a connection
    sf::TcpSocket socket;
    listener.accept(socket);
    std::cout << "New client connected: " << socket.getRemoteAddress().value() << std::endl;

    //idkChess host_game = idkChess(true);
    idkMove move_to_send = idkMove();

    sf::Packet move_packet;
    while (true) {
        move_packet.clear();
        // Receive a message from the client
        //std::array<char, 1024> buffer;
        //std::size_t received = 0;
        //socket.receive(buffer.data(), buffer.size(), received);
        //std::cout << "The client said: " << buffer.data() << std::endl;

        // Send an answer
       // std::string message;
       // std::getline(std::cin, message);
       // std::cin.clear();

        move_to_send = this->network_game->networked_game_loop(true);
        move_packet << move_to_send;
        //socket.send(message.c_str(), message.size() + 1);
        socket.send(move_packet);

        move_packet.clear();

        socket.receive(move_packet);
        move_packet >> move_to_send;
        this->network_game->update_board(move_to_send);

    }
}

void idkNetwork::connectClient(sf::IpAddress hostAddress)
{
    sf::Packet chess_packet;
    sf::Packet move_packet;
    idkNetwork::ChessData rcv_cd;

    std::cout << "starting client..." << std::endl;

    //auto host = (sf::IpAddress::resolve("127.0.0.1"));
    //sf::IpAddress host_address = *host;
    sf::TcpSocket socket;
    socket.connect(hostAddress, 55001);

    //idkChess client_game = idkChess(false);

    idkMove move_to_send = idkMove();
    this->network_game->set_orientation(true);

    while (true) {
        move_packet.clear();
        // Send a message to the connected host
        //std::string message;
        //std::getline(std::cin, message);
        //std::cin.clear();

        //socket.send(message.c_str(), message.size() + 1);

        // Receive an answer from the server
        //std::array<char, 1024> buffer;
        //std::size_t received = 0;
        //socket.receive(buffer.data(), buffer.size(), received);
        socket.receive(move_packet);

        move_packet >> move_to_send;
        this->network_game->update_board(move_to_send);


        //std::cout << "The server said: " << buffer.data() << std::endl;
        move_packet.clear();

        move_to_send = this->network_game->networked_game_loop(false);
        move_packet << move_to_send;

        socket.send(move_packet);
        //chess_packet >> rcv_cd;
        //std::cout << "received data:" << rcv_cd.fen << " " << rcv_cd.ply_move << " " << rcv_cd.message << std::endl;
    }
}
