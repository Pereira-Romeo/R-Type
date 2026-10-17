/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** PlayMenu
*/

#pragma once

#include "ButtonList.hpp"
#include "ClientContext.hpp"
#include "IScene.hpp"
#include "SceneManager.hpp"

namespace client
{
    class PlayMenu : public IScene {
        public:
            PlayMenu(SceneManager &scenes, ClientContext &context, float screenWidth, float screenHeight);

            void onLoad() override;
            void onUnload() override;
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            void createRoom();

            SceneManager &_scenes;
            ClientContext &_context;
            ButtonList _buttons;
            float _screenWidth;
    };
}
