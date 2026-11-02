/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** lua binding script engine 
*/

#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL.h>
#include <sol/sol.hpp>

class ScriptEngine {
public:
    using Create = std::function<size_t(const std::string &)>;
    using Destroy = std::function<void(size_t)>;
    using Builder = std::function<void(size_t, const sol::table &)>;

    ScriptEngine(float screenW, float screenH, Create create, Destroy destroy);

    void registerComponent(const std::string &name, Builder builder);

    void loadDir(const std::filesystem::path &dir);
    void update(float dt);
    
    void draw(SDL_Renderer *ren, const std::function<void()> &entities);

    void  fireWeapon(const std::string &name, float x, float y);
    float weaponInterval(const std::string &name);

private:
    void registerBindings(float screenW, float screenH);
    void registerDraw();
    sol::optional<size_t> spawn(const sol::table &def);

    Create _create;
    Destroy _destroy;
    sol::state _lua;
    std::unordered_map<std::string, Builder> _builders;
    std::vector<sol::protected_function> _callbacks;
    struct DrawCb {int layer; sol::protected_function fn;};
    std::vector<DrawCb> _drawCallbacks;
    SDL_Renderer *_ren = nullptr;
    float _fallback = 0.2f;
};
