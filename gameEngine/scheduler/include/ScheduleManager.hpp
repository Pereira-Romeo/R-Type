/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** ScheduleManager with bucket
*/

#pragma once

#include <entt/entt.hpp>
#include <array>
#include <functional>
#include <string>
#include <vector>

enum class Bucket {
    Input,
    Update,
    Physique,
    Fin,
    Rendu,
    COUNT
};

class ScheduleManager {
    private:
        using Systeme = std::function<void(entt::registry&, float)>;
        struct Entree { std::string nom; Systeme fn; };
        std::array<std::vector<Entree>, static_cast<int>(Bucket::COUNT)> m_buckets;

    public:
        void OnStart(); //init jeu (remplie registry)
        void OnStop(); //stop jeu (clean registtry)
        void add(Bucket bucket, std::string nom, Systeme fn) {
            m_buckets[static_cast<int>(bucket)].push_back({std::move(nom), std::move(fn)});
        }

        void run(entt::registry& registry, float dt) {
            for (int b = 0; b < static_cast<int>(Bucket::COUNT); ++b) {
                for (auto& entree : m_buckets[b])
                    entree.fn(registry, dt);
            }
        }

};