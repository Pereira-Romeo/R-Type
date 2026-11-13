/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** EditorScene
*/

#pragma once
#include "./IScene.hpp"
#include "./SceneManager.hpp"
#include "../../jsonHandler/jsonHandler.hpp"
#include <SDL3/SDL_render.h>
#include "imgui.h"

namespace client
{
    class EditorScene : public IScene {
        public:
            EditorScene(client::SceneManager &scenes, std::string &_levelPath, float screenWidth, float screenHeight);
            void onLoad() override;
            void onUnload() override;
            void handleEvent(const SDL_Event &event) override {};
            void update(float deltaTime) override;
            void render(SDL_Renderer *renderer) override;

        private:
            client::SceneManager &_scenes;
            std::string &_levelPath;
            jsonHandler::levelData_t _levelData;
            std::vector<std::string> _scriptFiles;
            bool _modified = false;

            float _winW = 0;
            float _winH = 0;
            float _panelW = 0;
            float _headerHeight = 0;
            ImVec2 _vpMin;
            ImVec2 _vpMax;
            float _waveTop = 0;

            float _camX = -50.0f;
            int _selectedWave = -1;

            std::string _pendingSavePath;
            bool _hasPendingSave = false;
            std::vector<std::string> _musicFiles;
            std::vector<std::string> _bossFiles;

            /**
             * @brief Recomputes the size and position of every UI zone.
             * @param renderer Renderer used to read the current window size.
             */
            void updateLayout(SDL_Renderer *renderer);

            /**
             * @brief Draws a list of draggable script buttons (one per script).
             * @param files Relative paths of the scripts to display.
             * @param emptyText Text shown in grey when @p files is empty.
             */
            void DrawScriptList(const std::vector<std::string> &files, const char *emptyText);

            /** @brief Draws the top bar (Back, Save, name, music). */
            void DrawHeader();

            /** @brief Draws the right-hand library panel (enemies and bosses tabs). */
            void DrawMonsterPalette();

            /** @brief Draws the properties panel of the selected wave. */
            void DrawProperties();

            /** @brief Draws the central viewport (grid, waves, click and drop zones). */
            void DrawViewport();

            /**
             * @brief Draws the background, the safe zone and the ruler.
             * @param dl ImGui draw list to draw into.
             */
            void drawGrid(ImDrawList *dl);

            /**
             * @brief Draws every wave and makes it clickable and a drop target.
             * @param dl ImGui draw list to draw into.
             */
            void drawWaves(ImDrawList *dl);

            /** @brief Scrolls the viewport horizontally with the mouse wheel. */
            void handleNavigation();

            /**
             * @brief Tells whether the mouse cursor is inside the viewport.
             * @return True if the cursor is inside.
             */
            bool isMouseInViewport() const;

            /**
             * @brief Converts a level position into a screen X coordinate.
             * @param worldX Position in level units.
             * @return Position in pixels.
             */
            float toScreen(float worldX) const;

            /** @brief Load the json level in _levelData struct */
            void LoadLevel();

            /** @brief Load the enemy scripts folder. */
            void LoadScript();

            /** @brief Load the boss scripts folder. */
            void LoadBoss();

            /** @brief Load the music folder. */
            void LoadMusic();

            /**
             * @brief Lists the .lua files of a folder as paths relative to the executable.
             * @param folder Folder to scan, relative to the executable.
             * @param out Vector filled with the relative paths (cleared first).
             */
            void loadLua(const std::string &folder, std::vector<std::string> &out);

            /** @brief Adds a new wave after the last one, outside the safe zone. */
            void addWave();

            /**
             * @brief Adds a script to a wave, or increments its count if already present.
             * @param waveIndex Index of the target wave.
             * @param script Relative path of the script to add.
             */
            void addSpawn(int waveIndex, const std::string &script);

            /**
             * @brief Writes the level to a file.
             * @param path Destination file path.
             * @return True on success.
             */
            bool Save(const std::string &path);

            /** @brief Opens the "Save as" dialog. */
            void SaveAs();

            /**
             * @brief SDL callback invoked when the save dialog closes.
             * @param userdata Pointer to the owning EditorScene.
             * @param filelist NULL-terminated list of chosen paths (null on error, empty on cancel).
             * @param filter Index of the selected file filter (unused).
             */
            static void SDLCALL OnSaveSelected(void *userdata, const char * const *filelist, int filter);
    };
}
