/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** Starfield (background fo the gale
*/


#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <random>

class StarField
{
public:
    StarField(int width, int height, int count = 200);

    void update(float dt);
    void render(SDL_Renderer* renderer) const;

private:
    struct Star
    {
        float x, y;
        float speed;
        int size;
        Uint8 brightness;
    };

    void resetStar(Star& s, bool randomY);

    int _width, _height;
    std::vector<Star> _stars;
    std::mt19937 _rng; //(Mersenne Twister) > rand
};