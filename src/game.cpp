#include <SDL3/SDL.h>
#include <iostream>

#include "game.h"
#include "input.h"
#include "hud.h"
#include "singleplayer.h"
#include "multiplayer.h"

#include <enet/enet.h>

namespace{
    const int FPS_TARGET = 60;
    constexpr int MAX_FRAME_TIME = 1000 / FPS_TARGET;

    unsigned int frameCount = 0;
    float currentFPS = 0.0f;
}

bool Game::_isRunning = true;

Game::Game() : 
    _graphics(),
    _hud(_graphics),
    _singleplayer(nullptr),
	_multiplayer(nullptr),
    _player(nullptr),
    _menu()
    {
    enet_initialize();
    this->gameLoop();
    }

Game::~Game(){
    // if (this->_singleplayer){
    //     delete this->_singleplayer;
    //     this->_singleplayer = nullptr;
    // }

	// if (this->_multiplayer) {
	// 	delete this->_multiplayer;
	// 	this->_multiplayer = nullptr;
	// }

    // if (this->_player){
    //     delete this->_player;
    //     this->_player = nullptr;
    // }
}

void Game::gameLoop() {
    Input input;
    Uint64 lastUpdateTime = SDL_GetTicks();
    Uint64 lastFpsUpdateTime = lastUpdateTime;

    while (this->_isRunning) {
        input.beginNewFrame();
        this->handleInput(input);

        const Uint64 currentTimeMs = SDL_GetTicks();
        Uint64 elapsedTimeMs = currentTimeMs - lastUpdateTime;

        frameCount++;
        if (currentTimeMs - lastFpsUpdateTime >= 1000) {
            currentFPS = frameCount / ((currentTimeMs - lastFpsUpdateTime) / 1000.0f);
            frameCount = 0;
            lastFpsUpdateTime = currentTimeMs;
        }

        this->update((((static_cast<float>(elapsedTimeMs)) < (MAX_FRAME_TIME)) ? (static_cast<float>(elapsedTimeMs)) : (MAX_FRAME_TIME)));
        lastUpdateTime = currentTimeMs;

        this->draw(currentFPS, static_cast<int>(elapsedTimeMs));

        // Frame rate limiting
        Uint64 frameEndTime = SDL_GetTicks();
        int frameDuration = static_cast<int>(frameEndTime - currentTimeMs);
        if (frameDuration < MAX_FRAME_TIME) {
            SDL_Delay(MAX_FRAME_TIME - frameDuration);
        }
    }
    enet_deinitialize();
}

void Game::draw(float p_currentFPS, int p_elapsedTime){
    this->_graphics.clear();

    if(this->_menu == LOSE){
        this->_player.reset();
        this->_singleplayer.reset();
    }

    this->_hud.draw(this->_menu, p_currentFPS, p_elapsedTime);

    if(this->_singleplayer)
        this->_singleplayer->draw(this->_graphics);
	if (this->_multiplayer)
		this->_multiplayer->draw(this->_graphics);

    this->_graphics.flip();
}

void Game::update(float p_elapsedTime){
    if(this->_singleplayer)
        this->_singleplayer->update(p_elapsedTime);

    if (this->_multiplayer)
        this->_multiplayer->update(p_elapsedTime);

    if(this->_player){
        if(this->_player->getLostStatus())
            this->_menu = LOSE; // delete sp and player instances unless you have a play again button
    }
}

void Game::handleInput(Input &p_input) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            p_input.keyDownEvent(e);
            if (this->_menu == MPOPTIONCLIENT && this->_hud.getOptionIndex() == 1) {
                this->_hud.handleKeyInput(e.key.scancode, &this->_menu);
            }
        } else if (e.type == SDL_EVENT_KEY_UP) {
            p_input.keyUpEvent(e);
        } else if (e.type == SDL_EVENT_QUIT) {
            this->setRunningFalse();
            return;
        }
    }

    auto handleArrowKeys = [this, &p_input]() {
        if (p_input.wasKeyPressed(SDL_SCANCODE_UP) && this->_menu != SPGAME && this->_menu != MPGAMECLIENT && this->_menu != MPGAMEHOST) this->_hud.handleKeyInput(SDL_SCANCODE_UP);
        if (p_input.wasKeyPressed(SDL_SCANCODE_DOWN) && this->_menu != SPGAME && this->_menu != MPGAMECLIENT && this->_menu != MPGAMEHOST) this->_hud.handleKeyInput(SDL_SCANCODE_DOWN);
        if (p_input.wasKeyPressed(SDL_SCANCODE_RIGHT) && this->_menu != SPGAME && this->_menu != MPGAMECLIENT && this->_menu != MPGAMEHOST) this->_hud.handleKeyInput(SDL_SCANCODE_RIGHT);
        if (p_input.wasKeyPressed(SDL_SCANCODE_LEFT) && this->_menu != SPGAME && this->_menu != MPGAMECLIENT && this->_menu != MPGAMEHOST) this->_hud.handleKeyInput(SDL_SCANCODE_LEFT);
        
        if ((this->_menu == SPGAME || this->_menu == MPGAMECLIENT || this->_menu == MPGAMEHOST) && this->_player) {
            if (p_input.isKeyHeld(SDL_SCANCODE_UP)) {
                this->_player->moveUp();
            }
            if (p_input.wasKeyReleased(SDL_SCANCODE_UP)) {
                this->_player->stopMoving();
            }
            if (p_input.isKeyHeld(SDL_SCANCODE_DOWN)) {
                this->_player->moveDown();
            }
            if (p_input.wasKeyReleased(SDL_SCANCODE_DOWN)) {
                this->_player->stopMoving();
            }
        }
   };

    auto handleMenuSelection = [this, &p_input]() {
        auto lambdaStartSPGame = [this]() {
                this->_menu = SPGAME;
                this->_player = std::make_shared<Player>(this->_graphics, Vector2f(100, 100));
                this->_singleplayer = std::make_unique<Singleplayer>(this->_graphics, this->_player, this->_hud);
                this->_hud.setOptionIndex(1);
            };

        auto lambdaStartMPGameClient = [this]() {
                this->_menu = MPGAMECLIENT;
                this->_player = std::make_shared<Player>(this->_graphics, Vector2f(100, 100));
                this->_multiplayer = std::make_unique<Multiplayer>(this->_graphics, this->_player, nullptr, this->_hud, globals::clientIpAddress);
                this->_hud.setOptionIndex(1);
            };

        auto lambdaStartMPGameHost = [this]() {
                this->_menu = MPGAMEHOST; //make another method "state" 
                // where when i press enter on option menu, its sends to menu lobby + packet to other player(broadcast) then when you press start its actually started
                this->_player = std::make_shared<Player>(this->_graphics, Vector2f(540, 100));
                this->_multiplayer = std::make_unique<Multiplayer>(this->_graphics, nullptr, this->_player, this->_hud);
                this->_hud.setOptionIndex(1);
            };

        if (p_input.wasKeyPressed(SDL_SCANCODE_ESCAPE) && (this->_menu == SPGAME || this->_menu == MPGAMECLIENT || this->_menu == MPGAMEHOST)) {
            if (this->_player) {
                this->_player.reset();
            }
            if (this->_singleplayer) {
                this->_singleplayer.reset();
            }
            if (this->_multiplayer) {
                this->_multiplayer.reset();
            }
            this->_menu = MAINMENU;
            this->_hud.setOptionIndex(1);
        }

        if (p_input.wasKeyPressed(SDL_SCANCODE_RETURN)) {
            if (this->_menu == MPOPTIONHOST && this->_hud.getOptionIndex() == 4 && this->_multiplayer == nullptr && this->_player == nullptr) lambdaStartMPGameHost();
            if (this->_menu == MPOPTIONCLIENT && this->_hud.getOptionIndex() == 2 && this->_multiplayer == nullptr && this->_player == nullptr) lambdaStartMPGameClient();
            if (this->_menu == MPLOBBY && this->_hud.getOptionIndex() == 2 && this->_multiplayer == nullptr && this->_player == nullptr) lambdaStartMPGameHost();
            if (this->_menu == SPMENU && this->_hud.getOptionIndex() == 4 && this->_singleplayer == nullptr && this->_player == nullptr) lambdaStartSPGame();
            if (this->_menu == LOSE && this->_hud.getOptionIndex() == 1 && this->_singleplayer == nullptr && this->_player == nullptr) lambdaStartSPGame();

            this->_hud.handleKeyInput(SDL_SCANCODE_RETURN, &this->_menu);
        }
    };
    handleMenuSelection();
    handleArrowKeys();
}