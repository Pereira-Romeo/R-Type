/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** Scene interface
*/

#pragma once

class IScene {
public:
    virtual ~IScene() = default;
    virtual void onLoad() = 0;
    virtual void onUnload() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
};