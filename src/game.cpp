#include <SDL2/SDL.h>
#include <iostream>

#include "game.h"
#include "graphics.h"
#include "input.h"
#include "hud.h"
#include "singleplayer.h"

namespace{
    const int FPS_TARGET = 60;
    const int MAX_FRAME_TIME = 1000 / FPS_TARGET;

    unsigned int frameCount = 0;
    float currentFPS = 0.0f;
}

Game::Game() : _graphics(), _hud(_graphics), _singleplayer(nullptr), _player(nullptr) {
    this->gameLoop();
}

Game::~Game(){
    if(this->_singleplayer != nullptr){
        delete this->_singleplayer;
        this->_singleplayer = nullptr;
    }

    if(this->_player != nullptr){
        delete this->_player;
        this->_player = nullptr;
    }
}

void Game::gameLoop() {
    Input input;
    SDL_Event e;

    Uint8 menuIndex = 0;

    Uint64 lastUpdateTime = SDL_GetTicks64();
    Uint64 lastFpsUpdateTime = lastUpdateTime;

    while (true) {
        input.beginNewFrame();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.repeat == 0) {
                    input.keyDownEvent(e);
                }
            } else if (e.type == SDL_KEYUP) {
                input.keyUpEvent(e);
            } else if (e.type == SDL_QUIT) {
                return;
            }
        }

        if (input.wasKeyPressed(SDL_SCANCODE_S) && menuIndex == 0){
            menuIndex = 1;
            if(this->_singleplayer == nullptr){
                this->_player = new Player(this->_graphics, Vector2f(100, 100));
                this->_singleplayer = new Singleplayer(this->_graphics, this->_player);
            }
        }
        
        if (input.wasKeyPressed(SDL_SCANCODE_M) && menuIndex == 0) menuIndex = 2; // multiplayer
        if (input.wasKeyPressed(SDL_SCANCODE_O) && menuIndex == 0) menuIndex = 3; // options
        if (input.wasKeyPressed(SDL_SCANCODE_Q) && menuIndex == 0) return; // quit
        if (input.wasKeyPressed(SDL_SCANCODE_S) && menuIndex == 3) this->_hud.toggleFps(); // frame info
        if (input.wasKeyPressed(SDL_SCANCODE_B) && menuIndex != 0){
            menuIndex = 0;
            if (this->_singleplayer != nullptr) {
                delete this->_singleplayer;
                this->_singleplayer = nullptr;
            }
            if (this->_player != nullptr) {
                delete this->_player;
                this->_player = nullptr;
            }
        }
        const Uint64 currentTimeMs = SDL_GetTicks64();
        int elapsedTimeMs = currentTimeMs - lastUpdateTime;

        frameCount++;
        if (currentTimeMs - lastFpsUpdateTime >= 1000) {
            currentFPS = frameCount / ((currentTimeMs - lastFpsUpdateTime) / 1000.0f);
            frameCount = 0;
            lastFpsUpdateTime = currentTimeMs;
        }

        this->update(std::min(elapsedTimeMs, MAX_FRAME_TIME), this->_graphics);
        lastUpdateTime = currentTimeMs;

        this->draw(menuIndex, currentFPS, elapsedTimeMs);

        // Frame rate limiting
        Uint64 frameEndTime = SDL_GetTicks64();
        int frameDuration = frameEndTime - currentTimeMs;
        if (frameDuration < MAX_FRAME_TIME) {
            SDL_Delay(MAX_FRAME_TIME - frameDuration);
        }
    }
}

void Game::draw(Uint8 p_menuIndex, float p_currentFPS, int p_elapsedTime){
    SDL_Color bgColor = {0, 0, 0, 255};
    SDL_SetRenderDrawColor(this->_graphics.getRenderer(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);

    this->_graphics.clear();

    this->_hud.draw(p_menuIndex, p_currentFPS, p_elapsedTime);

    if(this->_singleplayer != nullptr)
        this->_singleplayer->draw(this->_graphics);

    this->_graphics.flip();
}

void Game::update(float p_elapsedTime, Graphics &p_graphics){
    if(this->_singleplayer != nullptr)
        this->_singleplayer->update(p_elapsedTime);


    //where the "game" will update status
    // player pos too
    // anda the hud too
}