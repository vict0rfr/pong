#include "player.h"
#include "graphics.h"

#include <iostream>

namespace player_constants{
    const float SPEED = 0.2f;
}

Player::Player(){}

Player::Player(Graphics &p_graphics, Vector2f p_spawnPoint):
    Sprite(p_graphics, "res/gfx/bar.png", 0, 0, 10, 75, 50, 50),
        _dy(0)
    {
        p_graphics.loadImage("res/gfx/bar.png");
    }

void Player::draw(Graphics &p_graphics){
    Sprite::draw(p_graphics, this->_x, this ->_y);
}

void Player::stopMoving(){
    this->_dy = 0.0f;
}

void Player::update(float p_elapsedTime){
    this->_y += _dy * p_elapsedTime;
    Sprite::update();
}

void Player::handleTileCollisions(std::vector<Rectangle> &p_others){
    //when ball enters in contact with bar, ball bounces back
}
