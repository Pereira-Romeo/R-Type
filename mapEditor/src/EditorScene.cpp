/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** EditorScene
*/

#include "../include/EditorScene.hpp"
#include <string>
#include "imgui_stdlib.h"
#include <iostream>
#include <filesystem>
#include "../include/EditorUtils.hpp"

std::string fileName(const std::string &path)
{
    return std::filesystem::path(path).stem().string();
}

editor::EditorScene::EditorScene(client::SceneManager &scenes, std::string &levelPath, float screenWidth, float screenHeight)
: _scenes(scenes), _levelPath(levelPath), _buttons(screenWidth / 2.0f, screenHeight / 2.0f, 300.0f, 60.0f, 20.0f)
{}

void editor::EditorScene::LoadLevel()
{
    _levelData = jsonHandler::LoadLevel(_levelPath);
}

void editor::EditorScene::onLoad()
{
    if (_levelPath != std::string())
        LoadLevel();
    LoadScript();
}

void editor::EditorScene::update(float deltaTime)
{
    (void)deltaTime;
    if (_hasPendingSave) {
        _hasPendingSave = false;
        std::string path = _pendingSavePath + ".json";
        if (Save(path))
            _levelPath = path;
    }
}

void editor::EditorScene::render(SDL_Renderer *renderer)
{
    updateLayout(renderer);
    _buttons.render(renderer);
    DrawHeader();
    DrawMonsterPalette();
    DrawViewport();
    DrawProperties();
}

void editor::EditorScene::updateLayout(SDL_Renderer *renderer)
{
    int w = 0;
    int h = 0;
    SDL_GetCurrentRenderOutputSize(renderer, &w, &h);

    _winW = (float)w;
    _winH = (float)h;
    _headerHeight = std::max(_winH * HEADER_RATIO, MIN_HEADER_H);
    _panelW = std::clamp(_winW * PANEL_RATIO, PANEL_MIN_W, PANEL_MAX_W);
    _vpMin = ImVec2(0, _headerHeight);
    _vpMax = ImVec2(_winW - _panelW, _winH);
    _waveTop = _vpMin.y + RULER_H + WAVE_MARGIN_TOP;
}

/* ========================= Panneaux ========================= */

void editor::EditorScene::DrawHeader()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(_winW, _headerHeight));
    ImGuiWindowFlags flags = FIXED_WINDOW | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
    ImGui::Begin("##header", nullptr, flags);

    if (ImGui::Button("Back"))
        _scenes.switchScene("Menu");
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        if (_levelPath.empty())
            SaveAs();
        else
            Save(_levelPath);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save as"))
        SaveAs();
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100.0f);
    if (ImGui::InputText("##levelname", &_levelData.name))
        _modified = true;
    if (_modified) {
        ImGui::SameLine();
        ImGui::TextUnformatted("(Unsaved)");
    }
    ImGui::End();
}

void editor::EditorScene::DrawMonsterPalette()
{
    ImGui::SetNextWindowPos(ImVec2(_winW - _panelW, _headerHeight));
    ImGui::SetNextWindowSize(ImVec2(_panelW, _winH - _headerHeight));
    ImGui::Begin("Monsters", nullptr, FIXED_WINDOW | ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("New wave"))
        addWave();
    ImGui::SameLine();
    if (ImGui::Button("Refresh"))
        LoadScript();
    ImGui::Separator();
    if (_scriptFiles.empty())
        ImGui::TextDisabled("No script found");

    for (const auto &path : _scriptFiles) {
        std::string name = fileName(path);

        // monster case
        ImGui::PushID(path.c_str());
        ImGui::Button(name.c_str(), ImVec2(-1, MONSTER_BTN_H));

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(PAYLOAD_MONSTER, path.c_str(), path.size() + 1);
            ImGui::TextUnformatted(name.c_str());
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();
    }
    ImGui::End();
}

void editor::EditorScene::DrawProperties()
{
    if (_selectedWave < 0 || _selectedWave >= (int)_levelData.waves.size())
        return;

    ImGui::SetNextWindowPos(ImVec2(0, _winH - PROPS_H));
    ImGui::SetNextWindowSize(ImVec2(_winW - _panelW, PROPS_H));
    ImGui::Begin("Wave properties", nullptr, FIXED_WINDOW | ImGuiWindowFlags_NoCollapse);

    jsonHandler::wave_t &wave = _levelData.waves[_selectedWave];

    if (ImGui::InputText("Name", &wave.name))
        _modified = true;

    if (ImGui::InputInt("Start", &wave.start_x, STEP_SMALL, STEP_BIG)) {
        wave.start_x = std::max(wave.start_x, _safeZone);
        _modified = true;
    }
    if (ImGui::InputInt("Length", &wave.length, STEP_SMALL, STEP_BIG)) {
        wave.length = std::max(wave.length, MIN_WAVE_LENGTH);
        _modified = true;
    }

    ImGui::Separator();
    ImGui::Text("Monsters :");

    int toRemove = -1;
    for (size_t i = 0; i < wave.spawns.size(); ++i) {
        auto &spawn = wave.spawns[i];
        ImGui::PushID((int)i);

        ImGui::TextUnformatted(fileName(spawn.script).c_str());
        ImGui::SameLine(COUNT_COL_X);
        ImGui::SetNextItemWidth(COUNT_FIELD_W);
        if (ImGui::InputInt("##count", &spawn.count)) {
            spawn.count = std::max(spawn.count, 1);
            _modified = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("X"))
            toRemove = (int)i;

        ImGui::PopID();
    }
    if (toRemove >= 0) {
        wave.spawns.erase(wave.spawns.begin() + toRemove);
        _modified = true;
    }

    ImGui::Separator();
    if (ImGui::Button("Delete wave")) {
        _levelData.waves.erase(_levelData.waves.begin() + _selectedWave);
        _selectedWave = -1;
        _modified = true;
    }

    ImGui::End();
}

/* ========================= Viewport ========================= */

void editor::EditorScene::DrawViewport()
{
    handleNavigation();
    ImDrawList *dl = ImGui::GetBackgroundDrawList();
    dl->PushClipRect(_vpMin, _vpMax, true);
    drawGrid(dl);

    ImGui::SetNextWindowPos(_vpMin);
    ImGui::SetNextWindowSize(ImVec2(_vpMax.x - _vpMin.x, _vpMax.y - _vpMin.y));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollWithMouse
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing
        | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("##viewportDrop", nullptr, flags);

    if (isMouseInViewport() && ImGui::IsWindowHovered()
        && !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        _selectedWave = -1;

    drawWaves(dl);
    drawBoss(dl);

    ImGui::End();
    dl->PopClipRect();
}

// Handle scrolling
void editor::EditorScene::handleNavigation()
{
    if (!isMouseInViewport())
        return;
    ImGuiIO &io = ImGui::GetIO();
    if (io.MouseWheel != 0.0f)
        _camX -= io.MouseWheel * SCROLL_SPEED / ZOOM;
    _camX = std::max(_camX, CAM_MIN_X);
}

void editor::EditorScene::drawGrid(ImDrawList *dl)
{
    dl->AddRectFilled(_vpMin, _vpMax, COL_BG);

    // spawn (no ennemies)
    float sx0 = toScreen(0);
    float sx1 = toScreen((float)_safeZone);
    dl->AddRectFilled(ImVec2(sx0, _vpMin.y), ImVec2(sx1, _vpMax.y), COL_SAFE_ZONE);
    dl->AddText(ImVec2(sx0 + TEXT_PAD_X, _vpMin.y + 30), COL_SAFE_TEXT, "Spawn");

    dl->AddRectFilled(_vpMin, ImVec2(_vpMax.x, _vpMin.y + RULER_H), COL_RULER_BG);
    float worldRight = _camX + (_vpMax.x - _vpMin.x) / ZOOM;
    for (float x = std::floor(_camX / STEP) * STEP; x <= worldRight; x += STEP) {
        float px = toScreen(x);
        std::string label = std::to_string((int)x);
        dl->AddLine(ImVec2(px, _vpMin.y), ImVec2(px, _vpMax.y), COL_RULER_LINE);
        dl->AddText(ImVec2(px + 3, _vpMin.y + 4), COL_RULER_TEXT, label.c_str());
    }
}

void editor::EditorScene::drawWaves(ImDrawList *dl)
{
    for (int i = 0; i < (int)_levelData.waves.size(); i++) {
        const jsonHandler::wave_t &wave = _levelData.waves[i];
        float x0 = toScreen((float)wave.start_x);
        float x1 = toScreen((float)(wave.start_x + wave.length));
        ImVec2 vectMin(x0, _waveTop);
        ImVec2 vectMax(x1, _waveTop + WAVE_H);
        bool selected = (i == _selectedWave);

        dl->AddRectFilled(vectMin, vectMax, WAVE_COLORS[i % WAVE_COLOR_COUNT], WAVE_ROUNDING);
        dl->AddRect(vectMin, vectMax, selected ? COL_SELECTED : COL_OUTLINE, WAVE_ROUNDING, 0, selected ? 3.0f : 1.0f);

        dl->PushClipRect(vectMin, vectMax, true);
        dl->AddText(ImVec2(x0 + TEXT_PAD_X, _waveTop + TEXT_NAME_Y), IM_COL32_WHITE, wave.name.c_str());
        float textY = _waveTop + TEXT_LIST_Y;
        for (const auto &spawn : wave.spawns) {
            std::string line = fileName(spawn.script) + " x" + std::to_string(spawn.count);
            dl->AddText(ImVec2(x0 + TEXT_PAD_X, textY), COL_WAVE_TEXT, line.c_str());
            textY += TEXT_LINE_H;
        }
        dl->PopClipRect();

        ImGui::SetCursorScreenPos(vectMin);
        ImGui::PushID(i);
        ImGui::InvisibleButton("wave", ImVec2(x1 - x0, WAVE_H));
        if (ImGui::IsItemClicked())
            _selectedWave = i;
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *p = ImGui::AcceptDragDropPayload(PAYLOAD_MONSTER))
                addSpawn(i, (const char *)p->Data);
            ImGui::EndDragDropTarget();
        }
        ImGui::PopID();
    }
}

void editor::EditorScene::drawBoss(ImDrawList *dl)
{
    if (_levelData.boss.trigger_x <= 0)
        return;

    float bx = toScreen((float)_levelData.boss.trigger_x);
    dl->AddLine(ImVec2(bx, _waveTop), ImVec2(bx, _waveTop + WAVE_H), COL_BOSS, 3.0f);
    dl->AddText(ImVec2(bx + TEXT_PAD_X, _waveTop), COL_BOSS, "Boss");
}

bool editor::EditorScene::isMouseInViewport() const
{
    ImVec2 m = ImGui::GetIO().MousePos;
    return m.x >= _vpMin.x && m.x < _vpMax.x && m.y >= _vpMin.y && m.y < _vpMax.y;
}

float editor::EditorScene::toScreen(float worldX) const
{
    return _vpMin.x + (worldX - _camX) * ZOOM;
}

bool editor::EditorScene::Save(const std::string &path)
{
    std::ofstream file(path);
    if (!file.is_open()) {
        _statusMessage = "Error : can't open";
        return false;
    }

    nlohmann::json out = _levelData;
    file << out.dump(4);

    _modified = false;
    _statusMessage = "Save";
    return true;
}


void editor::EditorScene::SaveAs()
{
    SDL_DialogFileFilter filters[] = { { "JSON file", "json" } };
    SDL_ShowSaveFileDialog(OnSaveSelected, this, nullptr, filters, 1, nullptr);
}

void SDLCALL editor::EditorScene::OnSaveSelected(void *userdata, const char * const *filelist, int)
{
    auto *self = static_cast<EditorScene *>(userdata);
    if (!filelist || !filelist[0])
        return;

    self->_pendingSavePath = filelist[0];
    self->_hasPendingSave = true;
}

void editor::EditorScene::LoadScript()
{
    _scriptFiles.clear();
    std::filesystem::path dir = "./content/scripts";
    for (const auto &entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".lua")
            _scriptFiles.push_back(entry.path());
    }
}

void editor::EditorScene::addWave()
{
    jsonHandler::wave_t wave;
    wave.name = "Wave " + std::to_string(_levelData.waves.size() + 1);
    wave.length = NEW_WAVE_LENGTH;

    int start = (int)_safeZone;
    for (const auto &w : _levelData.waves)
        start = std::max(start, w.start_x + w.length + WAVE_GAP);
    wave.start_x = start;
    _levelData.waves.push_back(std::move(wave));
    _selectedWave = (int)_levelData.waves.size() - 1;
    _modified = true;
}

void editor::EditorScene::addSpawn(int waveIndex, const std::string &script)
{
    if (waveIndex < 0 || waveIndex >= (int)_levelData.waves.size())
        return;

    auto &spawns = _levelData.waves[waveIndex].spawns;
    for (auto &s : spawns) {
        if (s.script == script) {
            s.count++;
            _modified = true;
            return;
        }
    }

    jsonHandler::spawn_t s;
    s.script = script;
    s.count = 1;
    spawns.push_back(std::move(s));
    _modified = true;
}
