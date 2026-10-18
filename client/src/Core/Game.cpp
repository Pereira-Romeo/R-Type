/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Game
*/

#include "../../include/Game.hpp"
#include "../../include/GameScene.hpp"
#include "../../include/JoinRoom.hpp"
#include "../../include/Lobby.hpp"
#include "../../include/MainMenu.hpp"
#include "../../include/MenuSettings.hpp"
#include "../../include/PlayMenu.hpp"
#include <stdexcept>

namespace client
{
    Game::Game()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
            throw std::runtime_error(SDL_GetError());

        _window.reset(SDL_CreateWindow("R-Type", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE));
        if (!_window)
            throw std::runtime_error(SDL_GetError());

        _renderer.reset(SDL_CreateRenderer(_window.get(), nullptr));
        if (!_renderer)
            throw std::runtime_error(SDL_GetError());

        SDL_SetRenderLogicalPresentation(_renderer.get(), WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_LOGICAL_PRESENTATION_LETTERBOX);

        _context.window = _window.get();

        _scenes.addScene("menu", std::make_unique<MainMenu>(_scenes, WINDOW_WIDTH, WINDOW_HEIGHT));
        _scenes.addScene("settings", std::make_unique<MenuSettings>(_scenes, WINDOW_WIDTH, WINDOW_HEIGHT, _window.get()));
        _scenes.addScene("game", std::make_unique<GameScene>(_scenes, &_network, WINDOW_WIDTH, WINDOW_HEIGHT));
        _scenes.addScene("playmenu", std::make_unique<PlayMenu>(_scenes, _context, WINDOW_WIDTH, WINDOW_HEIGHT));
        _scenes.addScene("joinroom", std::make_unique<JoinRoom>(_scenes, _context, WINDOW_WIDTH, WINDOW_HEIGHT));
        _scenes.addScene("lobby", std::make_unique<Lobby>(_scenes, _context, WINDOW_WIDTH, WINDOW_HEIGHT));
        _scenes.switchScene("menu");
    }

    Game::~Game()
    {
        _renderer.reset();
        _window.reset();
        SDL_Quit();
    }

    void Game::run()
    {
        uint64_t last = SDL_GetTicksNS();

        while (_running && !_scenes.shouldQuit()) {
            uint64_t now = SDL_GetTicksNS();
            float dt = static_cast<float>(now - last) / 1e9f;
            last = now;

            handleEvents();
            update(dt);
            render();
        }
    }

    void Game::handleEvents()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                _running = false;
                continue;
            }
            SDL_ConvertEventToRenderCoordinates(_renderer.get(), &event);
            _scenes.handleEvent(event);
        }
    }

    void Game::update(float dt)
    {
        _scenes.update(dt);
    }

    void Game::render()
    {
        SDL_SetRenderDrawColor(_renderer.get(), 10, 10, 30, 255);
        SDL_RenderClear(_renderer.get());
        _scenes.render(_renderer.get());
        SDL_RenderPresent(_renderer.get());
    }
}
