/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** IScene
*/

#pragma once

#include <SDL3/SDL.h>

namespace client
{
    class IScene {

        public:
            virtual ~IScene() = default;

            virtual void onLoad() = 0;
            virtual void onUnload() = 0;
            virtual void handleEvent(const SDL_Event &event) = 0;
            virtual void update(float deltaTime) = 0;
            virtual void render(SDL_Renderer *renderer) = 0;
    };
}
