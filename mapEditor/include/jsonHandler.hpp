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

    typedef struct boss_s {
        int trigger_x = 0;
        std::string preset;
        std::string sprite;
        std::string script;
    } boss_t;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(boss_s, trigger_x, preset, sprite, script)

    typedef struct endCondition_s {
        std::string type;
        std::string preset;
    } endCondition_t;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(endCondition_s, type, preset)

    typedef struct levelData_s {
        std::string name;
        std::string background;
        std::string music;
        std::vector<wave_t> waves;
        boss_t boss;
        endCondition_t end_condition;
    } levelData_t;
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(levelData_s, name, background, music, waves, boss, end_condition)

    inline nlohmann::json Load(std::string path)
    {
        nlohmann::json data;
        std::ifstream j(path);
        if (!j.is_open())
            return data;

        data = nlohmann::json::parse(j);
        return data;
    }

    inline bool Save(std::string path, nlohmann::json data)
    {
        std::ofstream file(path);
        if (!file.is_open())
            return false;

        file << data;
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

    inline void SaveLevel(std::string path, levelData_t levelData)
    {
        nlohmann::json data;
    }
}

