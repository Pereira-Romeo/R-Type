/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** GameScene
*/

#include "../../include/GameScene.hpp"

namespace client
{
    GameScene::GameScene(SceneManager &scenes, Network *net, float screenWidth, float screenHeight)
        : _scenes(scenes), _screenWidth(screenWidth), _screenHeight(screenHeight), _net(net),
        _starfield(screenWidth, screenHeight, 250)
    {
    }

    void GameScene::onLoad()
    {
        _remotes.fill(std::nullopt);

        _connected = false;
        if (!_net->connect("127.0.0.1", 4242)) {
            _scenes.switchScene("menu");
            return;
        }
        _connected = true;

        const WelcomeData &w = _net->getWelcome();

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
        _registry.register_component<NetEnemy>();

        createPlayer(_registry, proto::startX(),
            proto::startY(w.player.id), w.player.r, w.player.g, w.player.b, true);

        for (const auto &o : w.others) {
            if (o.id >= _remotes.size() || o.id == w.player.id)
                continue;

            _remotes[o.id] = createPlayer(_registry,
                proto::startX(), proto::startY(o.id), o.r, o.g, o.b, false);
        }
    
        _schedule.add(Bucket::Input, "input", [this](ecs::Registry &reg, float dt) {
            inputSystem(reg, this->_gamepad);
        });
        _schedule.add(Bucket::Physique, "delete enemy", [this](ecs::Registry &reg, float dt) {
            collisionSystem(reg, dt, _net);
        });
        _schedule.add(Bucket::Update, "update control", controlSystem);
        _schedule.add(Bucket::Update, "Update movement", movementSystem);
        _schedule.add(Bucket::Physique, "Update clamp pos", [this](ecs::Registry &reg, float dt) {
            clampSystem(reg, _screenWidth, _screenHeight);
        });
        _schedule.add(Bucket::Physique, "delete player", playerHitSystem);
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
        _connected = false;
    }

    void GameScene::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            _scenes.switchScene("menu");
    }

    void GameScene::update(float deltaTime)
    {
        if (!_connected)
            return;

        _starfield.update(deltaTime);
        for (const NetEvent &e : _net->poll()) {
            switch (e.kind) {
            case NetEvent::Kind::PlayerJoined:
                if (e.id < _remotes.size() && !_remotes[e.id])
                    _remotes[e.id] = createPlayer(_registry, proto::startX(),
                        proto::startY(e.id), e.r, e.g, e.b, false);
                break;
            case NetEvent::Kind::PlayerInput:
                if (e.id < _remotes.size() && _remotes[e.id]) {
                    auto &in = _registry.get<InputState>(*_remotes[e.id]);
                    in.up    = e.mask & proto::INPUT_UP;
                    in.down  = e.mask & proto::INPUT_DOWN;
                    in.left  = e.mask & proto::INPUT_LEFT;
                    in.right = e.mask & proto::INPUT_RIGHT;
                    in.shoot = e.mask & proto::INPUT_SHOOT;
                }
                break;
            case NetEvent::Kind::EnemySpawn:
                if (!_enemies.contains(e.enemyId)) {
                    size_t ent = createEnemy(_registry, (float)e.x, (float)e.y);
                    _registry.add<NetEnemy>(ent, {e.enemyId});
                    _enemies[e.enemyId] = ent;
                }
                break;
            case NetEvent::Kind::EnemyDead: {
                auto it = _enemies.find(e.enemyId);
                if (it != _enemies.end()) {
                    _registry.delete_entity(it->second);
                    _enemies.erase(it);
                }
                break;
            }
            default:
                break;
            }
        }

        _schedule.run(_registry, deltaTime, Bucket::Input, Bucket::Fin);

        std::uint8_t mask = 0;
        for (auto ent : _registry.view<InputState, LocalPlayer>()) {
            auto &in = _registry.get<InputState>(ent);
            if (in.up)    mask |= proto::INPUT_UP;
            if (in.down)  mask |= proto::INPUT_DOWN;
            if (in.left)  mask |= proto::INPUT_LEFT;
            if (in.right) mask |= proto::INPUT_RIGHT;
            if (in.shoot) mask |= proto::INPUT_SHOOT;
        }
        _net->sendInput(mask);
    }

    void GameScene::render(SDL_Renderer *renderer)
    {
        if (!_connected)
            return;

        _renderer = renderer;
        _starfield.render(_renderer);
        _schedule.run(_registry, 0.0f, Bucket::Rendu, Bucket::Rendu);
    }
}
