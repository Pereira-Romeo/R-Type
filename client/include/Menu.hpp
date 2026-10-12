/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Menu
*/

#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>

namespace client {

    enum class MenuAction {
        None,
        Play,
        Quit
    };

    class Menu {
        public:
            Menu(float screenWidth, float screenheight);

            MenuAction handleEvent(const SDL_Event &event);
            void render(SDL_Renderer *renderer) const;

        private:
            struct Button {
                std::string label;
                MenuAction action;
                SDL_FRect rect;
            };
        void drawCenteredText(SDL_Renderer *renderer, const std::string &text,
                const SDL_FRect &area, float scale) const;

        std::vector<Button> _buttons;
        std::size_t _selected = 0;
        float _screenWidth;
    };
}
