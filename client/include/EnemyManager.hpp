/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** manager enemy
*/

#pragma once

#include <vector>
#include "Enemy.hpp"
#include "BulletManager.hpp"

namespace client {
    class EnemyManager {
        public:
            EnemyManager() = default;
            ~EnemyManager() = default;
            
            void update(float dt, float screenWidth, float screenHeight, BulletManager &bullets);
            void render(SDL_Renderer *renderer) const;


        private:
            std::vector<Enemy> enemies;
            
            float _spawnTimer = 0.0f;
            float _spawnInterval = 5.0f;
            void spawnEnemy(float screenWidth, float screenHeight);


    };
}