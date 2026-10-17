/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** SceneManager
*/

#include "../../include/SceneManager.hpp"

namespace client
{
    void SceneManager::addScene(const std::string &name, std::unique_ptr<IScene> scene)
    {
        _scenes[name] = std::move(scene);
    }

    void SceneManager::switchScene(const std::string &name)
    {
        _pendingScene = name;
    }

    void SceneManager::requestQuit()
    {
        _quitRequested = true;
    }

    bool SceneManager::shouldQuit() const
    {
        return _quitRequested;
    }

    void SceneManager::handleEvent(const SDL_Event &event)
    {
        if (_currentScene)
            _currentScene->handleEvent(event);
    }

    void SceneManager::update(float deltaTime)
    {
        applyPendingScene();
        if (_currentScene)
            _currentScene->update(deltaTime);
    }

    void SceneManager::render(SDL_Renderer *renderer)
    {
        if (_currentScene)
            _currentScene->render(renderer);
    }

    void SceneManager::applyPendingScene()
    {
        if (!_pendingScene)
            return;

        auto it = _scenes.find(*_pendingScene);
        _pendingScene.reset();
        if (it == _scenes.end() || it->second.get() == _currentScene)
            return;

        if (_currentScene)
            _currentScene->onUnload();
        _currentScene = it->second.get();
        _currentScene->onLoad();
    }
}
