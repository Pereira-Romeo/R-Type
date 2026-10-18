/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** Components for game logic
*/

#pragma once

#include <SDL3/SDL.h>
#include <math.h>

struct Position     { float x = 0.f, y = 0.f; };
struct Velocity     { float x = 0.f, y = 0.f; };
struct Hitbox       { float w = 0.f, h = 0.f; };
struct Renderable   { __uint8_t r, g, b, a = 255; };
struct Controllable { float speed = 300.f; };
struct InputState   { bool up=false, down=false, left=false, right=false, shoot=false; };
struct Shooter      { float cooldown = 0.f, interval = 0.2f; };
struct Projectile   {}; // tag
struct Enemy        {}; // tag