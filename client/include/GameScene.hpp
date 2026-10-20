/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** GameScene
*/

#pragma once

#include "IScene.hpp"

#include "Registry.hpp"
#include "ScheduleManager.hpp"
#include "Background/Starfield.hpp"
#include "GameLogic/Components.hpp"
#include "GameLogic/Factories.hpp"
#include "GameLogic/Systems.hpp"
#include "SceneManager.hpp"
#include "TextureManager.hpp"

#include <array>
#include <optional>
#include <unordered_map>
#include "Network.hpp"

namespace client
{
    class GameScene : public IScene {
        public:
            GameScene(SceneManager &scenes, Network *net, float screenWidth, float screenHeight);

            void onLoad() override;
            void onUnload() override;
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            SceneManager &_scenes;
            float _screenWidth;
            float _screenHeight;
            float _spawnTimer = 0.f;
            SDL_Gamepad *_gamepad = nullptr;
            ScheduleManager _schedule;
            SDL_Renderer *_renderer = nullptr;
            ecs::Registry _registry;
            StarField _starfield;
            TextureManager _textures;

            Network *_net;
            std::array<std::optional<size_t>, proto::MAX_PLAYERS> _remotes;
            std::unordered_map<std::uint16_t, size_t> _enemies;
            bool _connected = false;
    };
}
