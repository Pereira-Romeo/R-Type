/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** GameScene
*/

#include "../../include/GameScene.hpp"

namespace client
{
    GameScene::GameScene(SceneManager &scenes, float screenWidth, float screenHeight)
        : _scenes(scenes), _screenWidth(screenWidth), _screenHeight(screenHeight),
          _player(100.0f, screenHeight / 2.0f),
          _enemies()
    {
    }

    void GameScene::onLoad()
    {
    }

    void GameScene::onUnload()
    {
    }

    void GameScene::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            _scenes.switchScene("menu");
    }

    void GameScene::update(float deltaTime)
    {
        _player.handleInput(SDL_GetKeyboardState(nullptr));
        _player.update(deltaTime, _screenWidth, _screenHeight);
        _enemies.update(deltaTime, _screenWidth, _screenHeight, _player.getBullet());
    }

    void GameScene::render(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
        SDL_RenderClear(renderer);

        _player.render(renderer);
        _enemies.render(renderer);

        SDL_RenderPresent(renderer);
    }
}
