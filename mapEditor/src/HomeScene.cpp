#include "../include/HomeScene.hpp"
#include "SceneManager.hpp"

void SwitchScene(client::SceneManager &_scenes)
{
        _scenes.switchScene("Editor");
}

editor::HomeScene::HomeScene(client::SceneManager &scenes, std::string &levelPath, 
        float screenWidth, float screenHeight)
: _scenes(scenes), _levelPath(levelPath), _buttons(screenWidth / 2.0f, screenHeight / 2.0f, 300.0f, 60.0f, 20.0f),
  _screenWidth(screenWidth)
{
    _buttons.addButton("New", [this]() { _scenes.switchScene("Editor"); });
    _buttons.addButton("Load", [this]() {
        static const SDL_DialogFileFilter filters[] = { { "Fichiers JSON", "json" } };
        SDL_ShowOpenFileDialog(OnFileSelected, this, nullptr, filters, 1, nullptr, false);
    });
}

void SDLCALL editor::HomeScene::OnFileSelected(void *userdata, const char * const *filelist, int)
{
    auto *self = static_cast<HomeScene *>(userdata);

    if (!filelist || !filelist[0])
        return;

    self->_levelPath = filelist[0];
    self->_fileChosen = true;
}

void editor::HomeScene::render(SDL_Renderer *renderer)
{
    SDL_FRect titleArea = {0.0f, 120.0f, _screenWidth, 80.0f};

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    client::drawCenteredText(renderer, "R-TYPE", titleArea, 6.0f);
    _buttons.render(renderer);
}

void editor::HomeScene::handleEvent(const SDL_Event &event)
{
    _buttons.handleEvent(event);
}


void editor::HomeScene::update(float deltaTime)
{
    if (_fileChosen) {
        _fileChosen = false;
        _scenes.switchScene("Editor");
    }
}

