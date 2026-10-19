/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** Entity Factory
*/

#pragma once

#include "Registry.hpp"
#include "Components.hpp"
#include "Protocol.hpp"

#include <SDL3/SDL.h>
#include <math.h>


inline size_t createPlayer(ecs::Registry &reg, float x, float y, uint8_t r, uint8_t g, uint8_t b, bool local)
{
    size_t ent = reg.create_entity(local ? "Player" : "RemotePlayer");
    reg.add<Position>(ent, {x, y});
    reg.add<Hitbox>(ent, {60.0f, 30.0f});
    reg.add<Velocity>(ent, {0.0f, 0.0f});
    reg.add<Renderable>(ent, {r, g, b, 255});
    reg.add<Controllable>(ent, {300.f});
    reg.add<InputState>(ent, {});
    reg.add<Shooter>(ent, {0.0f, 0.2f});
    if (local)
        reg.add<LocalPlayer>(ent, {});
    return ent;
}


inline size_t createBullet(ecs::Registry &reg, float x, float y)
{
    size_t ent = reg.create_entity("Bullet");
    reg.add<Position>(ent, {x, y});
    reg.add<Hitbox>(ent, {10.f, 5.f});
    reg.add<Velocity>(ent, {500.0f, 0.0f});
    reg.add<Renderable>(ent, {0, 255, 200, 200});
    reg.add<Projectile>(ent, {});
    return ent;
}

inline size_t createEnemy(ecs::Registry &reg, float x, float y)
{
    size_t ent = reg.create_entity("Enemy");
    reg.add<Position>(ent, {x, y});
    reg.add<Hitbox>(ent, {60.f, 30.f});
    reg.add<Velocity>(ent, {-proto::ENEMY_SPEED, 0.0f});
    reg.add<Renderable>(ent, {0, 200, 200, 20});
    reg.add<Enemy>(ent, {});
    return ent;
}
