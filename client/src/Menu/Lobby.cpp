/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Lobby
*/

#include "../../include/Lobby.hpp"
#include "../../include/FakeNetwork.hpp"

namespace client
{
    static constexpr float ROW_Y = 200.0f;
    static constexpr float ROW_HEIGHT = 50.0f;
    static constexpr float ROW_SPACING = 10.0f;
    static constexpr float ROW_WIDTH = 700.0f;

    Lobby::Lobby(SceneManager &scenes, ClientContext &context, float screenWidth, float screenHeight)
        : _scenes(scenes), _context(context),
          _buttons(screenWidth / 2.0f, 450.0f, 360.0f, 60.0f, 20.0f),
          _screenWidth(screenWidth)
    {
        (void)screenHeight;
    }

    void Lobby::onLoad()
    {
        _message.clear();
        loadFakePlayers();
        buildButtons();
    }

    void Lobby::onUnload()
    {
    }

    void Lobby::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            leave();
            return;
        }
        _buttons.handleEvent(event);
    }

    void Lobby::update(float deltaTime)
    {
        (void)deltaTime;
    }

    void Lobby::render(SDL_Renderer *renderer)
    {
        SDL_FRect titleArea = {0.0f, 50.0f, _screenWidth, 70.0f};
        SDL_FRect codeArea = {0.0f, 130.0f, _screenWidth, 40.0f};
        SDL_FRect messageArea = {0.0f, 395.0f, _screenWidth, 40.0f};
        const float rowX = (_screenWidth - ROW_WIDTH) / 2.0f;
        const float charSize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * 2.0f;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawCenteredText(renderer, "LOBBY", titleArea, 6.0f);
        drawCenteredText(renderer, "ROOM CODE: " + _context.roomCode, codeArea, 3.0f);

        for (std::size_t i = 0; i < _players.size(); i++) {
            const LobbyPlayer &player = _players[i];
            SDL_FRect row = {rowX, ROW_Y + i * (ROW_HEIGHT + ROW_SPACING), ROW_WIDTH, ROW_HEIGHT};
            const float textY = row.y + (ROW_HEIGHT - charSize) / 2.0f;

            SDL_SetRenderDrawColor(renderer, 30, 30, 60, 255);
            SDL_RenderFillRect(renderer, &row);
            SDL_SetRenderDrawColor(renderer, player.you ? 0 : 80, player.you ? 200 : 80, player.you ? 255 : 120, 255);
            SDL_RenderRect(renderer, &row);

            std::string name = player.name;
            if (player.host)
                name += " (HOST)";
            if (player.you)
                name += " (YOU)";
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            drawText(renderer, name, row.x + 16.0f, textY, 2.0f);

            const std::string status = player.ready ? "READY" : "NOT READY";
            if (player.ready)
                SDL_SetRenderDrawColor(renderer, 80, 220, 120, 255);
            else
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
            drawText(renderer, status, row.x + row.w - 16.0f - status.size() * charSize, textY, 2.0f);
        }

        if (!_message.empty()) {
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
            drawCenteredText(renderer, _message, messageArea, 2.0f);
        }
        _buttons.render(renderer);
    }

    void Lobby::loadFakePlayers()
    {
        _players.clear();
        if (_context.isHost) {
            _players.push_back({_context.playerName, false, true, true});
            _players.push_back({"PLAYER 2", true, false, false});
            _players.push_back({"PLAYER 3", true, false, false});
        } else {
            _players.push_back({"HOST", true, true, false});
            _players.push_back({_context.playerName, false, false, true});
            _players.push_back({"PLAYER 3", true, false, false});
        }
        fakeServerResponse("ROOM_STATE code=" + _context.roomCode
            + " players=" + std::to_string(_players.size()));
    }

    void Lobby::buildButtons()
    {
        _buttons.clear();
        _buttons.addButton("READY", [this]() { toggleReady(); });
        if (_context.isHost)
            _buttons.addButton("START", [this]() { startGame(); });
        _buttons.addButton("LEAVE", [this]() { leave(); });
    }

    void Lobby::toggleReady()
    {
        LobbyPlayer *you = findYou();
        if (!you)
            return;

        you->ready = !you->ready;
        _buttons.setLabel(0, you->ready ? "CANCEL READY" : "READY");
        _message.clear();
        sendToServer(std::string("SET_READY code=") + _context.roomCode
            + " ready=" + (you->ready ? "1" : "0"));
    }

    void Lobby::startGame()
    {
        for (const auto &player : _players) {
            if (!player.ready) {
                _message = "ALL PLAYERS MUST BE READY";
                return;
            }
        }

        sendToServer("START_GAME code=" + _context.roomCode);
        fakeServerResponse("GAME_START code=" + _context.roomCode);
        _scenes.switchScene("game");
    }

    void Lobby::leave()
    {
        sendToServer("LEAVE_ROOM code=" + _context.roomCode);
        _context.roomCode.clear();
        _context.isHost = false;
        _scenes.switchScene("playmenu");
    }

    Lobby::LobbyPlayer *Lobby::findYou()
    {
        for (auto &player : _players)
            if (player.you)
                return &player;
        return nullptr;
    }
}
