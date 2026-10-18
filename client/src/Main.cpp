/*
** EPITECH PROJECT, 2026
** RType
** File description:
** Main
*/

#include "../include/Game.hpp"
#include <iostream>

int main(void)
{
    try {
        client::Game game;
        game.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 84;
    }
    return 0;
}
