/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** ClientContext
*/

#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace client
{
    // Données partagées entre les scènes (appartient à Game)
    struct ClientContext {
        SDL_Window *window = nullptr;
        std::string playerName = "PLAYER";
        std::string roomCode;
        bool isHost = false;
    };
}
