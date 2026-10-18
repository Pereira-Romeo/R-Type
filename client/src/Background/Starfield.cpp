/*
** EPITECH PROJECT, 2026
** Rtype
** File description:
** Starfield in background ame
*/

#include "Background/Starfield.hpp"

StarField::StarField(int width, int height, int count)
    : _width(width), _height(height), _rng(std::random_device{}())
{
    _stars.resize(count);
    for (auto& s : _stars)
        resetStar(s, true);
}

void StarField::resetStar(Star& s, bool randomY)
{
    s.x = SDL_randf() * _width;
    s.y = randomY ? SDL_randf() * _height : -5.f;

    // 3 couches : lointaine / moyenne / proche
    switch (SDL_rand(3)) {
        case 0:
            s.speed = 20.f;
            s.size = 1;
            s.brightness = 90;
            break;

        case 1:
            s.speed = 60.f;
            s.size = 1;
            s.brightness = 170;
            break;

        case 2:
            s.speed = 140.f;
            s.size = 2;
            s.brightness = 255;
            break;
    }
}

void StarField::update(float dt)
{
    for (auto& s : _stars) {
        s.y += s.speed * dt;
        if (s.y > _height + 5.f)
            resetStar(s, false);
    }
}

void StarField::render(SDL_Renderer* renderer) const
{
    for (const auto& s : _stars)
    {
        SDL_SetRenderDrawColor(renderer, s.brightness, s.brightness, s.brightness, 255);
        if (s.size == 1)
            SDL_RenderPoint(renderer, s.x, s.y);
        else {
            SDL_FRect r{s.x, s.y, (float)s.size, (float)s.size};
            SDL_RenderFillRect(renderer, &r);
        }
    }
}