/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** Menu
*/

#include "../../include/Menu.hpp"

namespace client
{
    Menu::Menu(float screenWidth, float screenHeight) : _screenWidth(screenWidth)
    {
        const float width = 300.0f;
        const float height = 60.0f;
        const float spacing = 20.0f;
        const float x = (screenWidth - width) / 2.0f;
        float y = screenHeight / 2.0f;

        _buttons.push_back({"PLAY", MenuAction::Play, {x, y, width, height}});
        y += height + spacing;
        _buttons.push_back({"QUIT", MenuAction::Quit, {x, y, width, height}});
    }

    MenuAction Menu::handleEvent(const SDL_Event &event)
    {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
                case SDLK_UP:
                    _selected = (_selected + _buttons.size() - 1) % _buttons.size();
                    break;
                case SDLK_DOWN:
                    _selected = (_selected + 1) % _buttons.size();
                    break;
                case SDLK_RETURN:
                    return _buttons[_selected].action;
                default:
                    break;
            }
        }

        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            SDL_FPoint mouse = {event.motion.x, event.motion.y};
            for (std::size_t i = 0; i < _buttons.size(); i++)
                if (SDL_PointInRectFloat(&mouse, &_buttons[i].rect))
                    _selected = i;
        }

        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            SDL_FPoint mouse = {event.button.x, event.button.y};
            for (const auto &button : _buttons)
                if (SDL_PointInRectFloat(&mouse, &button.rect))
                    return button.action;
        }

        return MenuAction::None;
    }

    void Menu::render(SDL_Renderer *renderer) const
    {
        SDL_FRect titleArea = {0.0f, 120.0f, _screenWidth, 80.0f};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawCenteredText(renderer, "R-TYPE", titleArea, 6.0f);

        for (std::size_t i = 0; i < _buttons.size(); i++) {
            const Button &button = _buttons[i];
            bool selected = (i == _selected);

            if (selected)
                SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
            else
                SDL_SetRenderDrawColor(renderer, 40, 40, 70, 255);
            SDL_RenderFillRect(renderer, &button.rect);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &button.rect);

            if (selected)
                SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
            drawCenteredText(renderer, button.label, button.rect, 3.0f);
        }
    }

    void Menu::drawCenteredText(SDL_Renderer *renderer, const std::string &text,
        const SDL_FRect &area, float scale) const
    {
        const float charSize = SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * scale;
        float x = area.x + (area.w - text.size() * charSize) / 2.0f;
        float y = area.y + (area.h - charSize) / 2.0f;

        SDL_SetRenderScale(renderer, scale, scale);
        SDL_RenderDebugText(renderer, x / scale, y / scale, text.c_str());
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);
    }
}
