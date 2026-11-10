/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** HomeScene
*/

#pragma once 
#include "../../client/include/IScene.hpp"
#include "../../client/include/SceneManager.hpp"
#include "../../client/include/ButtonList.hpp"
#include <atomic>

namespace editor {

    class HomeScene : public client::IScene {
        private:
            client::SceneManager &_scenes;
            std::string &_levelPath;
            client::ButtonList _buttons;
            float _screenWidth;
            float _screenHeight;
            std::atomic<bool> _fileChosen{false};  // devient true quand l'utilisateur a choisi

            static void SDLCALL OnFileSelected(void *userdata, const char * const *filelist, int filter);

        public:
            HomeScene(client::SceneManager &scenes, std::string &levelPath, float screenWidth, float screenHeight);
            void onLoad() override {};
            void onUnload() override {};
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;
    };
}

