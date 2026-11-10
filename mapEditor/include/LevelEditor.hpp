#pragma once
#include <SDL3/SDL_events.h>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <fstream>
#include "HomeScene.hpp"
#include "EditorScene.hpp"

namespace editor {
    enum Menu {
        HOME,
        CREATION,
        SETTINGLEVEL,
    };

    class LevelEditor {
        private:
            nlohmann::json levelJson;
            std::ifstream currPath;
            bool isSave = false;
            bool isRunning = true;
            SDL_Window *win = nullptr;
            SDL_Renderer *renderer = nullptr;
            Menu currMenu = HOME;
            client::SceneManager scenes;
            std::string levelPath;

            void EventHandler();
            void SwitchMenu(Menu idMenu);

        public:
            LevelEditor();
            ~LevelEditor();
            void Run();
    };
}

