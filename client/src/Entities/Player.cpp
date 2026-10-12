/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Player
*/

#include "../../include/Player.hpp"
#include <algorithm>
#include <cmath>

namespace client
{
    Player::Player(float x, float y) : _x(x), _y(y) {}

    void Player::handleInput(const bool *keys)
    {
        _dirX = 0.0f;
        _dirY = 0.0f;

        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
            _dirY -= 1.0f;
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
            _dirY += 1.0f;
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
            _dirX -= 1.0f;
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
            _dirX += 1.0f;

        float length = std::sqrt(_dirX * _dirX + _dirY * _dirY);
        if (length > 0.0f) {
            _dirX /= length;
            _dirY /= length;
        }
    }

    void Player::update(float dt, float screenWidth, float screenHeight)
    {
        _x += _dirX * _speed * dt;
        _y += _dirY * _speed * dt;

        _x = std::clamp(_x, 0.0f, screenWidth - _width);
        _y = std::clamp(_y, 0.0f, screenHeight - _height);
    }

    void Player::render(SDL_Renderer *renderer) const
    {
        SDL_FRect rect = getHitbox();

        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_FRect Player::getHitbox() const
    {
        return { _x, _y, _width, _height };
    }
}
