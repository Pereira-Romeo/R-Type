/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Player
*/

#pragma once

#include <SDL3/SDL.h>

namespace client
{
    class Enemy {
        public:
            Enemy(float x, float y);

            void update(float dt, float screenWidth, float screenHeight);
            void render(SDL_Renderer *renderer) const;

            SDL_FRect getHitbox() const;

        private:
            float _x;
            float _y;
            float _speed = 100.0f;
            float _width = 60.0f;
            float _height = 30.0f;
    };
}
