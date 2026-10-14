/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** ButtonList
*/

#include "../../include/ButtonList.hpp"

namespace client
{
    ButtonList::ButtonList(float centerX, float startY, float width, float height, float spacing)
        : _x(centerX - width / 2.0f), _startY(startY), _nextY(startY), _width(width), _height(height), _spacing(spacing)
    {
    }

    void ButtonList::addButton(const std::string &label, std::function<void()> onClick)
    {
        _buttons.push_back({label, std::move(onClick), {_x, _nextY, _width, _height}});
        _nextY += _height + _spacing;
    }

    void ButtonList::reset()
    {
        _selected = 0;
    }

    void ButtonList::clear()
    {
        _buttons.clear();
        _selected = 0;
        _nextY = _startY;
    }

    void ButtonList::setLabel(std::size_t index, const std::string &label)
    {
        if (index < _buttons.size())
            _buttons[index].label = label;
    }

    void ButtonList::handleEvent(const SDL_Event &event)
    {
        if (_buttons.empty())
            return;

        if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
                case SDLK_UP:
                    _selected = (_selected + _buttons.size() - 1) % _buttons.size();
                    break;
                case SDLK_DOWN:
                    _selected = (_selected + 1) % _buttons.size();
                    break;
                case SDLK_RETURN:
                    _buttons[_selected].onClick();
                    break;
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
            for (const auto &button : _buttons) {
                if (SDL_PointInRectFloat(&mouse, &button.rect)) {
                    button.onClick();
                    break;
                }
            }
        }
    }

    void ButtonList::render(SDL_Renderer *renderer) const
    {
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

}
