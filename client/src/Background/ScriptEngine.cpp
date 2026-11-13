/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** lua binding script engine
*/

#include "ScriptEngine/ScriptEngine.hpp"
#include <algorithm>
#include <SDL3/SDL.h>

ScriptEngine::ScriptEngine(float screenW, float screenH, Create create, Destroy destroy)
    : _create(std::move(create)), _destroy(std::move(destroy))
{
    _lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::table, sol::lib::string);
    registerBindings(screenW, screenH);
    registerDraw();
}

void ScriptEngine::registerComponent(const std::string &name, Builder builder)
{
    _builders[name] = std::move(builder);
}

void ScriptEngine::registerBindings(float screenW, float screenH)

{
    _lua["SCREEN_W"] = screenW;
    _lua["SCREEN_H"] = screenH;
    _lua.set_function("spawn", [this](const sol::table &d) { return spawn(d);});
    _lua.set_function("destroy", [this](size_t e) {
        if (!_destroy) {
            SDL_Log("[lua] destroy: no drestroy function");
            return;
        }
        _destroy(e);
    });
    _lua.set_function("log", [](const std::string &s) { SDL_Log("[lua] %s", s.c_str()); });
    _lua.set_function("on_update", [this](sol::protected_function func) {_callbacks.push_back(func);});

    _lua.set_function("register_monster", [this](const std::string &name, sol::protected_function fn) {
        _monsters[name] = fn;
    });
}

sol::optional<size_t> ScriptEngine::spawn(const sol::table &def) 
{
    if (!_create) {
        SDL_Log("[lua] spawn: no create function");
        return sol::nullopt;
    }
    std::string name = def.get_or<std::string>("name", "luaEntity");
    size_t ent = _create(name);

    for (auto &[k, v] : def) {
        if (!k.is<std::string>()) {
            SDL_Log("[lua] spawn: Key ignore (not a stirng)");
            continue;
        }
        std::string key = k.as<std::string>();
        if (key == "name")
            continue;
        if (!v.is<sol::table>()) {
            SDL_Log("[lua] spawn: '%s' nedeed to be a table", key.c_str());
            continue;
        }
        auto it = _builders.find(key);
        if (it == _builders.end()) {
            SDL_Log("[lua] unknown component: %s", key.c_str());
            continue;
        }
        it->second(ent, v.as<sol::table>());
    }
    return ent;
}

void ScriptEngine::loadDir(const std::filesystem::path &dir) 
{
    std::vector<std::filesystem::path> files;

    if (!std::filesystem::exists(dir)) {
        SDL_Log("[lua] folder unfoundable: %s", dir.string().c_str());
        return;
    }

    for (auto &e : std::filesystem::recursive_directory_iterator(dir))
        if (e.path().extension() == ".lua")
            files.push_back(e.path());
    
    std::sort(files.begin(), files.end(), [](const auto &a, const auto &b) {
        if (a.filename() != b.filename())
            return a.filename() < b.filename();
        return a < b;
    });
    for (auto &f : files) {
        auto r = _lua.safe_script_file(f.string(), sol::script_pass_on_error);
        if (!r.valid()) {
            sol::error err = r;
            SDL_Log("[lua] %s: %s", f.string().c_str(), err.what());
        } else {
            SDL_Log("[lua] loading: %s", f.filename().string().c_str());
        }
    }
}


void ScriptEngine::update(float dt) 
{
    for (auto &cb : _callbacks) {
        auto r = cb(dt);
        if (!r.valid()) {
            sol::error err = r;
            SDL_Log("[lua] update: %s", err.what());
        }
    }
}

void ScriptEngine::registerDraw() 
{
    _lua.set_function("on_draw", [this](sol::protected_function func, sol::optional<int> layer) {
        _drawCallbacks.push_back({layer.value_or(-100), func});
        std::stable_sort(_drawCallbacks.begin(), _drawCallbacks.end(),
                         [](const DrawCb &a, const DrawCb &b) {return a.layer < b.layer;});
    });
    _lua.set_function("set_color", [this](int r, int g, int b, sol::optional<int> a) {
        if (_ren)
            SDL_SetRenderDrawColor(_ren, r, g, b, a.value_or(255));
    });
    _lua.set_function("draw_point", [this](float x, float y) {
        if (_ren) 
            SDL_RenderPoint(_ren, x, y);
    });
    _lua.set_function("draw_line", [this](float x1, float y1, float x2, float y2) {
        if (_ren) 
            SDL_RenderLine(_ren, x1, y1, x2, y2);
    });
    _lua.set_function("fill_rect", [this](float x, float y, float w, float h) {
        if (!_ren)
            return;
        SDL_FRect rect{x, y, w, h};
        SDL_RenderFillRect(_ren, &rect);
    });
}

void ScriptEngine::draw(SDL_Renderer *ren, const std::function<void()> &entities) 
{
    size_t i = 0;

    _ren = ren;
    auto run = [&](DrawCb &cb) {
        auto r = cb.fn();
        if (!r.valid()) {
            sol::error err = r;
            SDL_Log("[lua] draw: %s", err.what());
        }
    };
    for (; i < _drawCallbacks.size() && _drawCallbacks[i].layer < 0; ++i)
        run(_drawCallbacks[i]);
    entities();
    for (; i < _drawCallbacks.size(); ++i)
        run(_drawCallbacks[i]);
    _ren = nullptr;
}


void  ScriptEngine::fireWeapon(const std::string &name, float x, float y)
{
    sol::table weapons = _lua["WEAPONS"];
    sol::optional<sol::table> weapon = weapons[name];

    if (!weapon){
        SDL_Log("[lua] unknown weapon %s", name.c_str());
        return;
    }
    sol::protected_function fire = (*weapon)["fire"];
    auto r = fire(x, y);
    if (!r.valid()) {
        sol::error err = r;
        SDL_Log("[Lua] fireWeapon: %s", err.what());
    }
}

float ScriptEngine::weaponInterval(const std::string &name)
{
    sol::table weapons = _lua["WEAPONS"];
    sol::optional<sol::table> weapon = weapons[name];

    if (!weapon){
        SDL_Log("[lua] unknown weapon %s", name.c_str());
        return _fallback;
    }
    return weapon->get_or("interval", _fallback);
}

void ScriptEngine::spawnMonster(const std::string &name, float x, float y)
{
    SDL_Log("[lua] spawnMonster(%s, %f, %f)", name.c_str(), x, y);
    auto it = _monsters.find(name);
    if (it == _monsters.end()) {
        SDL_Log("[lua] unknown monster %s", name.c_str());
        for (const auto &[k, v] : _monsters)
            SDL_Log("[lua]   registered: '%s'", k.c_str());
        return;
    }

    auto r = it->second(x, y);
    if (!r.valid()) {
        sol::error err = r;
        SDL_Log("[lua] spawnMonster %s: %s", name.c_str(), err.what());
    }
}
