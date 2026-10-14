/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** MenuSettings
*/

#include "../../include/MenuSettings.hpp"
#include <iostream>

namespace client
{
    MenuSettings::MenuSettings(SceneManager &scenes, float screenWidth, float screenHeight)
        : _scenes(scenes),
          _buttons(screenWidth / 2.0f, 240.0f, 600.0f, 60.0f, 20.0f),
          _screenWidth(screenWidth)
    {
        (void)screenHeight;
        _buttons.addButton("VOLUME", []() { std::cout << "Volume" << std::endl; });
        _buttons.addButton("FULLSCREEN", []() { std::cout << "Fullscreen" << std::endl; });
        _buttons.addButton("BORDERLESS WINDOW", []() { std::cout << "BorderlessWindow" << std::endl; });
        _buttons.addButton("WINDOWED SMALL SCREEN", []() { std::cout << "SmallWindowed" << std::endl; });
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
