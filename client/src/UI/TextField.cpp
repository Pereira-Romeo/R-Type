/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** TextField
*/

#include "../../include/TextField.hpp"
#include "../../include/Text.hpp"
#include <cctype>

namespace client
{
    static constexpr float TEXT_SCALE = 3.0f;
    static constexpr float PADDING = 16.0f;

    TextField::TextField(SDL_Window *window, const SDL_FRect &rect, std::size_t maxLength,
        const std::string &placeholder)
        : _window(window), _rect(rect), _maxLength(maxLength), _placeholder(placeholder)
    {
    }

    bool TextField::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            SDL_FPoint mouse = {event.button.x, event.button.y};
            if (SDL_PointInRectFloat(&mouse, &_rect)) {
                focus();
                return true;
            }
            unfocus();
            return false;
        }

        if (!_focused)
            return false;

        if (event.type == SDL_EVENT_TEXT_INPUT) {
            appendText(event.text.text);
            return true;
        }

        if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
                case SDLK_BACKSPACE:
                    if (!_text.empty())
                        _text.pop_back();
                    return true;
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    if (_onSubmit)
                        _onSubmit();
                    return true;
                default:
                    break;
            }
        }
        return false;
    }

    void TextField::render(SDL_Renderer *renderer) const
    {
        const float charSize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * TEXT_SCALE;
        const float textX = _rect.x + PADDING;
        const float textY = _rect.y + (_rect.h - charSize) / 2.0f;

        SDL_SetRenderDrawColor(renderer, 20, 20, 45, 255);
        SDL_RenderFillRect(renderer, &_rect);

        if (_focused)
            SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        else
            SDL_SetRenderDrawColor(renderer, 120, 120, 150, 255);
        SDL_RenderRect(renderer, &_rect);

        if (_text.empty() && !_focused) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 130, 255);
            drawText(renderer, _placeholder, textX, textY, TEXT_SCALE);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            drawText(renderer, _text, textX, textY, TEXT_SCALE);
        }

        // Curseur qui clignote toutes les 500 ms
        if (_focused && (SDL_GetTicks() / 500) % 2 == 0) {
            SDL_FRect cursor = {textX + _text.size() * charSize, textY, 3.0f, charSize};
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &cursor);
        }
    }

    void TextField::focus()
    {
        if (_focused)
            return;
        _focused = true;
        SDL_StartTextInput(_window);
    }

    void TextField::unfocus()
    {
        if (!_focused)
            return;
        _focused = false;
        SDL_StopTextInput(_window);
    }

    bool TextField::isFocused() const
    {
        return _focused;
    }

    const std::string &TextField::getText() const
    {
        return _text;
    }

    void TextField::clear()
    {
        _text.clear();
    }

    void TextField::setUppercase(bool uppercase)
    {
        _uppercase = uppercase;
    }

    void TextField::setCharFilter(std::function<bool(char)> filter)
    {
        _filter = std::move(filter);
    }

    void TextField::setOnSubmit(std::function<void()> onSubmit)
    {
        _onSubmit = std::move(onSubmit);
    }

    void TextField::appendText(const char *text)
    {
        for (const char *c = text; *c && _text.size() < _maxLength; c++) {
            unsigned char ch = static_cast<unsigned char>(*c);

            // La police de debug ne gère que l'ASCII : on ignore les accents, emojis...
            if (ch >= 128)
                continue;
            char value = static_cast<char>(_uppercase ? std::toupper(ch) : ch);
            if (_filter && !_filter(value))
                continue;
            _text += value;
        }
    }
}
