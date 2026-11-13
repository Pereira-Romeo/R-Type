/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** jsonHandler
*/

#pragma once
#include <asio/serial_port_base.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <vector>

namespace jsonHandler {
    typedef struct spawn_s {
        std::string preset;
        std::string sprite;
        std::string script;
        int count = 1;
    } spawn_t;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(spawn_s, preset, sprite, script, count)

    typedef struct wave_s {
        std::string name;
        int start_x = 0;
        int length = 0;
        std::vector<spawn_t> spawns;
    } wave_t;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(wave_s, name, start_x, length, spawns)

    typedef struct levelData_s {
        std::string name;
        std::string background;
        std::string music;
        std::vector<wave_t> waves;
    } levelData_t;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(levelData_s, name, background, music, waves)

    inline nlohmann::json Load(std::string path)
    {
        nlohmann::json data;
        std::ifstream j(path);
        if (!j.is_open())
            return data;

        data = nlohmann::json::parse(j);
        return data;
    }

    inline bool Save(std::string path, levelData_t level)
    {
        std::ofstream file(path);
        if (!file.is_open())
            return false;

        nlohmann::json out = level;
        file << out.dump(4);
        return true;
    }

    inline levelData_t LoadLevel(const std::string &path)
    {
        nlohmann::json data = Load(path);
        if (data.is_null())
            return levelData_t{};

        try {
            return data.get<levelData_t>();
        } catch (const nlohmann::json::exception &e) {
            return levelData_t{};
        }
    }
}
