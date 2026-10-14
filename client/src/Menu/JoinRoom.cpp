/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** JoinRoom
*/

#include "../../include/JoinRoom.hpp"
#include "../../include/FakeNetwork.hpp"
#include <cctype>

namespace client
{
    JoinRoom::JoinRoom(SceneManager &scenes, ClientContext &context, float screenWidth, float screenHeight)
        : _scenes(scenes), _context(context),
          _codeField(context.window, {screenWidth / 2.0f - 150.0f, 280.0f, 300.0f, 70.0f},
              ROOM_CODE_LENGTH, "CODE"),
          _buttons(screenWidth / 2.0f, 420.0f, 300.0f, 60.0f, 20.0f),
          _screenWidth(screenWidth)
    {
        (void)screenHeight;
        _codeField.setUppercase(true);
        _codeField.setCharFilter([](char c) { return std::isalnum(static_cast<unsigned char>(c)) != 0; });
        _codeField.setOnSubmit([this]() { tryJoin(); });

        _buttons.addButton("JOIN", [this]() { tryJoin(); });
        _buttons.addButton("BACK", [this]() { _scenes.switchScene("playmenu"); });
    }

    void JoinRoom::onLoad()
    {
        _codeField.clear();
        _codeField.focus();
        _buttons.reset();
        _error.clear();
    }

    void JoinRoom::onUnload()
    {
        _codeField.unfocus();
    }

    void JoinRoom::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            _scenes.switchScene("playmenu");
            return;
        }

        if (_codeField.handleEvent(event))
            return;
        _buttons.handleEvent(event);
    }

    void JoinRoom::update(float deltaTime)
    {
        (void)deltaTime;
    }

    void JoinRoom::render(SDL_Renderer *renderer)
    {
        SDL_FRect titleArea = {0.0f, 100.0f, _screenWidth, 80.0f};
        SDL_FRect labelArea = {0.0f, 230.0f, _screenWidth, 40.0f};
        SDL_FRect errorArea = {0.0f, 360.0f, _screenWidth, 40.0f};

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawCenteredText(renderer, "JOIN ROOM", titleArea, 6.0f);
        drawCenteredText(renderer, "ROOM CODE", labelArea, 2.0f);

        _codeField.render(renderer);

        if (!_error.empty()) {
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
            drawCenteredText(renderer, _error, errorArea, 2.0f);
        }
        _buttons.render(renderer);
    }

    void JoinRoom::tryJoin()
    {
        const std::string &code = _codeField.getText();

        if (code.size() != ROOM_CODE_LENGTH) {
            _error = "THE CODE MUST BE " + std::to_string(ROOM_CODE_LENGTH) + " CHARACTERS";
            return;
        }

        sendToServer("JOIN_ROOM code=" + code + " player=" + _context.playerName);
        fakeServerResponse("ROOM_JOINED code=" + code);

        _context.roomCode = code;
        _context.isHost = false;
        _scenes.switchScene("lobby");
    }
}
