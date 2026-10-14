/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** GameScene
*/

#pragma once

#include "IScene.hpp"
#include "Player.hpp"
#include "SceneManager.hpp"

namespace client
{
    class GameScene : public IScene {
        public:
            GameScene(SceneManager &scenes, float screenWidth, float screenHeight);

            void onLoad() override;
            void onUnload() override;
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            SceneManager &_scenes;
            float _screenWidth;
            float _screenHeight;
            Player _player;
    };
}
