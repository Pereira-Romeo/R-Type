/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** manager Bullet
*/

#pragma once

#include <vector>
#include "Bullet.hpp"

namespace client {
    class BulletManager {
        public:
            BulletManager() = default;
            ~BulletManager() = default;
            

            void shoot(float x, float y);
            void update(float dt, float screenWidth);
            void render(SDL_Renderer *renderer) const;

            std::vector<Bullet> &getBullets() {return bullets;};  

        private:
            std::vector<Bullet> bullets;
            float _shootTimer;
            float _shootInterval;


    };
}