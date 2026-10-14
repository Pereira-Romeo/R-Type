/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** MainMenu
*/

#pragma once

#include "ButtonList.hpp"
#include "IScene.hpp"
#include "SceneManager.hpp"

namespace client
{
    class MainMenu : public IScene {
        public:
            MainMenu(SceneManager &scenes, float screenWidth, float screenHeight);

            void onLoad() override;
            void onUnload() override;
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            SceneManager &_scenes;
            ButtonList _buttons;
            float _screenWidth;
    };
}
