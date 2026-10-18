/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** System func
*/

#pragma once

#include "Registry.hpp"
#include "Components.hpp"
#include "Factories.hpp"

#include <SDL3/SDL.h>
#include <math.h>
#include <set>
#include <cstdlib>

inline void movementSystem(ecs::Registry &reg, float dt)
{
    for (auto ent : reg.view<Position, Velocity>()) {
        auto &pos = reg.get<Position>(ent);
        auto &vel = reg.get<Velocity>(ent);
        pos.x +=  vel.x * dt;
        pos.y +=  vel.y * dt;
    }
}

inline void inputSystem(ecs::Registry &reg, SDL_Gamepad *pad)
{
    const bool *keys = SDL_GetKeyboardState(nullptr);

    for (auto ent : reg.view<InputState, Controllable>())
    {
        auto &in = reg.get<InputState>(ent);
        in.up = (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W] || (pad && SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_UP)));
        in.down = (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S] || (pad && SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)));
        in.left = (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A] || (pad && SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_LEFT)));
        in.right = (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D] || (pad && SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)));
        in.shoot = keys[SDL_SCANCODE_SPACE] || (pad && SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_SOUTH));
    }
}

inline void controlSystem(ecs::Registry &reg, float dt)
{
    for (auto ent: reg.view<Controllable, InputState, Velocity>()) {
        auto &in = reg.get<InputState>(ent);
        auto &v = reg.get<Velocity>(ent);
        auto &c = reg.get<Controllable>(ent);

        float dx = float(in.right) - float(in.left);
        float dy = float(in.down) - float(in.up);
        float len = std::sqrt(dx * dx + dy * dy);
        if (len > 1.f) { dx /= len; dy /= len; }
        v.x = dx * c.speed;
        v.y = dy * c.speed;
    }
}

inline void shootingSystem(ecs::Registry &reg, float dt)
{
    auto whereToShoot = std::vector<Position>{};

    for (auto ent: reg.view<Shooter, Position, Hitbox, InputState>()) {
        auto &sh = reg.get<Shooter>(ent);
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);

        if (sh.cooldown > 0.0f)
            sh.cooldown -= dt;
        if (reg.get<InputState>(ent).shoot && sh.cooldown <= 0.0f){ 
            whereToShoot.push_back({pos.x + box.w, pos.y + box.h / 2.0f});
            sh.cooldown = sh.interval;
        }
    }
    for (auto it: whereToShoot)
        createBullet(reg, it.x, it.y);
}

inline void renderSystem(ecs::Registry &reg, SDL_Renderer *renderer)
{
    for (auto ent: reg.view<Hitbox, Position, Renderable>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        auto &cl = reg.get<Renderable>(ent);

        SDL_FRect rect = {pos.x, pos.y, box.w, box.h};
        SDL_SetRenderDrawColor(renderer, cl.r, cl.g, cl.b, cl.a);
        SDL_RenderFillRect(renderer, &rect);
    }
}

inline void clampSystem(ecs::Registry &reg, float screenWidth, float screenHeight)
{
    for (auto ent : reg.view<Position, Hitbox, Controllable>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        pos.x = std::clamp(pos.x, 0.0f, screenWidth - box.w);
        pos.y = std::clamp(pos.y, 0.0f, screenHeight - box.h);
    }
}

inline void offscreenSystem(ecs::Registry &reg, float screenWidth)
{
    auto toDestroy = std::vector<size_t>{};

    for (auto ent : reg.view<Position, Hitbox>().without<Controllable>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        if (pos.x >= screenWidth || pos.x <= -box.w)
            toDestroy.push_back(ent);
    }

    for (auto ent : toDestroy)
        reg.delete_entity(ent);
}

inline bool overlaps(const Position &pa, const Hitbox &ba, const Position &pb, const Hitbox &bb)
{
    return !(pa.x + ba.w <= pb.x || pa.x >= pb.x + bb.w ||
             pa.y + ba.h <= pb.y || pa.y >= pb.y + bb.h);
}

inline void collisionSystem(ecs::Registry &reg, float dt)
{
    auto toDestroy = std::set<size_t>{};

    for (auto bullet : reg.view<Position, Hitbox, Projectile>()) {
        auto &bpos = reg.get<Position>(bullet);
        auto &bbox = reg.get<Hitbox>(bullet);

        for (auto enemy : reg.view<Position, Hitbox, Enemy>()) {
            auto &epos = reg.get<Position>(enemy);
            auto &ebox = reg.get<Hitbox>(enemy);

            if (overlaps(bpos, bbox, epos, ebox)) {
                toDestroy.insert(bullet);
                toDestroy.insert(enemy);
            }
        }
    }

    for (auto ent : toDestroy)
        reg.delete_entity(ent);
}

inline void enemySpawnSystem(ecs::Registry &reg, float dt, float &timer, float screenWidth, float screenHeight)
{
    timer += dt;
    float spawnInterval = 1.5f;

    if (timer >= spawnInterval) {
        timer = 0.f;
        createEnemy(reg, screenWidth, (float)(std::rand() % (int)(screenHeight - 31)));
    }
}

inline void playerHitSystem(ecs::Registry &reg, float dt)
{
    auto toDestroy = std::set<size_t>{};

    for (auto player : reg.view<Position, Hitbox, Controllable>()) {
        auto &ppos = reg.get<Position>(player);
        auto &pbox = reg.get<Hitbox>(player);

        for (auto enemy : reg.view<Position, Hitbox, Enemy>()) {
            auto &epos = reg.get<Position>(enemy);
            auto &ebox = reg.get<Hitbox>(enemy);

            if (overlaps(ppos, pbox, epos, ebox)) {
                toDestroy.insert(player);
                toDestroy.insert(enemy);
            }
        }
    }

    for (auto ent : toDestroy)
        reg.delete_entity(ent);
}