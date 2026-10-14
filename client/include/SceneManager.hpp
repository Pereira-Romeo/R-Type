/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** SceneManager
*/

#pragma once

#include <SDL3/SDL.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "IScene.hpp"

namespace client
{
    class SceneManager {
        public:
            void addScene(const std::string &name, std::unique_ptr<IScene> scene);

            void switchScene(const std::string &name);

            void requestQuit();
            bool shouldQuit() const;

            void handleEvent(const SDL_Event &event);
            void update(float deltaTime);
            void render(SDL_Renderer *renderer);

        private:
            void applyPendingScene();

            std::unordered_map<std::string, std::unique_ptr<IScene>> _scenes;
            IScene *_currentScene = nullptr;
            std::optional<std::string> _pendingScene;
            bool _quitRequested = false;
    };
}
