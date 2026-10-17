/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** MenuSettings
*/

#include "../../include/MenuSettings.hpp"
#include <iostream>

#include "../../include/Game.hpp"

namespace client
{
    MenuSettings::MenuSettings(SceneManager &scenes, float screenWidth, float screenHeight, SDL_Window* window)
        : _window(window),
          _scenes(scenes),
          _buttons(screenWidth / 2.0f, 240.0f, 600.0f, 60.0f, 20.0f),
          _screenWidth(screenWidth),
          _screenHeight(screenHeight)
    {
        _buttons.addButton("VOLUME", []() { std::cout << "Volume" << std::endl; });
        _buttons.addButton("FULLSCREEN", [this]()
        {
            if (is_fullscreen == true)
                is_fullscreen = false;
            else if (is_fullscreen == false)
                is_fullscreen = true;
        });
        _buttons.addButton("BORDERLESS WINDOW", [this]()
        {
            SDL_SetWindowFullscreen(_window, false);
            is_fullscreen = false;

            SDL_DisplayID display = SDL_GetDisplayForWindow(_window);
            SDL_Rect bounds;
            if (!SDL_GetDisplayBounds(display, &bounds))
                return;

            SDL_SetWindowBordered(_window, true);
            SDL_SetWindowSize(_window, bounds.w, bounds.h);
            SDL_SetWindowPosition(_window, bounds.x, bounds.y);
        });
        _buttons.addButton("WINDOWED SMALL SCREEN", [this]()
        {
            SDL_SetWindowFullscreen(_window, false);
            SDL_RestoreWindow(_window);
            SDL_SetWindowBordered(_window, true);
            is_fullscreen = false;

            SDL_DisplayID display = SDL_GetDisplayForWindow(_window);
            int w = static_cast<int>(_screenWidth);
            int h = static_cast<int>(_screenHeight);

            SDL_Rect usable;
            if (SDL_GetDisplayUsableBounds(display, &usable)) {
                w = std::min(w, usable.w);
                h = std::min(h, usable.h);
            }

            SDL_SetWindowSize(_window, w, h);
            SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED_DISPLAY(display), SDL_WINDOWPOS_CENTERED_DISPLAY(display));
        });
        _buttons.addButton("RETURN", [this]() { _scenes.switchScene("menu"); });
    }

    void MenuSettings::onLoad()
    {
        _buttons.reset();
    }

    void MenuSettings::onUnload()
    {
    }

    void MenuSettings::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            _scenes.switchScene("menu");
            return;
        }
        _buttons.handleEvent(event);
    }

    void MenuSettings::update(float deltaTime)
    {
        if (is_fullscreen == true)
            SDL_SetWindowFullscreen(_window, true);
        else
            SDL_SetWindowFullscreen(_window, false);
        (void)deltaTime;
    }

    void MenuSettings::render(SDL_Renderer *renderer)
    {
        SDL_FRect titleArea = {0.0f, 100.0f, _screenWidth, 80.0f};

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawCenteredText(renderer, "SETTINGS", titleArea, 5.0f);
        _buttons.render(renderer);
    }
}
