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
        : _scenes(scenes), _screenWidth(screenWidth), _screenHeight(screenHeight)
    {
    }

    void GameScene::onLoad()
    {
        int num_gamepads = 0;
        SDL_JoystickID* gamepad_ids = SDL_GetGamepads(&num_gamepads);

        if (num_gamepads > 0 && gamepad_ids != nullptr)
        {
            _gamepad = SDL_OpenGamepad(gamepad_ids[0]);
            if (_gamepad == nullptr)
                SDL_Log("Erreur manette : %s", SDL_GetError());
            else
                SDL_Log("Manette connectee : %s",SDL_GetGamepadName(_gamepad));
        }
        else
            SDL_Log("Aucune manette detectee.");
        SDL_free(gamepad_ids);
        
        _registry.register_component<Position>();
        _registry.register_component<Velocity>();
        _registry.register_component<Hitbox>();
        _registry.register_component<Renderable>();
        _registry.register_component<Controllable>();
        _registry.register_component<Shooter>();
        _registry.register_component<Projectile>();
        _registry.register_component<Enemy>();

        size_t player = createPlayer(_registry, 100.0f, _screenHeight / 2.0f);
        _schedule.add(Bucket::Input, "input", [this](ecs::Registry &reg, float dt) {
            inputSystem(reg, this->_gamepad);
        });
        _schedule.add(Bucket::Update, "pop enemy", [this](ecs::Registry &reg, float dt) {
            enemySpawnSystem(reg, dt, _spawnTimer, _screenWidth, _screenHeight);
        });
        _schedule.add(Bucket::Update, "update control", controlSystem);
        _schedule.add(Bucket::Update, "Update movement", movementSystem);
        _schedule.add(Bucket::Physique, "Update clamp pos", [this](ecs::Registry &reg, float dt) {
            clampSystem(reg, _screenWidth, _screenHeight);
        });
        _schedule.add(Bucket::Physique, "delete player", playerHitSystem);
        _schedule.add(Bucket::Physique, "delete enemy", collisionSystem);
        _schedule.add(Bucket::Update, "update shooting", shootingSystem);
        _schedule.add(Bucket::Fin, "Destroy bullet", [this](ecs::Registry &reg, float dt) {
            offscreenSystem(reg, _screenWidth);
        });
        _schedule.add(Bucket::Rendu, "render", [this](ecs::Registry &reg, float dt) {
            renderSystem(reg, this->_renderer);
        });
    }

    void GameScene::onUnload()
    {
        _schedule.clear();
        _registry.reset();

        if (_gamepad) {
            SDL_CloseGamepad(_gamepad);
            _gamepad = nullptr;
        }
        _renderer = nullptr;
        _spawnTimer = 0.f;
    }

    void GameScene::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            _scenes.switchScene("menu");
    }

    void GameScene::update(float deltaTime)
    {
        // _player.handleInput(SDL_GetKeyboardState(nullptr));
        // _player.update(deltaTime, _screenWidth, _screenHeight);
        // _enemies.update(deltaTime, _screenWidth, _screenHeight, _player.getBullet());
        _schedule.run(_registry, deltaTime, Bucket::Input, Bucket::Fin);
    }

    void GameScene::render(SDL_Renderer *renderer)
    {
        _renderer = renderer;
        SDL_SetRenderDrawColor(_renderer, 10, 10, 30, 255);
        SDL_RenderClear(_renderer);

        // _player.render(renderer);
        // _enemies.render(renderer);
        _schedule.run(_registry, 0.0f, Bucket::Rendu, Bucket::Rendu);

        SDL_RenderPresent(_renderer);
    }
}
