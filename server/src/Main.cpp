/*
** EPITECH PROJECT, 2026
** RType
** file description:
** Main
*/

#include <iostream>
#include "../include/ProtoServer.hpp"

int main()
{
    try {
        asio::io_context io;
        Server server(io, 4242);
        std::cout << "serveur lancé sur le port 4242" << std::endl;
        io.run();
    } catch (const std::exception& e) {
        std::cerr << "erreur fatale : " << e.what() << std::endl;
        return 84;
    }
    return 0;
}


