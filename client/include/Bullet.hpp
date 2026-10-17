/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Player
*/

#pragma once

#include <SDL3/SDL.h>
#include <iostream>

namespace client
{
    class Bullet {
        public:
            Bullet(float x, float y);

            void update(float dt, float screenWidth);
            void render(SDL_Renderer *renderer) const;
            bool isOffScreen(float screenWidth) const {return (_x > screenWidth);}

            SDL_FRect getHitbox() const;

        private:
            float _x;
            float _y;
            float _speed = 600.0f;
            float _width = 10.0f;
            float _height = 5.0f;
    };
}
