/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Enemy
*/

#include "../../include/Enemy.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace client
{
    Enemy::Enemy(float x, float y) : _x(x), _y(y) {}

    void Enemy::update(float dt, float screenWidth, float screenHeight)
    {
        _x -= _speed * dt;

        _x = std::clamp(_x, 0.0f, screenWidth - _width);
        _y = std::clamp(_y, 0.0f, screenHeight - _height);
    }

    void Enemy::render(SDL_Renderer *renderer) const
    {
        SDL_FRect rect = getHitbox();

        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_FRect Enemy::getHitbox() const
    {
        return { _x, _y, _width, _height };
    }

}
