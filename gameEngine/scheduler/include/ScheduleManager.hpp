/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** ScheduleManager with bucket
*/

#pragma once

#include "Registry.hpp"
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
        using Systeme = std::function<void(ecs::Registry&, float)>;
        struct Entree { std::string nom; Systeme fn; };
        std::array<std::vector<Entree>, static_cast<int>(Bucket::COUNT)> m_buckets;

    public:
        void add(Bucket bucket, std::string nom, Systeme fn) {
            m_buckets[static_cast<int>(bucket)].push_back({std::move(nom), std::move(fn)});
        }

        void run(ecs::Registry& registry, float dt, Bucket from, Bucket to) {
            for (int b = static_cast<int>(from);  b <= static_cast<int>(to); ++b) {
                for (auto& entree : m_buckets[b])
                    entree.fn(registry, dt);
            }
        }

        void clear() {
            for (auto &bucket : m_buckets)
                bucket.clear();
        }

};