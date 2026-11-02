/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** Components for game logic
*/

#pragma once

#include <SDL3/SDL.h>
#include <math.h>

struct Position {
    float x = 0.f, y = 0.f;
};

struct Velocity {
    float x = 0.f, y = 0.f; 
};

struct Hitbox {
    float w = 0.f, h = 0.f; 
};

struct Renderable {
    std::uint8_t r, g, b, a = 255; 
};

struct Controllable {
    float speed = 300.f;
};

struct InputState {
    bool up=false,
    down=false, 
    left=false, 
    right=false, 
    shoot=false;
};

struct Shooter {
    float cooldown = 0.f, interval = 0.2f;
    std::string weapon = "basic";
};

struct Sprite       { 
    std::string texture; 
    int col = 0, row = 0;
    float frameW = 0.f, frameH = 0.f; 
    float srcX = 0.f, srcY = 0.f, srcW = 0.f, srcH = 0.f;
};

struct Animation {
    int frames = 1, cols = 1, index = 0;
    float frameTime = 0.1f, timer = 0.f;
    bool loop = true;
    bool finishedCycle = false;
};

struct EnemyShooter {
    std::string weapon = "enemy_basic";
};

// ====== TAG ======
struct Projectile {};
struct Enemy {};
struct EnemyProjectile {};