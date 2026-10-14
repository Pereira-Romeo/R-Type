/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Player
*/

#include "../../include/Player.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <valarray>

#include <chrono>
#include <thread>

namespace client
{
    Player::Player(float x, float y) : _x(x), _y(y) {}

    void Player::handleInput(const bool *keys)
    {
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
            _handler->Delay("UP");
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
            _handler->Delay("DOWN");
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
            _handler->Delay("RIGHT");
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
            _handler->Delay("LEFT");
    }

    void Player::ManageInput(void)
    {
        _dirX = 0.0f;
        _dirY = 0.0f;

        if (_input == "UP")
            _dirY -= 1.0f;
        if (_input == "DOWN")
            _dirY += 1.0f;
        if (_input == "RIGHT")
            _dirX -= 1.0f;
        if (_input == "LEFT")
            _dirX += 1.0f;

        float length = std::sqrt(_dirX * _dirX + _dirY * _dirY);
        if (length > 0.0f) {
            _dirX /= length;
            _dirY /= length;
        }
    }

    void Player::update(float dt, float screenWidth, float screenHeight)
    {
        // std::cout << "[NEW FRAME]: " << std::endl;
        //
        // std::this_thread::sleep_for(std::chrono::seconds(1));

        _handler->Display();

        _x += _dirX * _speed * dt;
        _y += _dirY * _speed * dt;

        _x = std::clamp(_x, 0.0f, screenWidth - _width);
        _y = std::clamp(_y, 0.0f, screenHeight - _height);

        _handler->updateFrame();
        _input = _handler->Read();
        if (_input != "no")
            ManageInput();
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
