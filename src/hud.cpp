#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include "hud.h"
#include "KenneyPixel.h"
#include "game.h"

Hud::Hud(Graphics &p_graphics) :
		_graphics(p_graphics),
		_font(nullptr),
		_color({255, 255, 255, 255}),
		_selectedOptionIndex(1)
		{
			this->_hudItem = {};

			if(TTF_Init() != 0){
				std::cerr << "TTF_Init Error: " << TTF_GetError() << '\n';
				return;
			}

			SDL_IOStream* io = SDL_IOFromMem(KenneyPixel_ttf, KenneyPixel_ttf_len);
			if (!io) {
				std::cerr << "SDL_IOFromMem Error: " << SDL_GetError() << '\n';
				return;
			}

			this->_font = TTF_OpenFontIO(io, 1, 56);
			if(!_font)
				std::cerr << "TTF_OpenFontIO Error: " << TTF_GetError() << '\n';
		}

Hud::~Hud(){
}

void Hud::draw(Menu p_menu, float p_fps, int p_elapsedTime){
	if(this->_graphics.getWindowStatus()){
		SDL_SetRenderDrawColor(this->_graphics.getRenderer(), 63, 63, 63, 255);
		SDL_RenderLine(this->_graphics.getRenderer(), 0.0f, 0.0f, static_cast<float>(globals::SCREEN_WIDTH), 0.0f); //top
		SDL_RenderLine(this->_graphics.getRenderer(), 0.0f, 0.0f, 0.0f, static_cast<float>(globals::SCREEN_HEIGHT)); //left
		SDL_RenderLine(this->_graphics.getRenderer(), static_cast<float>(globals::SCREEN_WIDTH) - 1, 0.0f, static_cast<float>(globals::SCREEN_WIDTH) - 1, static_cast<float>(globals::SCREEN_HEIGHT) - 1); // right
		SDL_RenderLine(this->_graphics.getRenderer(), 0.0f, static_cast<float>(globals::SCREEN_HEIGHT) - 1, static_cast<float>(globals::SCREEN_WIDTH) - 1, static_cast<float>(globals::SCREEN_HEIGHT) - 1); // bottom
	}
	this->renderFrameInfo(p_fps,  p_elapsedTime);
	switch (p_menu){
		case MAINMENU:
			this->renderMenu();
			break;
		case SPMENU:
			this->renderSPOptions();
			break;
		case MPMENU:
			this->renderMPMenu();
			break;
		case MPOPTIONHOST:
			this->renderMPHost();
			break;
		case MPOPTIONCLIENT:
			this->renderMPClient();
			break;
		case MPLOBBY:
			this->renderMPLobby();
			break;
		case OPTIONS:
			this->renderOptions();
			break;
		case LOSE:
			this->renderLose();
			break;
		default:
			break;
	}
}

// void Hud::update(){
// }

void Hud::renderText(const std::string& p_text, float p_x, float p_y, float p_texW, float p_texH){
	if(!this->_font){
		return;
	}

	SDL_Surface* surface = TTF_RenderText_Solid(this->_font, p_text.c_str(), this->_color);
	if(!surface){
		std::cerr << "TTF_RenderText_Solid Error: " << TTF_GetError() << '\n';
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(this->_graphics.getRenderer(), surface);
	if(!texture){
		std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << '\n';
		SDL_DestroySurface(surface);
		return;
	}

	SDL_FRect dst = { p_x, p_y, p_texW, p_texH };

	this->_graphics.blitSurface(texture, NULL, &dst);

	SDL_DestroyTexture(texture);
	SDL_DestroySurface(surface);
}

void Hud::renderMenu(){
	this->_hudItem = {
		{"PONG", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 5), Dash::NODASH},
		{"SINGLEPLAYER", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.5), Dash::DASH},
		{"MULTIPLAYER", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.0), Dash::DASH},
		{"OPTIONS", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 1.6667), Dash::DASH},
		{"QUIT", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 1.25), Dash::DASH}
	};
	this->renderHudItems();
}

void Hud::renderOptions(){
	this->_hudItem = {
		{"GAME OPTIONS", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 5), Dash::NODASH},
		{"SHOW FRAME INFO", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.5), Dash::DASH},
		{"FULLSCREEN TOGGLE", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.0), Dash::DASH},
		{"RESOLUTION : " + std::to_string(globals::SCREEN_WIDTH) + " / " + std::to_string(globals::SCREEN_HEIGHT), Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 1.66667), Dash::DASH},
		{"BACK", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 1.25), Dash::DASH}
	};
	this->renderHudItems();
}

void Hud::renderFrameInfo(float p_fps, int p_elapsedTime){
	if(this->_showFPS){
		this->_hudItem = {
			{"FPS: " + std::to_string(static_cast<int>(p_fps)), Vector2f(0, 0), Dash::NODASH, 3} // add a ping counter later
			};

		for(const HudItem& item : this->_hudItem){
			int titleTexW = 0;
			int titleTexH = 0;
			TTF_SizeUTF8(this->_font, item.text.c_str(), &titleTexW, &titleTexH);
			this->renderText(item.text, item.pos.x, item.pos.y, titleTexW / 3, titleTexH / 3);
		}
	}
}

// void Hud::toggleFps(){
//     this->_showFPS = !this->_showFPS;
// }

void Hud::renderPoints(Player* p_client, Player* p_host){
	if (p_host == NULL) {
		this->_hudItem = {
			{std::to_string(p_client->getPoints()), Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 7), Dash::NODASH}
		};
		return;
	}
	else {
		this->_hudItem = {
			{std::to_string(p_client->getPoints()), Vector2f(globals::SCREEN_WIDTH / 4, globals::SCREEN_HEIGHT / 7), Dash::NODASH},
			{std::to_string(p_host->getPoints()), Vector2f(globals::SCREEN_WIDTH / 1.25, globals::SCREEN_HEIGHT / 7), Dash::NODASH}
		};
	}
	this->renderHudItems();
}

void Hud::renderLose(){
	this->_hudItem = {
		{"YOU LOSE!", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 5), Dash::NODASH},
		{"PLAY AGAIN", Vector2f(globals::SCREEN_WIDTH / 3, globals::SCREEN_HEIGHT / 1.25), Dash::DASH},
		{"BACK", Vector2f(globals::SCREEN_WIDTH / 1.5, globals::SCREEN_HEIGHT / 1.25), Dash::DASH}
	};
	this->renderHudItems();
}

void Hud::renderSPOptions(){
	std::vector<std::vector<float>> SPOptions = {
		{0.15f, 0.30f, 0.45f, 0.60f, 0.90f}, // ballSpeedOptions
		{0.20f, 0.40f, 0.60f, 0.80f, 1.00f}, // playerSpeedOptions
		{5.00f, 7.50f, 10.00f, 20.00f, 30.00f} // ballSizeOptions
	};

	std::ostringstream stream;
	stream.precision(2);

	stream << "BALL SPEED: " << SPOptions[0][this->_firstVectorIndex];
	std::string ballSpeedStr = stream.str();
	stream.str("");
	stream.clear();

	stream << "PLAYER SPEED: " << SPOptions[1][this->_secondVectorIndex];
	std::string playerSpeedStr = stream.str();
	stream.str("");
	stream.clear();

	stream << "BALL SIZE: " << SPOptions[2][this->_thirdVectorIndex];
	std::string ballSizeStr = stream.str();
	stream.str("");
	stream.clear();

	globals::setBallSpeed(SPOptions[0][this->_firstVectorIndex]);
	globals::setPlayerSpeed(SPOptions[1][this->_secondVectorIndex]);
	globals::setBallSize(SPOptions[2][this->_thirdVectorIndex]);

	this->_hudItem = {
		{"SINGLEPLAYER OPTIONS", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 5), Dash::NODASH},
		{ballSpeedStr, Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.5), Dash::DASH},
		{playerSpeedStr, Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2), Dash::DASH},
		{ballSizeStr, Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 1.666667), Dash::DASH},
		//add cooldown option?
		{"PLAY", Vector2f(globals::SCREEN_WIDTH / 3, globals::SCREEN_HEIGHT / 1.25), Dash::DASH},
		{"BACK", Vector2f(globals::SCREEN_WIDTH / 1.5, globals::SCREEN_HEIGHT / 1.25), Dash::DASH}     
	};
	this->renderHudItems();
}

void Hud::renderMPMenu() {
	this->_hudItem = {
		{"MULTIPLAYER", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 5), Dash::NODASH},
		{"CLIENT", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.5), Dash::DASH},
		{"HOST", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.0), Dash::DASH},
		{"BACK", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 1.25), Dash::DASH}
	};
	this->renderHudItems();
}

void Hud::renderMPHost() {
	std::vector<std::vector<float>> MPOptions = {
		{0.15f, 0.30f, 0.45f, 0.60f, 0.90f}, // ballSpeedOptions
		{0.20f, 0.40f, 0.60f, 0.80f, 1.00f}, // playerSpeedOptions
		{5.00f, 7.50f, 10.00f, 20.00f, 30.00f} // ballSizeOptions
	};

	std::ostringstream stream;
	stream.precision(2);

	stream << "BALL SPEED: " << MPOptions[0][this->_firstVectorIndex];
	std::string ballSpeedStr = stream.str();
	stream.str("");
	stream.clear();

	stream << "PLAYER SPEED: " << MPOptions[1][this->_secondVectorIndex];
	std::string playerSpeedStr = stream.str();
	stream.str("");
	stream.clear();

	stream << "BALL SIZE: " << MPOptions[2][this->_thirdVectorIndex];
	std::string ballSizeStr = stream.str();
	stream.str("");
	stream.clear();

	globals::setBallSpeed(MPOptions[0][this->_firstVectorIndex]);
	globals::setPlayerSpeed(MPOptions[1][this->_secondVectorIndex]);
	globals::setBallSize(MPOptions[2][this->_thirdVectorIndex]);
	//make this lambda function later
	this->_hudItem = {
		{"HOST", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 5), Dash::NODASH},
		{ballSpeedStr, Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.5), Dash::DASH},
		{playerSpeedStr, Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2), Dash::DASH},
		{ballSizeStr, Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 1.666667), Dash::DASH},
		{"START", Vector2f(globals::SCREEN_WIDTH / 3, globals::SCREEN_HEIGHT / 1.25), Dash::DASH},
		{"BACK", Vector2f(globals::SCREEN_WIDTH / 1.5, globals::SCREEN_HEIGHT / 1.25), Dash::DASH},
		{"IP ADDRESS: ", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 3.5), Dash::NODASH, 2}
	};
	this->renderHudItems();
}

void Hud::renderMPClient() {
	this->_hudItem = {
		{"CLIENT", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 5), Dash::NODASH},
		{"IP ADDRESS: " + globals::clientIpAddress, Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.5), Dash::DASH},
		{"CONNECT", Vector2f(globals::SCREEN_WIDTH / 3, globals::SCREEN_HEIGHT / 1.5), Dash::DASH},
		{"BACK", Vector2f(globals::SCREEN_WIDTH / 1.5, globals::SCREEN_HEIGHT / 1.5), Dash::DASH}
	};
	this->renderHudItems();
}

void Hud::renderMPLobby(){
	this->_hudItem = {
		{"LOBBY", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 5), Dash::NODASH},
		{"PLAYER 1: NOT CONNECTED", Vector2f(globals::SCREEN_WIDTH / 2, globals::SCREEN_HEIGHT / 2.5), Dash::NODASH},
		{"START", Vector2f(globals::SCREEN_WIDTH / 3, globals::SCREEN_HEIGHT / 1.5), Dash::DASH},
		{"BACK", Vector2f(globals::SCREEN_WIDTH / 1.5, globals::SCREEN_HEIGHT / 1.5), Dash::DASH}
	};
	this->renderHudItems();
}

void Hud::handleKeyInput(SDL_Scancode p_key, Menu* p_menu) {
	do {
		switch (p_key){
		case SDL_SCANCODE_UP:
			if((this->_selectedOptionIndex = (this->_selectedOptionIndex - 1 + this->_hudItem.size()) % this->_hudItem.size()) == 0){
				this->_selectedOptionIndex = this->_hudItem.size() - 1;
			}
			break;
		case SDL_SCANCODE_DOWN:
			this->_selectedOptionIndex = (this->_selectedOptionIndex + 1) % this->_hudItem.size();
			break;
		case SDL_SCANCODE_LEFT:
			if(this->_selectedOptionIndex == 1) this->_firstVectorIndex = std::clamp(this->_firstVectorIndex -= 1, 0, 4);
			if(this->_selectedOptionIndex == 2) this->_secondVectorIndex = std::clamp(this->_secondVectorIndex -= 1, 0, 4);
			if(this->_selectedOptionIndex == 3) this->_thirdVectorIndex = std::clamp(this->_thirdVectorIndex -= 1, 0, 4);
			break;
		case SDL_SCANCODE_RIGHT:
			if(this->_selectedOptionIndex == 1) this->_firstVectorIndex = std::clamp(this->_firstVectorIndex += 1, 0, 4);
			if(this->_selectedOptionIndex == 2) this->_secondVectorIndex = std::clamp(this->_secondVectorIndex += 1, 0, 4);
			if(this->_selectedOptionIndex == 3) this->_thirdVectorIndex = std::clamp(this->_thirdVectorIndex += 1, 0, 4);
			break;
		case SDL_SCANCODE_RETURN:
			this->handleSelect(p_menu);
			break;
		default:
			if (p_menu != nullptr && *p_menu == MPOPTIONCLIENT && this->_selectedOptionIndex == 1) {
				if (p_key == SDL_SCANCODE_BACKSPACE) {
					if (!globals::clientIpAddress.empty()) {
						globals::clientIpAddress.pop_back();
					}
				} else if (globals::clientIpAddress.length() < 15) {
					char ch = '\0';
					if (p_key >= SDL_SCANCODE_1 && p_key <= SDL_SCANCODE_9) {
						ch = '1' + (p_key - SDL_SCANCODE_1);
					} else if (p_key == SDL_SCANCODE_0) {
						ch = '0';
					} else if (p_key >= SDL_SCANCODE_KP_1 && p_key <= SDL_SCANCODE_KP_9) {
						ch = '1' + (p_key - SDL_SCANCODE_KP_1);
					} else if (p_key == SDL_SCANCODE_KP_0) {
						ch = '0';
					} else if (p_key == SDL_SCANCODE_PERIOD || p_key == SDL_SCANCODE_KP_PERIOD) {
						ch = '.';
					}
					if (ch != '\0') {
						globals::clientIpAddress += ch;
					}
				}
			}
			break;
		}
	} while (this->_hudItem[this->_selectedOptionIndex].dash == Dash::NODASH);
}

void Hud::handleSelect(Menu* p_menu){
	if(*p_menu == MAINMENU){
		switch(this->_selectedOptionIndex){
			case 1:
				*p_menu = SPMENU;
				break;
			case 2:
				*p_menu = MPMENU;
				this->_selectedOptionIndex = 1;
				break;
			case 3:
				*p_menu = OPTIONS;
				this->_selectedOptionIndex = 1;
				break;
			case 4:
				Game::setRunningFalse();
				break;
			default:
				break;
		}
	} else if(*p_menu == SPMENU){
		this->_firstVectorIndex = 0;
		this->_secondVectorIndex = 0;
		this->_thirdVectorIndex = 0;
		switch (this->_selectedOptionIndex){
			case 1:
				//ball speed
				break;
			case 2:
				//player speed
				break;
			case 3:
				//ball size
				break;
			case 4:
				//start sp game
				break;
			case 5:
				*p_menu = MAINMENU;
				this->_selectedOptionIndex = 1;
			default:
				break;
		}
	} else if(*p_menu == MPMENU){
		switch(this->_selectedOptionIndex){
			case 1:
				 *p_menu = MPOPTIONCLIENT;
				break;
			case 2:
				 *p_menu = MPOPTIONHOST;
				break;
			case 3:
				*p_menu = MAINMENU;
				this->_selectedOptionIndex = 1;
				break;
			default:
				break;
		}
	} else if (*p_menu == MPOPTIONCLIENT) {
		switch (this->_selectedOptionIndex) {
		case 2:
			*p_menu = MPGAMECLIENT;
			break;
		case 3:
			*p_menu = MPMENU;
			this->_selectedOptionIndex = 1;
			break;
		default:
			break;
		}
	} else if (*p_menu == MPOPTIONHOST) {
		switch (this->_selectedOptionIndex) {
		case 4:
			*p_menu = MPGAMEHOST;
			break;
		case 5:
			*p_menu = MPMENU;
			this->_selectedOptionIndex = 1;
			break;
		default:
			break;
		}
	} else if (*p_menu == MPLOBBY) {
		switch (this->_selectedOptionIndex) {
		case 1:
			//*p_menu = CLIENT; ip address is show here
			break;
		case 2:
			*p_menu = MPGAMEHOST;
			break;
		case 3:
			*p_menu = MPOPTIONHOST;
			this->_selectedOptionIndex = 2;
			break;
		default:
			break;
		}
	} else if(*p_menu == OPTIONS){
		switch (this->_selectedOptionIndex){
			case 1:
				this->_showFPS = !this->_showFPS;
				break;
			case 2:
				this->_graphics.toggleFullScreen();
				break;
			case 3:
				//resolution change
				break;
			case 4:
				*p_menu = MAINMENU;
				this->_selectedOptionIndex = 1;
				break;
			default:
				break;
		}
	} else if (*p_menu == LOSE){
		switch(this->_selectedOptionIndex){
			case 1:
				break;
			case 2:
				*p_menu = MAINMENU;
				this->_selectedOptionIndex = 1;
				break;
			default:
				break;
		}
	}
}

void Hud::renderHudItems(){
	for (size_t i = 0; i < this->_hudItem.size(); ++i) {
		const HudItem& item = this->_hudItem[i];
		int titleTexW = 0;
		int titleTexH = 0;

		std::string displayText = item.text;
		if (i == this->_selectedOptionIndex && item.dash == Dash::DASH) {
			displayText = "- " + displayText;
		}

		TTF_SizeUTF8(this->_font, displayText.c_str(), &titleTexW, &titleTexH);

		int scaledTexW = titleTexW / item.sizeScale;
		int scaledTexH = titleTexH / item.sizeScale;
		this->renderText(displayText, item.pos.x - (scaledTexW / 2), item.pos.y - (scaledTexH / 2), scaledTexW, scaledTexH);
	}
}