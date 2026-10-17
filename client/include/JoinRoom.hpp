/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** JoinRoom
*/

#pragma once

#include <string>

#include "ButtonList.hpp"
#include "ClientContext.hpp"
#include "IScene.hpp"
#include "SceneManager.hpp"
#include "TextField.hpp"

namespace client
{
    class JoinRoom : public IScene {
        public:
            JoinRoom(SceneManager &scenes, ClientContext &context, float screenWidth, float screenHeight);

            void onLoad() override;
            void onUnload() override;
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            void tryJoin();

            static constexpr std::size_t ROOM_CODE_LENGTH = 4;

            SceneManager &_scenes;
            ClientContext &_context;
            TextField _codeField;
            ButtonList _buttons;
            std::string _error;
            float _screenWidth;
    };
}
