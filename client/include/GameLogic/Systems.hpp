/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** System func
*/

#pragma once

#include "Registry.hpp"
#include "Components.hpp"
#include "ScriptEngine/ScriptEngine.hpp"

#include <SDL3/SDL.h>
#include <math.h>
#include <set>

inline bool onScreen(const Position &pos, const Hitbox &box, float screenW, float screenH)
{
    return !(pos.x >= screenW || pos.x + box.w <= 0 || pos.y >= screenH || pos.y + box.h <= 0);
}

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

    for (auto ent : reg.view<InputState, Controllable>()) {
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

struct wts {std::string weapon; float x = 0.f, y = 0.f;};
inline void shootingSystem(ecs::Registry &reg, float dt, ScriptEngine &se)
{
    auto whereToShoot = std::vector<wts>{};

    for (auto ent: reg.view<Shooter, Position, Hitbox, InputState>()) {
        auto &sh = reg.get<Shooter>(ent);
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);

        if (sh.cooldown > 0.0f)
            sh.cooldown -= dt;
        if (reg.get<InputState>(ent).shoot && sh.cooldown <= 0.0f){ 
            whereToShoot.push_back({sh.weapon, pos.x + box.w, pos.y + box.h / 2.0f});
            sh.cooldown = se.weaponInterval(sh.weapon);
        }
    }
    for (auto it: whereToShoot)
        se.fireWeapon(it.weapon, it.x, it.y);
}

inline void renderSystem(ecs::Registry &reg, SDL_Renderer *renderer, client::TextureManager &textures, float screenW, float screenH)
{
    for (auto ent: reg.view<Hitbox, Position, Renderable>().without<Sprite>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        auto &cl = reg.get<Renderable>(ent);

        if (pos.x >= screenW || pos.x + box.w <= 0 || pos.y >= screenH || pos.y + box.h <= 0)
            continue;
        SDL_FRect rect = {pos.x, pos.y, box.w, box.h};
        SDL_SetRenderDrawColor(renderer, cl.r, cl.g, cl.b, cl.a);
        SDL_RenderFillRect(renderer, &rect);
    }

    for (auto ent : reg.view<Hitbox, Position, Sprite, Renderable>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        auto &spr = reg.get<Sprite>(ent);

        if (pos.x >= screenW || pos.x + box.w <= 0 || pos.y >= screenH || pos.y + box.h <= 0)
            continue;
        SDL_Texture *tex = textures.get(renderer, spr.texture);
        if (!tex)   {
            auto &cl = reg.get<Renderable>(ent);
            SDL_FRect rect = {pos.x, pos.y, box.w, box.h};
            SDL_SetRenderDrawColor(renderer, cl.r, cl.g, cl.b, cl.a);
            SDL_RenderFillRect(renderer, &rect);
            continue;
        }
        SDL_FRect src;
        if (spr.srcW > 0)
            src = {spr.srcX, spr.srcY, spr.srcW, spr.srcH};
        else
            src = {spr.offsetX + spr.col * spr.frameW,
                spr.offsetY + spr.row * spr.frameH,
                spr.frameW, spr.frameH};

        SDL_FRect dst = {pos.x, pos.y, box.w, box.h}; // sprite calé sur la hitbox
        SDL_RenderTexture(renderer, tex, &src, &dst);
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

    for (auto ent : reg.view<Position, Hitbox>().without<Controllable, Projectile>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        if (pos.x <= -box.w)
            toDestroy.push_back(ent);
    }

    for (auto ent : reg.view<Position, Hitbox, Projectile>()) {
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

inline bool overlapsRel(const Position &p, const Hitbox &b, const Position &owner, const Rect &r)
{
    return overlaps(p, b, Position{owner.x + r.x, owner.y + r.y}, Hitbox{r.w, r.h});
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
            auto &stat = reg.get<Stat>(enemy);

            if (!overlaps(bpos, bbox, epos, ebox))
                continue;

            bool hitWeak = true;
            bool hitSolid = true; 
            if (reg.has<Hurtbox>(enemy)) {
                hitWeak = overlapsRel(bpos, bbox, epos, reg.get<Hurtbox>(enemy));
                if (reg.has<Solids>(enemy)) {
                    hitSolid = false;
                    for (auto &r: reg.get<Solids>(enemy).rects)
                        if (overlapsRel(bpos, bbox, epos, r)) {
                            hitSolid = true;
                            break;
                        }
                }
            }
            if (!hitWeak && !hitSolid)
                continue;
            
            toDestroy.insert(bullet);
            if (hitWeak) {
                stat.life -= 40;
                if (stat.life <= 0) {
                    stat.life = 0;
                    toDestroy.insert(enemy);
                }
            }
        }
    }
    for (auto ent : toDestroy)
        reg.delete_entity(ent);
}

inline void playerHitSystem(ecs::Registry &reg, float dt)
{
    auto toDestroy = std::set<size_t>{};

    for (auto player : reg.view<Position, Hitbox, Controllable, Shooter>()) {
        auto &ppos = reg.get<Position>(player);
        auto &pbox = reg.get<Hitbox>(player);
        auto &pshoot = reg.get<Shooter>(player);

        for (auto enemy : reg.view<Position, Hitbox, Enemy>()) {
            auto &epos = reg.get<Position>(enemy);
            auto &ebox = reg.get<Hitbox>(enemy);

            if (reg.has<BossMotion>(enemy)) {
                for (auto &r: reg.get<Solids>(enemy).rects)
                    if (overlapsRel(ppos, pbox, epos, r)) {
                        toDestroy.insert(player);
                        break;
                    }
            } else if (overlaps(ppos, pbox, epos, ebox)) {
                toDestroy.insert(player);
                toDestroy.insert(enemy);
            }
        }
        for (auto powerup: reg.view<Position, Hitbox, PowerUp>()) {
            auto &upos = reg.get<Position>(powerup);
            auto &ubox = reg.get<Hitbox>(powerup);

            if (overlaps(ppos, pbox, upos, ubox)) {
                pshoot.weapon = "spread";
                toDestroy.insert(powerup);
            }
        }
    }

    for (auto bullet : reg.view<Position, Hitbox, EnemyProjectile>()) {
        auto &bpos = reg.get<Position>(bullet);
        auto &bbox = reg.get<Hitbox>(bullet);

        for (auto player : reg.view<Position, Hitbox, Controllable>()) {
            auto &stat = reg.get<Stat>(player);
            if (overlaps(bpos, bbox, reg.get<Position>(player), reg.get<Hitbox>(player))) {
                stat.life -= 10;
                if (stat.life < 0) {
                    stat.life = 0;
                    toDestroy.insert(player);
                }
                toDestroy.insert(bullet);
            }
        }
    }

    for (auto ent : toDestroy)
        reg.delete_entity(ent);
}

inline void shipTiltSystem(ecs::Registry &reg, float)
{
    for (auto ent : reg.view<Velocity, Sprite, Controllable>()) {
        auto &v = reg.get<Velocity>(ent);
        auto &s = reg.get<Sprite>(ent);
        s.col = v.y < 0.f ? 4 : (v.y > 0.f ? 0 : 2); // inverse 4 et 0 si c'est à l'envers
    }
}

inline void animationSystem(ecs::Registry &reg, float dt, float screenW, float screenH)
{
    for (auto ent : reg.view<Animation, Sprite, Position, Hitbox>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        auto &anim = reg.get<Animation>(ent);
        auto &spr = reg.get<Sprite>(ent);

        if (!onScreen(pos, box, screenW, screenH))
            continue;
        anim.finishedCycle = false;
        anim.timer += dt;
        while (anim.timer >= anim.frameTime) {
            anim.timer -= anim.frameTime;
            int last = anim.loopTo >= 0 ? anim.loopTo : anim.frames - 1;
            if (anim.index < last) {
                anim.index++;
                if (anim.index == last)
                    anim.finishedCycle = true;
            } else if (anim.loop) {
                anim.index = anim.loopFrom;
            }
        }
        spr.col = anim.index % anim.cols;
        spr.row = anim.index / anim.cols;
    }
}

inline void enemyShootingSystem(ecs::Registry &reg, float, ScriptEngine &se, float screenW, float screenH)
{
    auto shots = std::vector<wts>{};

    for (auto ent : reg.view<Animation, EnemyShooter, Position, Hitbox>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);

        if (!onScreen(pos, box, screenW, screenH))
            continue;
        if (!reg.get<Animation>(ent).finishedCycle)
            continue;
        shots.push_back({reg.get<EnemyShooter>(ent).weapon, pos.x, pos.y + box.h / 2.0f});
    }
    for (auto &s : shots)
        se.fireWeapon(s.weapon, s.x, s.y);
}

inline void drawBar(SDL_Renderer *renderer, const HealthBar &bar)
{
    float ratio = std::clamp(bar.ratio, 0.f, 1.f);

    SDL_FRect bg = {bar.x, bar.y, bar.w, bar.h};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
    SDL_RenderFillRect(renderer, &bg);

    if (ratio > 0.f) {
        SDL_FRect fg = {bar.x, bar.y, bar.w * ratio, bar.h};
        SDL_SetRenderDrawColor(renderer, bar.r, bar.g, bar.b, 255);
        SDL_RenderFillRect(renderer, &fg);
    }
}

inline void healthBarSystem(ecs::Registry &reg, SDL_Renderer *renderer)
{
    if (!renderer)
        return;

    // unit health bar
    for (auto ent : reg.view<Stat, Position, Hitbox>()) {
        auto &stat = reg.get<Stat>(ent);
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        if (stat.lifeMax <= 0.f)
            continue;
        float ratio = stat.life / stat.lifeMax;
        if (reg.has<Enemy>(ent))
            drawBar(renderer, {pos.x, pos.y - 8.f, box.w, 4.f, ratio, 220, 30, 30});
        else if (reg.has<Controllable>(ent))
            drawBar(renderer, {pos.x, pos.y - 8.f, box.w, 4.f, ratio, 40, 220, 60});
    }

    // Player HUD health bar
    float y = 10.f;
    for (auto ent : reg.view<Stat, Controllable>()) {
        auto &stat = reg.get<Stat>(ent);
        if (stat.lifeMax <= 0.f)
            continue;
        drawBar(renderer, {10.f, y, 200.f, 16.f, stat.life / stat.lifeMax, 40, 220, 60});
        y += 22.f;
    }
}

inline void bossMotionSystem(ecs::Registry &reg, float dt)
{
    for (auto ent: reg.view<Position, BossMotion>()) {
        auto &pos = reg.get<Position>(ent);
        auto &bm = reg.get<BossMotion>(ent);

        if (pos.x > bm.stopX) {
            pos.x = std::max(bm.stopX, pos.x - bm.enterSpeed * dt);
        } else {
            bm.t += dt;
            pos.y = bm.baseY + bm.amp * std::sin(bm.t * bm.speed);
        }
    }
}

inline void bossShootingSystem(ecs::Registry &reg, float dt, ScriptEngine &se, float screenW, float screenH)
{
    auto shots = std::vector<wts>{};

    for (auto ent : reg.view<BossShooter, Animation, Position, Hitbox>()) {
        auto &pos = reg.get<Position>(ent);
        auto &box = reg.get<Hitbox>(ent);
        auto &bs  = reg.get<BossShooter>(ent);

        if (!onScreen(pos, box, screenW, screenH))
            continue;
        if (reg.get<Animation>(ent).index < bs.activeFromFrame)
            continue; // not arrived to the desired frame
        for (auto &g : bs.shooters) {
            g.cooldown -= dt;
            if (g.cooldown <= 0.f) {
                shots.push_back({g.weapon, pos.x + g.ox, pos.y + g.oy});
                g.cooldown = g.interval;
            }
        }
    }
    for (auto &s : shots)
        se.fireWeapon(s.weapon, s.x, s.y);
}

