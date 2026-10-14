/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** PlayMenu
*/

#include "../../include/PlayMenu.hpp"
#include "../../include/FakeNetwork.hpp"
#include <random>

namespace client
{
    PlayMenu::PlayMenu(SceneManager &scenes, ClientContext &context, float screenWidth, float screenHeight)
        : _scenes(scenes), _context(context),
          _buttons(screenWidth / 2.0f, 260.0f, 400.0f, 60.0f, 20.0f),
          _screenWidth(screenWidth)
    {
        (void)screenHeight;
        _buttons.addButton("CREATE ROOM", [this]() { createRoom(); });
        _buttons.addButton("JOIN ROOM", [this]() { _scenes.switchScene("joinroom"); });
        _buttons.addButton("BACK", [this]() { _scenes.switchScene("menu"); });
    }

    void PlayMenu::onLoad()
    {
        _buttons.reset();
    }

    void PlayMenu::onUnload()
    {
    }

    void PlayMenu::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            _scenes.switchScene("menu");
            return;
        }
        _buttons.handleEvent(event);
    }

    void PlayMenu::update(float deltaTime)
    {
        (void)deltaTime;
    }

    void PlayMenu::render(SDL_Renderer *renderer)
    {
        SDL_FRect titleArea = {0.0f, 100.0f, _screenWidth, 80.0f};

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawCenteredText(renderer, "PLAY", titleArea, 6.0f);
        _buttons.render(renderer);
    }

    void PlayMenu::createRoom()
    {
        sendToServer("CREATE_ROOM player=" + _context.playerName);

        // Plus tard, c'est le serveur qui générera le code de la room
        static const std::string chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
        static std::mt19937 rng{std::random_device{}()};
        std::uniform_int_distribution<std::size_t> dist(0, chars.size() - 1);
        std::string code;
        for (int i = 0; i < 4; i++)
            code += chars[dist(rng)];

        fakeServerResponse("ROOM_CREATED code=" + code);
        _context.roomCode = code;
        _context.isHost = true;
        _scenes.switchScene("lobby");
    }
}
