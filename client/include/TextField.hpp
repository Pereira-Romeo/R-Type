/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** TextField
*/

#pragma once

#include <SDL3/SDL.h>
#include <functional>
#include <string>

namespace client
{
    class TextField {
        public:
            TextField(SDL_Window *window, const SDL_FRect &rect, std::size_t maxLength,
                const std::string &placeholder);

            // Renvoie true si l'événement a été consommé par le champ
            bool handleEvent(const SDL_Event &event);
            void render(SDL_Renderer *renderer) const;

            void focus();
            void unfocus();
            bool isFocused() const;

            const std::string &getText() const;
            void clear();

            void setUppercase(bool uppercase);
            void setCharFilter(std::function<bool(char)> filter);
            void setOnSubmit(std::function<void()> onSubmit);

        private:
            void appendText(const char *text);

            SDL_Window *_window;
            SDL_FRect _rect;
            std::size_t _maxLength;
            std::string _placeholder;
            std::string _text;
            bool _focused = false;
            bool _uppercase = false;
            std::function<bool(char)> _filter;
            std::function<void()> _onSubmit;
    };
}
