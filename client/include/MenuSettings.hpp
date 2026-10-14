/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** MenuSettings
*/

#pragma once

#include "ButtonList.hpp"
#include "IScene.hpp"
#include "SceneManager.hpp"

namespace client
{
    class MenuSettings : public IScene {

        public:
            MenuSettings(SceneManager &scenes, float screenWidth, float screenHeight, SDL_Window* window);

            void onLoad() override;
            void onUnload() override;
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            SDL_Window* _window;
            SceneManager &_scenes;
            ButtonList _buttons;
            bool is_fullscreen;
            float _screenWidth;
    };
}
