/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** TextureManager
*/

#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <unordered_map>

namespace client {

    class TextureManager {

        public:
            TextureManager() = default;
            TextureManager(const TextureManager &) = delete;
            TextureManager &operator=(const TextureManager &) = delete;
            ~TextureManager() { clear(); }

            SDL_Texture *get(SDL_Renderer *renderer, const std::string &path)
            {
                if (auto it = _textures.find(path); it != _textures.end())
                    return it->second;

                std::string full = std::string(SDL_GetBasePath()) + path;
                SDL_Texture *tex = IMG_LoadTexture(renderer, full.c_str());
                if (!tex)
                    SDL_Log("Texture %s : %s", full.c_str(), SDL_GetError());
                else
                    SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
                _textures[path] = tex;
                return tex;
            }

            void clear()
            {
                for (auto &[path, tex] : _textures)
                    if (tex) SDL_DestroyTexture(tex);
                _textures.clear();
            }

        private:
            std::unordered_map<std::string, SDL_Texture *> _textures;
        };
}