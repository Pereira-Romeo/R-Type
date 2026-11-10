#pragma once
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>
#include "jsonHandler.hpp"

struct SpawnEvent {
    float triggerX;      // position de défilement qui déclenche le spawn
    std::string monster; // nom du monstre (nom du fichier sans extension)
};

class LevelRunner {
public:
    void load(const jsonHandler::levelData_t &level)
    {
        _events.clear();
        for (const auto &wave : level.waves) {
            for (const auto &spawn : wave.spawns) {
                std::string name = std::filesystem::path(spawn.script).filename().string();
                for (int i = 0; i < spawn.count; ++i) {
                    float x = wave.start_x + (float)(std::rand() % (wave.length + 1));
                    _events.push_back({x, name});
                }
            }
        }
        std::stable_sort(_events.begin(), _events.end(),
            [](const SpawnEvent &a, const SpawnEvent &b) { return a.triggerX < b.triggerX; });
        _next = 0;
        _scroll = 0.0f;
        SDL_Log("[level] %zu spawn events", _events.size());
    }

    // scrollSpeed : unités du niveau par seconde.
    // onSpawn(nom) est appelé pour chaque spawn atteint (plusieurs possibles par frame).
    template <typename Fn>
    void update(float dt, float scrollSpeed, Fn &&onSpawn)
    {
        _scroll += scrollSpeed * dt;
        while (_next < _events.size() && _events[_next].triggerX <= _scroll)
            onSpawn(_events[_next++]);
    }

private:
    std::vector<SpawnEvent> _events;
    size_t _next = 0;
    float _scroll = 0.0f;
};
