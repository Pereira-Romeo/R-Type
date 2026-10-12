/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Game
*/

#include "../../include/Game.hpp"
#include <stdexcept>

namespace client
{
    Game::Game()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error(SDL_GetError());

        _window.reset(SDL_CreateWindow("R-Type", WINDOW_WIDTH, WINDOW_HEIGHT, 0));
        if (!_window)
            throw std::runtime_error(SDL_GetError());

        _renderer.reset(SDL_CreateRenderer(_window.get(), nullptr));
        if (!_renderer)
            throw std::runtime_error(SDL_GetError());

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

        while (_running)
        {
            uint64_t now = SDL_GetTicksNS();
            float dt = static_cast<float> (now - last) / 1e9f;
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
            if (event.type == SDL_EVENT_QUIT)
                _running = false;

            if (_state == State::Menu) {
                MenuAction action = _menu.handleEvent(event);
                if (action == MenuAction::Play)
                    _state = State::Playing;
                else if (action == MenuAction::Quit)
                    _running = false;
            }
        }
    }

    void Game::update(float dt)
    {
        if (_state != State::Playing)
            return;

        _player.handleInput(SDL_GetKeyboardState(nullptr));
        _player.update(dt, WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    void Game::render()
    {
        SDL_SetRenderDrawColor(_renderer.get(), 10, 10, 30, 255);
        SDL_RenderClear(_renderer.get());

        if (_state == State::Menu)
            _menu.render(_renderer.get());
        else
            _player.render(_renderer.get());

        SDL_RenderPresent(_renderer.get());
    }
}
