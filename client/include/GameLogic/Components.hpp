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
    float ox = 0.f, oy = 0.f;
};

struct BossShooter {
    std::vector<Shooter> shooters;
    int activeFromFrame = 0;
};

struct EnemyShooter {
    std::string weapon = "enemy_basic";
};

struct Sprite       { 
    std::string texture; 
    int col = 0, row = 0;
    float frameW = 0.f, frameH = 0.f; 
    float srcX = 0.f, srcY = 0.f, srcW = 0.f, srcH = 0.f;
    float offsetX = 0.f, offsetY = 0.f;
};

struct Animation {
    int frames = 1, cols = 1, index = 0;
    float frameTime = 0.1f, timer = 0.f;
    bool loop = true;
    bool finishedCycle = false;
    int loopFrom = 0;
    int loopTo = -1;
};

struct BossMotion {
    float stopX = 0.f;
    float enterSpeed = 100.f;
    float baseY = 0.f, amp = 80.f, speed = 1.5f;
    float t = 0.f;
};

struct Stat {
    float life = 0.f;
    float lifeMax = 0.f;
};

struct HealthBar {
    float x = 0.f,  y = 0.f;
    float w = 0.f,  h = 0.f;
    float ratio = 0.f;
    Uint8 r = 0.f,  g = 0.f,  b = 0.f;
};

struct Rect { float x = 0.f, y = 0.f, w = 0.f, h = 0.f; };
using Hurtbox = Rect;

struct Solids {
    std::vector<Rect> rects;
};

// ====== TAG ======
struct Projectile {};
struct Enemy {};
struct EnemyProjectile {};
struct PowerUp {};