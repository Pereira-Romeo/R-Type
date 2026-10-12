/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Game
*/

#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "Menu.hpp"
#include "Player.hpp"

namespace client
{

    class Game {
        public:
            Game();
            ~Game();

            Game(const Game &) = delete;
            Game &operator=(const Game &) = delete;

            void run();

        private:
            void handleEvents();
            void update(float dt);
            void render();

            enum class State { Menu, Playing };

            struct WindowDeleter {void operator()(SDL_Window *w) const { SDL_DestroyWindow(w); } };
            struct RendererDeleter { void operator()(SDL_Renderer *r) const { SDL_DestroyRenderer(r); } };

            std::unique_ptr<SDL_Window, WindowDeleter> _window;
            std::unique_ptr<SDL_Renderer, RendererDeleter> _renderer;
            static constexpr int WINDOW_WIDTH = 1200;
            static constexpr int WINDOW_HEIGHT = 720;

            bool _running = true;
            Player _player{100.0f, WINDOW_HEIGHT / 2.0f};
            State _state = State::Menu;
            Menu _menu{WINDOW_WIDTH, WINDOW_HEIGHT};
    };
}