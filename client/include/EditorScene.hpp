/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** EditorScene
*/

#pragma once
#include "./ButtonList.hpp"
#include "./IScene.hpp"
#include "./SceneManager.hpp"
#include "../../jsonHandler/jsonHandler.hpp"
#include <SDL3/SDL_render.h>
#include <atomic>
#include <mutex>
#include "imgui.h"

namespace client
{
    class EditorScene : public IScene {
        public:
            EditorScene(client::SceneManager &scenes, std::string &_levelPath, float screenWidth, float screenHeight);
            void onLoad() override;
            void onUnload() override {};
            void handleEvent(const SDL_Event &event) override {};
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;
            void LoadLevel();

         private:
            client::SceneManager &_scenes;
            std::string &_levelPath;
            client::ButtonList _buttons;
            jsonHandler::levelData_t _levelData;
            std::vector<std::string> _scriptFiles;
            bool _modified = false;

            void updateLayout(SDL_Renderer *renderer);
            float _winW = 0;
            float _winH = 0;
            float _panelW = 0;
            float _headerHeight;
            ImVec2 _vpMin;
            ImVec2 _vpMax;
            float _waveTop = 0;

            void DrawHeader();
            void DrawMonsterPalette();
            void DrawProperties();

            void DrawViewport();
            void handleNavigation();
            void drawGrid(ImDrawList *dl);
            void drawWaves(ImDrawList *dl);
            void drawBoss(ImDrawList *dl);
            bool isMouseInViewport() const;
            float toScreen(float worldX) const;

            float _camX = -50.0f;
            float _zoom = 0.15f;
            int _selectedWave = -1;

            void LoadScript();
            void addWave();
            void addSpawn(int waveIndex, const std::string &script);

            bool Save(const std::string &path);
            void SaveAs();
            static void SDLCALL OnSaveSelected(void *userdata, const char * const *filelist, int filter);
            std::string _statusMessage;
            std::mutex _saveMutex;
            std::string _pendingSavePath;
            std::atomic<bool> _hasPendingSave{false};

            std::vector<std::string> _musicFiles;
            void LoadMusic();

            std::vector<std::string> _bossFiles;
            void LoadBoss();
            void scanLua(const std::string &folder, std::vector<std::string> &out);
            void DrawBossList();
            void DrawMonsterList();
    };
}

