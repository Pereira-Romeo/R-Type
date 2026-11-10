/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** GameScene
*/

#pragma once

#include "IScene.hpp"
#include "SceneManager.hpp"
#include "ScriptEngine/ScriptEngine.hpp"
#include "TextureManager.hpp"

#include "../include/LevelRunner.hpp"
#include "../../jsonHandler/jsonHandler.hpp"

#include "Registry.hpp"
#include "ScheduleManager.hpp"
#include "GameLogic/Components.hpp"
#include "GameLogic/Systems.hpp"

namespace client
{
    class GameScene : public IScene {
        public:
            GameScene(SceneManager &scenes, float screenWidth, float screenHeight);

            void onLoad() override;
            void onUnload() override;
            void registerLuaComponents();
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            SceneManager &_scenes;
            float _screenWidth;
            float _screenHeight;
            float _spawnTimer   = 0.f;
            SDL_Gamepad *_gamepad = nullptr;
            ScheduleManager _schedule;
            SDL_Renderer *_renderer = nullptr;
            ecs::Registry _registry;
            std::unique_ptr<ScriptEngine> _scriptEngine;
            LevelRunner _level;
            TextureManager _textures;
            bool _playerSeen = false;
    };
}
