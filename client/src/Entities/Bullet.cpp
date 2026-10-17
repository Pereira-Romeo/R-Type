/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Bullet
*/

#include "../../include/Bullet.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace client
{
    Bullet::Bullet(float x, float y) : _x(x), _y(y) {}

    void Bullet::update(float dt, float screenWidth)
    {
        _x += _speed * dt;
    }

    void Bullet::render(SDL_Renderer *renderer) const
    {
        SDL_FRect rect = getHitbox();

        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_FRect Bullet::getHitbox() const
    {
        return { _x, _y, _width, _height };
    }
}
