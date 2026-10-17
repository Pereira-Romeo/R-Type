/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** enemy manager
*/

#include <random>

#include "EnemyManager.hpp"

static
bool CheckCollisionFRect(SDL_FRect a, SDL_FRect b)
{
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}


namespace client {
    void EnemyManager::update(float dt, float screenWidth, float screenHeight, BulletManager &bullets)
    {
        _spawnTimer += dt;

        if (_spawnTimer >= _spawnInterval) {
            spawnEnemy(screenWidth, screenHeight);
            _spawnTimer -= _spawnInterval;
        }

        auto &bulletList = bullets.getBullets();
        for (auto it = enemies.begin(); it != enemies.end();) {
            it->update(dt, screenWidth, screenHeight);
            SDL_FRect enemyHitbox = it->getHitbox();
            bool enemyHit = false;
            for (auto bullet = bulletList.begin(); bullet != bulletList.end();)
                if (CheckCollisionFRect(bullet->getHitbox(), enemyHitbox)) {
                    bullet = bulletList.erase(bullet);
                    enemyHit = true;
                    break;
                } else 
                    ++bullet;
            if (enemyHit)
                it = enemies.erase(it);
            else
                ++it;
        }
    }

    void EnemyManager::spawnEnemy(float screenWidth, float screenHeight)
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> distY(0.0f, screenHeight - 50.0f);

        float x = screenWidth;
        float y = distY(rng);

        enemies.emplace_back(x, y);
    }

    void EnemyManager::render(SDL_Renderer *renderer) const
    {
        for (const auto &enemy : enemies) {
            enemy.render(renderer);
        }
    }
}