/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** Bullet manager
*/

#include <random>

#include "BulletManager.hpp"
#include <iostream>


namespace client {
    
    void BulletManager::update(float dt, float screenWidth)
    {
        for (auto it = bullets.begin(); it != bullets.end();) {
            it->update(dt, screenWidth);

            if (it->isOffScreen(screenWidth)) {
                std::cout << "BULLET OUT OF SCREEN\n";
                it = bullets.erase(it);
            } else {
                ++it;
            }
        }
    }

    void BulletManager::shoot(float x, float y)
    {
        bullets.emplace_back(x, y);
    }

    void BulletManager::render(SDL_Renderer *renderer) const
    {
        for (const auto &bullet : bullets) {
            bullet.render(renderer);
        }
    }
}