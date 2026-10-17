/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Text
*/

#include "../../include/Text.hpp"

namespace client
{
    void drawText(SDL_Renderer *renderer, const std::string &text, float x, float y, float scale)
    {
        // SetRenderScale multiplie aussi les coordonnées, donc on divise par scale
        SDL_SetRenderScale(renderer, scale, scale);
        SDL_RenderDebugText(renderer, x / scale, y / scale, text.c_str());
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);
    }

    void drawCenteredText(SDL_Renderer *renderer, const std::string &text,
        const SDL_FRect &area, float scale)
    {
        const float charSize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * scale;
        float x = area.x + (area.w - text.size() * charSize) / 2.0f;
        float y = area.y + (area.h - charSize) / 2.0f;

        drawText(renderer, text, x, y, scale);
    }
}
