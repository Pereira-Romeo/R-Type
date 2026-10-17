/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Lobby
*/

#pragma once

#include <string>
#include <vector>

#include "ButtonList.hpp"
#include "ClientContext.hpp"
#include "IScene.hpp"
#include "SceneManager.hpp"

namespace client
{
    class Lobby : public IScene {
        public:
            Lobby(SceneManager &scenes, ClientContext &context, float screenWidth, float screenHeight);

            void onLoad() override;
            void onUnload() override;
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            struct LobbyPlayer {
                std::string name;
                bool ready;
                bool host;
                bool you;
            };

            void loadFakePlayers();
            void buildButtons();
            void toggleReady();
            void startGame();
            void leave();
            LobbyPlayer *findYou();

            SceneManager &_scenes;
            ClientContext &_context;
            ButtonList _buttons;
            std::vector<LobbyPlayer> _players;
            std::string _message;
            float _screenWidth;
    };
}
