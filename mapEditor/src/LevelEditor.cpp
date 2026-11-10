#include "../include/LevelEditor.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <memory>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

editor::LevelEditor::LevelEditor()
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Erreur initialisation SDL : %s", SDL_GetError());
    }

    if (!SDL_CreateWindowAndRenderer("Level Editor", 800, 600, SDL_WINDOW_RESIZABLE, &win, &renderer)) {
        SDL_Log("Erreur création fenêtre/rendu : %s", SDL_GetError());
        SDL_Quit();
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(win, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    scenes.addScene("Menu", std::make_unique<HomeScene>(scenes, levelPath, 800, 600));
    scenes.addScene("Editor", std::make_unique<EditorScene>(scenes, levelPath, 800, 600));
    scenes.switchScene("Menu");
    scenes.update(0);
}

editor::LevelEditor::~LevelEditor()
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (win)
        SDL_DestroyWindow(win);
    SDL_Quit();
}

void editor::LevelEditor::Run()
{
    while(isRunning) {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();

//ImGui::ShowDemoWindow();   // pour tester

        ImGui::NewFrame();

        EventHandler();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  
        SDL_RenderClear(renderer);
        scenes.update(0);
        scenes.render(renderer);
        
        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }
}

void editor::LevelEditor::EventHandler()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT) {
            isRunning = false;
            continue;
        }
        SDL_ConvertEventToRenderCoordinates(renderer, &event);
        scenes.handleEvent(event);
    }
}

