/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** GameScene
*/

#include "../../include/GameScene.hpp"
#include <SDL3/SDL_log.h>

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
                SDL_Log("Error controller : %s", SDL_GetError());
            else
                SDL_Log("Controller connected : %s",SDL_GetGamepadName(_gamepad));
        }
        else
            SDL_Log("Aucune manette detectee.");
        SDL_free(gamepad_ids);
        _registry.register_component<Position>();
        _registry.register_component<Velocity>();
        _registry.register_component<Hitbox>();
        _registry.register_component<Renderable>();
        _registry.register_component<InputState>();
        _registry.register_component<Controllable>();
        _registry.register_component<Shooter>();
        _registry.register_component<Animation>();
        _registry.register_component<Sprite>();
        _registry.register_component<EnemyShooter>();
        _registry.register_component<EnemyProjectile>();
        _registry.register_component<Stat>();
        _registry.register_component<Hurtbox>();
        _registry.register_component<BossMotion>();
        _registry.register_component<BossShooter>();
        _registry.register_component<Solids>();
        _registry.register_component<PowerUp>();
        _registry.register_component<Projectile>();
        _registry.register_component<Enemy>();

        _scriptEngine = std::make_unique<ScriptEngine>(
            _screenWidth, _screenHeight,
            [this](const std::string &name) { return _registry.create_entity(name); },
            [this](size_t e) { _registry.delete_entity(e);}
        );
        registerLuaComponents();
        _scriptEngine->loadDir("assets/scripts");

        auto lvl = jsonHandler::LoadLevel("assets/levels/level1.json");
        SDL_Log("[level] %s : %zu vagues", lvl.name.c_str(), lvl.waves.size());
        _level.load(lvl);
    
        // ===== INPUT =====
        _schedule.add(Bucket::Input, "input", [this](ecs::Registry &reg, float) {
            inputSystem(reg, _gamepad);
        });

        // ===== UPDATE =====
        _schedule.add(Bucket::Update, "update control", controlSystem);
        _schedule.add(Bucket::Update, "Update movement", movementSystem);
        _schedule.add(Bucket::Update, "ship tilt", shipTiltSystem);
        _schedule.add(Bucket::Update, "Animatio,", [this](ecs::Registry &reg, float dt) {
            animationSystem(reg, dt, _screenWidth, _screenHeight);
        });
        _schedule.add(Bucket::Update, "update shooting", [this](ecs::Registry &reg, float dt) {
            shootingSystem(reg, dt, *_scriptEngine);
        });
        _schedule.add(Bucket::Update, "enemy shooting", [this](ecs::Registry &reg, float dt) {
            enemyShootingSystem(reg, dt, *_scriptEngine, _screenWidth, _screenHeight);
        });
        _schedule.add(Bucket::Update, "boss motion", bossMotionSystem);
        _schedule.add(Bucket::Update, "boss shooting", [this](ecs::Registry &reg, float dt) {
            bossShootingSystem(reg, dt, *_scriptEngine, _screenWidth, _screenHeight);
        });

        // ===== PHYSIQUE =====
        _schedule.add(Bucket::Physique, "delete player", playerHitSystem);
        _schedule.add(Bucket::Physique, "delete enemy", collisionSystem);
        _schedule.add(Bucket::Physique, "Update clamp pos", [this](ecs::Registry &reg, float dt) {
            clampSystem(reg, _screenWidth, _screenHeight);
        });

        // ===== FIN =====
        _schedule.add(Bucket::Fin, "Destroy bullet", [this](ecs::Registry &reg, float dt) {
            offscreenSystem(reg, _screenWidth);
        });

        // ===== RENDER =====
        _schedule.add(Bucket::Rendu, "render", [this](ecs::Registry &reg, float dt) {
            renderSystem(reg, _renderer, _textures, _screenWidth, _screenHeight);
            healthBarSystem(reg, _renderer);
        });
    }

    void GameScene::onUnload()
    {
        _schedule.clear();
        _registry.reset();
        _textures.clear();
        _scriptEngine.reset();
        if (_gamepad) {
            SDL_CloseGamepad(_gamepad);
            _gamepad = nullptr;
        }
        _renderer = nullptr;
        _spawnTimer = 0.f;
        _playerSeen = false;
    }

    void GameScene::registerLuaComponents()
    {
        auto &se = *_scriptEngine;

        se.registerComponent("position", [this](size_t e, const sol::table &t) {
            _registry.add<Position>(e, t.get_or("x", 0.f), t.get_or("y", 0.f));
        });
        se.registerComponent("stat", [this](size_t e, const sol::table &t) {
            float life = t.get_or("life", 100.0f);
            float lifeMax = t.get_or("lifeMax", 100.0f);
            if (lifeMax <= 0)
                lifeMax = 100;
            if (life > lifeMax)
                life = lifeMax;
            _registry.add<Stat>(e, life, lifeMax);
        });
        se.registerComponent("velocity", [this](size_t e, const sol::table &t) {
            _registry.add<Velocity>(e, t.get_or("x", 0.f), t.get_or("y", 0.f));
        });
        se.registerComponent("hitbox", [this](size_t e, const sol::table &t) {
            _registry.add<Hitbox>(e, t.get_or("w", 0.f), t.get_or("h", 0.f));
        });
        se.registerComponent("renderable", [this](size_t e, const sol::table &t) {
            auto c = [&](const char *k) {
                return static_cast<Uint8>(std::clamp(t.get_or(k, 255), 0, 255));
            };
            _registry.add<Renderable>(e, c("r"), c("g"), c("b"), c("a"));
        });
        se.registerComponent("controllable", [this](size_t e, const sol::table &t) {
            _registry.add<Controllable>(e, t.get_or("speed", 300.f));
        });
        se.registerComponent("input", [this](size_t e, const sol::table &) {
            _registry.add<InputState>(e);
        });
        se.registerComponent("shooter", [this](size_t e, const sol::table &t) {
            _registry.add<Shooter>(e, 
                t.get_or("cooldown", 0.f),
                t.get_or("interval", 0.2f),
                t.get_or("weapon", std::string("basic")));
        });
        se.registerComponent("sprite", [this](size_t e, const sol::table &t) {
            Sprite s {
                t.get_or("texture", std::string{}),
                t.get_or("col", 0),
                t.get_or("row", 0),
                t.get_or("w", 0.f),
                t.get_or("h", 0.f),
            };
            s.offsetX = t.get_or("offset_x", 0.f);
            s.offsetY = t.get_or("offset_y", 0.f);
            sol::optional<sol::table> src = t["src"];
            if (src) {
                s.srcX = src->get_or("x", 0.f);
                s.srcY = src->get_or("y", 0.f);
                s.srcW = src->get_or("w", 0.f);
                s.srcH = src->get_or("h", 0.f);
            };
            _registry.add<Sprite>(e, std::move(s));
        });

        se.registerComponent("animation", [this](size_t e, const sol::table &t) {
            Animation anim;
            anim.frames = t.get_or("frames", 1);
            anim.cols = t.get_or("cols", 1);
            anim.frameTime = 1.f / t.get_or("fps", 10.f);
            anim.loop = t.get_or("loop", true);
            anim.loopFrom = t.get_or("loop_from", 0);
            anim.loopTo = t.get_or("loop_to", -1);
            _registry.add<Animation>(e, std::move(anim));
        });
        se.registerComponent("enemy_shooter", [this](size_t e, const sol::table &t) {
            _registry.add<EnemyShooter>(e, t.get_or("weapon", std::string("enemy_basic")));
        });
        se.registerComponent("enemy_projectile", [this](size_t e, const sol::table &) {
            _registry.add<EnemyProjectile>(e);
        });

        se.registerComponent("hurtbox", [this](size_t e, const sol::table &t) {
            _registry.add<Hurtbox>(e,
                t.get_or("x", 0.f),
                t.get_or("y", 0.f),
                t.get_or("w", 0.f),
                t.get_or("h", 0.f)
            );
        });
        se.registerComponent("boss_motion", [this](size_t e, const sol::table &t) {
            BossMotion b;
            b.stopX = t.get_or("stop_x", 0.f);
            b.enterSpeed = t.get_or("enter_speed", 100.f);
            b.baseY = t.get_or("base_y", 0.f);
            b.amp = t.get_or("amp", 80.f);
            b.speed = t.get_or("speed", 1.5f);
            _registry.add<BossMotion>(e, std::move(b));
        });

        se.registerComponent("boss_shooter", [this](size_t e, const sol::table &t) {
            BossShooter bs;
            bs.activeFromFrame = t.get_or("active_from", 0);

            sol::optional<sol::table> shooters = t["shooters"];
            if (shooters) {
                for (auto &kv : *shooters) {
                    sol::table g = kv.second;
                    Shooter shooter;
                    shooter.weapon = g.get_or("weapon", std::string("enemy_basic"));
                    shooter.ox = g.get_or("x", 0.f);
                    shooter.oy = g.get_or("y", 0.f);
                    shooter.interval = g.get_or("interval", 1.f);
                    shooter.cooldown = g.get_or("delay", 0.f);
                    bs.shooters.push_back(std::move(shooter));
                }
            }
            _registry.add<BossShooter>(e, std::move(bs));
        });

        se.registerComponent("solids", [this](size_t e, const sol::table &t) {
            Solids s;
            for (auto &kv : t) {
                sol::table r = kv.second;
                s.rects.push_back({
                    r.get_or("x", 0.f),
                    r.get_or("y", 0.f),
                    r.get_or("w", 0.f),
                    r.get_or("h", 0.f)
                });
            }
            _registry.add<Solids>(e, std::move(s));
        });

        // tags
        se.registerComponent("enemy", [this](size_t e, const sol::table &) {
            _registry.add<Enemy>(e);
        });

        se.registerComponent("projectile", [this](size_t e, const sol::table &) {
            _registry.add<Projectile>(e);
        });
        se.registerComponent("powerup", [this](size_t e, const sol::table &) {
            _registry.add<PowerUp>(e);
        });
    }

    void GameScene::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE)
            _scenes.switchScene("menu");
    }

    void GameScene::update(float deltaTime)
    {
        _scriptEngine.get()->update(deltaTime);

        const float SCROLL_SPEED = 100.0f;   // unités du niveau par seconde
        _level.update(deltaTime, SCROLL_SPEED, [&](const SpawnEvent &e) {
            float y = (float)(std::rand() % (int)(_screenHeight - 75.0f));
            _scriptEngine->spawnMonster(e.monster, _screenWidth, y);
        });

        _schedule.run(_registry, deltaTime, Bucket::Input, Bucket::Fin);
        
        bool alive = false;
        for (auto ent : _registry.view<Controllable>()) {
            (void)ent;
            alive = true;
            break;
        }
        if (alive)
            _playerSeen = true;
        else if (_playerSeen)
            _scenes.switchScene("menu");
    }

    void GameScene::render(SDL_Renderer *renderer)
    {
        _renderer = renderer;
        _scriptEngine->draw(renderer, [this] {
            _schedule.run(_registry, 0.0f, Bucket::Rendu, Bucket::Rendu);
        });
    }
}

