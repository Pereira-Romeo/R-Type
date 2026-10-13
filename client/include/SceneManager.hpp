/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Scene Manager
*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "IScene.hpp"

class SceneManager {
    private:
        std::unordered_map<std::string, std::unique_ptr<IScene>> scenes;
        IScene* currentScene = nullptr;

    public:
        void addScene(const std::string& name, std::unique_ptr<IScene> scene) {
            scenes[name] = std::move(scene);
        }

        void switchScene(const std::string& name) {
            auto it = scenes.find(name);
            if (it != scenes.end()) {
                if (currentScene) {
                    currentScene->onUnload();
                }
                currentScene = it->second.get();
                currentScene->onLoad();
            }
        }

        void update(float deltaTime) {
            if (currentScene) {
                currentScene->update(deltaTime);
            }
        }

        void render() {
            if (currentScene) {
                currentScene->render();
            }
        }
};
