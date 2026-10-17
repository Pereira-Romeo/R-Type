/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Player
*/

#pragma once

#include <SDL3/SDL.h>
#include "../../frameBufferHandler/include/FrameBufferHandler.hpp"

#include "BulletManager.hpp"

namespace client
{
    class Player {
        public:
            Player(float x, float y);

            void handleInput(const bool *keys);
            void update(float dt, float screenWidth, float screenHeight);
            void render(SDL_Renderer *renderer) const;
            void ManageInput(void);
            void shoot(float x, float y);

            SDL_FRect getHitbox() const;
            BulletManager &getBullet() {return _BulletManager;};

        private:
            float _x;
            float _y;
            float _dirX = 0.0f;
            float _dirY = 0.0f;
            float _speed = 400.0f;
            float _width = 60.0f;
            float _height = 30.0f;
            std::string _input;

            FrameBufferHandler* _handler = new FrameBufferHandler();
            SDL_Gamepad *_gamepad = nullptr;

            BulletManager _BulletManager;
            float _shootCooldown = 0.0f;
            float _shootInterval = 0.2f;
    };
}
