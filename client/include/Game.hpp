/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Game
*/

#pragma once

#include <SDL3/SDL.h>
#include <memory>

#include "ClientContext.hpp"
#include "SceneManager.hpp"

#include "Network.hpp"

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

            struct WindowDeleter { void operator()(SDL_Window *w) const { SDL_DestroyWindow(w); } };
            struct RendererDeleter { void operator()(SDL_Renderer *r) const { SDL_DestroyRenderer(r); } };

            static constexpr int WINDOW_WIDTH = 1200;
            static constexpr int WINDOW_HEIGHT = 720;

            std::unique_ptr<SDL_Window, WindowDeleter> _window;
            std::unique_ptr<SDL_Renderer, RendererDeleter> _renderer;
            ClientContext _context;
            Network _network;
            SceneManager _scenes;
            bool _running = true;
    };
}
