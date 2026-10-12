/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Main
*/

#include "../include/Game.hpp"
#include <iostream>

int main()
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
