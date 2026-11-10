/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** HomeEditorMenu
*/

#pragma once 
#include "../../client/include/IScene.hpp"
#include "../../client/include/SceneManager.hpp"
#include "../../client/include/ButtonList.hpp"
#include <atomic>

namespace client {

    class HomeEditorMenu : public client::IScene {
        private:
            client::SceneManager &_scenes;
            std::string &_levelPath;
            client::ButtonList _buttons;
            float _screenWidth;
            float _screenHeight;
            std::atomic<bool> _fileChosen{false};

            /**
            * Stores the selected file path in the scene and raises a flag so that the
            * main thread can process it on the next frame. This function may be called
            * from another thread, so it must not touch the UI or the scene manager.
            *
            * @param userdata Pointer given to SDL_ShowOpenFileDialog (the owning scene, cast back with static_cast).
            * @param filelist NULL-terminated array of selected file paths.
            * @param filter   Index of the file filter selected in the dialog, or -1 if unknown (unused).
            */
            static void SDLCALL OnFileSelected(void *userdata, const char * const *filelist, int filter);

        public:
            HomeEditorMenu(client::SceneManager &scenes, std::string &levelPath, float screenWidth, float screenHeight);
            void onLoad() override {};
            void onUnload() override {};
            void handleEvent(const SDL_Event &event) override;
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;
    };
}

