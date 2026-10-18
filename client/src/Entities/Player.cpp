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
    Player::Player(float x, float y): _x(x), _y(y)
    {
        int num_gamepads = 0;
        SDL_JoystickID* gamepad_ids = SDL_GetGamepads(&num_gamepads);

        if (num_gamepads > 0 && gamepad_ids != nullptr)
        {
            _gamepad = SDL_OpenGamepad(gamepad_ids[0]);
            if (_gamepad == nullptr)
                SDL_Log("Erreur manette : %s", SDL_GetError());
            else
                SDL_Log("Manette connectee : %s",SDL_GetGamepadName(_gamepad));
        }
        else
            SDL_Log("Aucune manette detectee.");
        SDL_free(gamepad_ids);
    }

    void Player::handleInput(const bool *keys)
    {
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] || (_gamepad && SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP)))
            _handler->Delay("UP");
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] || (_gamepad && SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)))
            _handler->Delay("DOWN");
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A] ||  (_gamepad && SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT)))
            _handler->Delay("LEFT");
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D] || (_gamepad && SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)))
            _handler->Delay("RIGHT");
        
        // Shoot
        bool wantsToShoot = keys[SDL_SCANCODE_SPACE] ||
            (_gamepad && SDL_GetGamepadButton(_gamepad, SDL_GAMEPAD_BUTTON_SOUTH));

        if (wantsToShoot && _shootCooldown <= 0.0f) {
            _BulletManager.shoot(_x + _width, _y + _height / 2.0f);
            _shootCooldown = _shootInterval;
        }
    }

    void Player::ManageInput(void)
    {
        if (_input == "UP")
            _dirY -= 1.0f;

        if (_input == "DOWN")
            _dirY += 1.0f;

        if (_input == "LEFT")
            _dirX -= 1.0f;

        if (_input == "RIGHT")
            _dirX += 1.0f;

        // analogi gauche
        /*
        if (_gamepad) {
            float stickX = (float)SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_LEFTX) / 32767.0f;
            float stickY = (float)SDL_GetGamepadAxis(_gamepad, SDL_GAMEPAD_AXIS_LEFTY) / 32767.0f;

            // zone morte → drift
            if (SDL_fabs(stickX) > 0.2f)
                _dirX += stickX;
            if (SDL_fabs(stickY) > 0.2f)
                _dirY += stickY;
        }
        */

        // normalisé moove diag
        float length = std::sqrt(_dirX * _dirX + _dirY * _dirY);
        if (length > 1.0f) {
            _dirX /= length;
            _dirY /= length;
        }
    }

    void Player::update(float dt, float screenWidth, float screenHeight)
    {
        _dirX = 0.0f;
        _dirY = 0.0f;
    
        if (_shootCooldown > 0.0f)
            _shootCooldown -= dt;

        _handler->updateFrame();
        std::vector<std::string> inputList = _handler->Read();

        if (inputList.empty()) {
            _input = std::string();
        } else {
            for (auto input : inputList) {
                _input = input;
                ManageInput();
            }
        }

        _x += _dirX * _speed * dt;
        _y += _dirY * _speed * dt;

        _x = std::clamp(_x, 0.0f, screenWidth - _width);
        _y = std::clamp(_y, 0.0f, screenHeight - _height);
        _BulletManager.update(dt, screenWidth);
    }

    void Player::render(SDL_Renderer *renderer) const
    {
        SDL_FRect rect = getHitbox();

        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
        _BulletManager.render(renderer);
    }

    SDL_FRect Player::getHitbox() const
    {
        return { _x, _y, _width, _height };
    }
}