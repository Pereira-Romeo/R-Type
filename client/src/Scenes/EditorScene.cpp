/*
** EPITECH PROJECT, 2026
** R_Type
** File description:
** EditorScene
*/

#include <string>
#include <iostream>
#include <filesystem>
#include "../../include/EditorScene.hpp"
#include "../include/EditorUtils.hpp"
#include "imgui_stdlib.h"

std::string fileName(const std::string &path)
{
    return std::filesystem::path(path).stem().string();
}

client::EditorScene::EditorScene(client::SceneManager &scenes, std::string &levelPath, float screenWidth, float screenHeight)
: _scenes(scenes), _levelPath(levelPath)
{}

void client::EditorScene::onLoad()
{
    if (_levelPath != std::string())
        LoadLevel();

    LoadScript();
    LoadBoss();
    LoadMusic();
}

void client::EditorScene::onUnload()
{
    SDL_Log("[editor] onUnload");
    _levelData = jsonHandler::levelData_t{};
    _selectedWave = -1;
    _modified = false;
    _camX = -50.0f;
    _levelPath.clear();
    _scriptFiles.clear();
    _bossFiles.clear();
    _musicFiles.clear();
}

void client::EditorScene::update(float deltaTime)
{
    (void)deltaTime;
    if (_hasPendingSave) {
        _hasPendingSave = false;
        std::string path = _pendingSavePath + ".json";
        if (Save(path))
            _levelPath = path;
    }
}

void client::EditorScene::render(SDL_Renderer *renderer)
{
    SDL_SetRenderLogicalPresentation(renderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);
    updateLayout(renderer);
    DrawHeader();
    DrawMonsterPalette();
    DrawViewport();
    DrawProperties();
}

void client::EditorScene::updateLayout(SDL_Renderer *renderer)
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

// -------- Drawing function ---------- //

void client::EditorScene::DrawHeader()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(_winW, _headerHeight));
    ImGuiWindowFlags flags = FIXED_WINDOW | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
    ImGui::Begin("##header", nullptr, flags);

    if (ImGui::Button("Back"))
        _scenes.switchScene("menueditor");
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

    ImGui::Dummy(ImVec2(0, 10.0f));
    ImGui::SetNextItemWidth(MUSIC_COMBO_W);
    std::string preview = _levelData.music.empty() ? "Music: none" : fileName(_levelData.music);

    if (ImGui::BeginCombo("##music", preview.c_str())) {
        if (ImGui::Selectable("(none)", _levelData.music.empty())) {
            _levelData.music.clear();
            _modified = true;
        }
        for (const auto &path : _musicFiles) {
            bool isSelected = (path == _levelData.music);
            if (ImGui::Selectable(fileName(path).c_str(), isSelected)) {
                _levelData.music = path;
                _modified = true;
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(MUSIC_COMBO_W);
    if (ImGui::BeginCombo("##background", preview.c_str())) {
        ImGui::EndCombo();
    }

    ImGui::End();
}

void client::EditorScene::DrawMonsterPalette()
{
    ImGui::SetNextWindowPos(ImVec2(_winW - _panelW, _headerHeight));
    ImGui::SetNextWindowSize(ImVec2(_panelW, _winH - _headerHeight));
    ImGui::Begin("Library", nullptr, FIXED_WINDOW | ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("New wave"))
        addWave();
    ImGui::SameLine();
    if (ImGui::Button("Refresh")) {
        SDL_Log("refresh %s", SDL_GetBasePath());
        LoadScript();
        LoadBoss();
        LoadMusic();
    }
    ImGui::Separator();
    if (ImGui::BeginTabBar("##library")) {
        if (ImGui::BeginTabItem("Enemies")) {
            DrawScriptList(_scriptFiles, "No enemies found");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Bosses")) {
            DrawScriptList(_bossFiles, "No boss found");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void client::EditorScene::DrawProperties()
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
        wave.start_x = std::max(wave.start_x, SAFE_ZONE_SIZE);
        _modified = true;
    }
    if (ImGui::InputInt("Length", &wave.length, STEP_SMALL, STEP_BIG)) {
        wave.length = std::max(wave.length, MIN_WAVE_LENGTH);
        _modified = true;
    }

    ImGui::Separator();
    ImGui::Text("Enemies :");

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

void client::EditorScene::DrawViewport()
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

    ImGui::End();
    dl->PopClipRect();
}

// Handle scrolling
void client::EditorScene::handleNavigation()
{
    if (!isMouseInViewport())
        return;
    ImGuiIO &io = ImGui::GetIO();
    if (io.MouseWheel != 0.0f)
        _camX -= io.MouseWheel * SCROLL_SPEED / ZOOM;
    _camX = std::max(_camX, CAM_MIN_X);
}

void client::EditorScene::drawGrid(ImDrawList *dl)
{
    dl->AddRectFilled(_vpMin, _vpMax, COL_BG);

    // spawn (no waves)
    float sx0 = toScreen(0);
    float sx1 = toScreen((float)SAFE_ZONE_SIZE);
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

void client::EditorScene::drawWaves(ImDrawList *dl)
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
            if (const ImGuiPayload *p = ImGui::AcceptDragDropPayload(PAYLOAD_ENEMY))
                addSpawn(i, (const char *)p->Data);
            ImGui::EndDragDropTarget();
        }
        ImGui::PopID();
    }
}

void client::EditorScene::DrawScriptList(const std::vector<std::string> &files, const char *emptyText)
{
    if (files.empty())
        ImGui::TextDisabled("%s", emptyText);
    for (const auto &path : files) {
        std::string name = fileName(path);
        ImGui::PushID(path.c_str());
        ImGui::Button(name.c_str(), ImVec2(-1, MONSTER_BTN_H));
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(PAYLOAD_ENEMY, path.c_str(), path.size() + 1);
            ImGui::TextUnformatted(name.c_str());
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();
    }
}

bool client::EditorScene::isMouseInViewport() const
{
    ImVec2 m = ImGui::GetIO().MousePos;
    return m.x >= _vpMin.x && m.x < _vpMax.x && m.y >= _vpMin.y && m.y < _vpMax.y;
}

float client::EditorScene::toScreen(float worldX) const
{
    return _vpMin.x + (worldX - _camX) * ZOOM;
}

bool client::EditorScene::Save(const std::string &path)
{
    if (!jsonHandler::Save(path, _levelData))
        return false;

    _modified = false;
    return true;
}

void client::EditorScene::SaveAs()
{
    SDL_DialogFileFilter filters[] = { { "JSON file", "json" } };
    SDL_ShowSaveFileDialog(OnSaveSelected, this, nullptr, filters, 1, nullptr);
}

void SDLCALL client::EditorScene::OnSaveSelected(void *userdata, const char * const *filelist, int)
{
    auto *self = static_cast<EditorScene *>(userdata);
    if (!filelist || !filelist[0])
        return;

    self->_pendingSavePath = filelist[0];
    self->_hasPendingSave = true;
}

void client::EditorScene::addWave()
{
    jsonHandler::wave_t wave;
    wave.name = "Wave " + std::to_string(_levelData.waves.size() + 1);
    wave.length = NEW_WAVE_LENGTH;

    int start = SAFE_ZONE_SIZE;
    for (const auto &w : _levelData.waves)
        start = std::max(start, w.start_x + w.length + WAVE_GAP);
    wave.start_x = start;
    _levelData.waves.push_back(std::move(wave));
    _selectedWave = (int)_levelData.waves.size() - 1;
    _modified = true;
}

void client::EditorScene::addSpawn(int waveIndex, const std::string &script)
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

//---------- Load function ---------------//

void client::EditorScene::LoadLevel()
{
    _levelData = jsonHandler::LoadLevel(_levelPath);
}

void client::EditorScene::LoadMusic()
{
    _musicFiles.clear();
    const char *base = SDL_GetBasePath();
    std::filesystem::path root = base ? base : "./";
    std::filesystem::path dir = root / "assets" / "music";

    std::error_code ec;
    if (!std::filesystem::is_directory(dir, ec))
        return;

    for (const auto &entry : std::filesystem::directory_iterator(dir, ec)) {
        std::string ext = entry.path().extension().string();
        if (entry.is_regular_file() && (ext == ".ogg" || ext == ".mp3" || ext == ".wav")) {
            std::filesystem::path rel = std::filesystem::relative(entry.path(), root, ec);
            _musicFiles.push_back(rel.generic_string());
        }
    }
    std::sort(_musicFiles.begin(), _musicFiles.end());
}

void client::EditorScene::loadLua(const std::string &folder, std::vector<std::string> &out)
{
    out.clear();
    const char *base = SDL_GetBasePath();
    std::filesystem::path root = base ? base : "./";
    std::filesystem::path dir = root / folder;
    std::error_code ec;

    if (!std::filesystem::is_directory(dir, ec)) {
        SDL_Log("[editor] dossier introuvable : %s", dir.string().c_str());
        return;
    }
    for (const auto &entry : std::filesystem::directory_iterator(dir, ec)) {
        if (entry.is_regular_file() && entry.path().extension() == ".lua") {
            std::filesystem::path rel = std::filesystem::relative(entry.path(), root, ec);
            out.push_back(rel.generic_string());
        }
    }
    std::sort(out.begin(), out.end());
}

void client::EditorScene::LoadScript()
{
    loadLua("assets/scripts/enemy", _scriptFiles);
}

void client::EditorScene::LoadBoss()
{
    loadLua("assets/scripts/boss",  _bossFiles);
}
