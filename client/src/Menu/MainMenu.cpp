/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** MainMenu
*/

#include "../../include/MainMenu.hpp"

namespace client
{
    MainMenu::MainMenu(SceneManager &scenes, float screenWidth, float screenHeight)
        : _scenes(scenes),
          _buttons(screenWidth / 2.0f, screenHeight / 2.0f, 300.0f, 60.0f, 20.0f),
          _screenWidth(screenWidth)
    {
        _buttons.addButton("PLAY", [this]() { _scenes.switchScene("playmenu"); });
        _buttons.addButton("SETTINGS", [this]() { _scenes.switchScene("settings"); });
        _buttons.addButton("QUIT", [this]() { _scenes.requestQuit(); });
    }

    void MainMenu::onLoad()
    {
        _buttons.reset();
    }

    void MainMenu::onUnload()
    {
    }

    void MainMenu::handleEvent(const SDL_Event &event)
    {
        _buttons.handleEvent(event);
    }

    void MainMenu::update(float deltaTime)
    {
        (void)deltaTime;
    }

    void MainMenu::render(SDL_Renderer *renderer)
    {
        SDL_FRect titleArea = {0.0f, 120.0f, _screenWidth, 80.0f};

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawCenteredText(renderer, "R-TYPE", titleArea, 6.0f);
        _buttons.render(renderer);
    }
}
