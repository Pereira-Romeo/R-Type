/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** GameScene
*/

#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>


#include "IScene.hpp"
#include "SceneManager.hpp"
#include "ScriptEngine/ScriptEngine.hpp"
#include "TextureManager.hpp"
#include "FrameBufferHandler.hpp"
#include "Protocol.hpp"
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
            void onServerMessage(frameData_t data);

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
            TextureManager _textures;
            bool _playerSeen = false;
            
            static constexpr float TICK = 1.f / 60.f;
            FrameBufferHandler _frameBuffer;
            float _accumulator = 0.f;
            int _replayFrom = -1;
            std::uint8_t _localId = 0; // id local joueur
            std::array<InputState, proto::MAX_PLAYERS> _inputs{}; // input courant de chaque joueur
            std::array<std::optional<size_t>, proto::MAX_PLAYERS> _players{};

            static std::string encodeInput(std::uint8_t id, std::uint8_t mask);
            std::uint8_t sampleLocalMask() const;
            void overrideOverrides(const std::vector<std::string> &datas);
            void runTick();
            void rollback(int fromFrame);
    };
}
