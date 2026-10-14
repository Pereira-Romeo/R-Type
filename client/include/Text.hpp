/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Text
*/

#pragma once

#include <SDL3/SDL.h>
#include <string>

namespace client
{
    // Texte avec la police de debug intégrée à SDL3 (8x8 px, multipliée par scale)
    void drawText(SDL_Renderer *renderer, const std::string &text, float x, float y, float scale);
    void drawCenteredText(SDL_Renderer *renderer, const std::string &text,
        const SDL_FRect &area, float scale);
}
