/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** ButtonList
*/

#pragma once

#include <SDL3/SDL.h>
#include <functional>
#include <string>
#include <vector>

#include "Text.hpp"

namespace client
{
    class ButtonList {
        public:
            ButtonList(float centerX, float startY, float width, float height, float spacing);

            void addButton(const std::string &label, std::function<void()> onClick);
            void reset();
            void clear();
            void setLabel(std::size_t index, const std::string &label);

            void handleEvent(const SDL_Event &event);
            void render(SDL_Renderer *renderer) const;

        private:
            struct Button {
                std::string label;
                std::function<void()> onClick;
                SDL_FRect rect;
            };

            std::vector<Button> _buttons;
            std::size_t _selected = 0;
            float _x;
            float _startY;
            float _nextY;
            float _width;
            float _height;
            float _spacing;
    };

}
