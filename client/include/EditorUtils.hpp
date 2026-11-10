/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** EditorUtils
*/

#pragma once
#include "imgui.h"

const ImGuiWindowFlags FIXED_WINDOW = ImGuiWindowFlags_NoResize
    | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

// ---- Display ----
const float MIN_HEADER_H = 40.0f;
const float HEADER_RATIO = 0.1f;
const float PANEL_RATIO = 0.18f;
const float PANEL_MIN_W = 180.0f;
const float PANEL_MAX_W = 320.0f;
const float PROPS_H = 220.0f;
const float RULER_H = 24.0f;
const float WAVE_MARGIN_TOP = 20.0f;
const float WAVE_H = 160.0f;
const int SAFE_ZONE_SIZE = 250;

// ---- level ----
const float ZOOM = 0.15f;
const int MIN_WAVE_LENGTH = 500;
const int NEW_WAVE_LENGTH = 1000;
const int WAVE_GAP = 200;
const float STEP = 500.0f;
const float CAM_MIN_X = -100.0f;
const float SCROLL_SPEED = 60.0f;
const float MUSIC_COMBO_W = 160.0f;

// ---- Widgets ----
const float NAME_FIELD_W = 100.0f;
const float MONSTER_BTN_H = 40.0f;
const float COUNT_COL_X = 160.0f;
const float COUNT_FIELD_W = 100.0f;
const float WAVE_MIN_CLICK_W = 4.0f;
const int STEP_SMALL = 50;
const int STEP_BIG = 500;
const float WAVE_ROUNDING = 4.0f;
const float OUTLINE_W = 1.0f;
const float OUTLINE_W_SELECT = 3.0f;

// ---- Wave data ----
const float TEXT_PAD_X = 6.0f;
const float TEXT_NAME_Y = 4.0f;
const float TEXT_LIST_Y = 24.0f;
const float TEXT_LINE_H = 16.0f;

// ---- Colors ----
const ImU32 COL_BG = IM_COL32(25, 25, 35, 255);
const ImU32 COL_SAFE_ZONE = IM_COL32(60, 60, 70, 255);
const ImU32 COL_SAFE_TEXT = IM_COL32(200, 200, 200, 255);
const ImU32 COL_RULER_BG = IM_COL32(40, 40, 55, 255);
const ImU32 COL_RULER_LINE = IM_COL32(255, 255, 255, 25);
const ImU32 COL_RULER_TEXT = IM_COL32(180, 180, 180, 255);
const ImU32 COL_SELECTED = IM_COL32(255, 255, 0, 255);
const ImU32 COL_OUTLINE = IM_COL32(0, 0, 0, 120);
const ImU32 COL_WAVE_TEXT = IM_COL32(240, 240, 240, 255);
const ImU32 COL_BOSS = IM_COL32(220, 60, 60, 255);

const ImU32 WAVE_COLORS[] = {
    IM_COL32(70, 130, 200, 255), IM_COL32(200, 120, 70, 255),
    IM_COL32(90, 180, 110, 255), IM_COL32(170, 100, 190, 255)
};
const int WAVE_COLOR_COUNT = 4;
const char *PAYLOAD_ENEMY = "ENEMY";
