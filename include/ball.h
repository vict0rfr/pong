#ifndef BALL_H
#define BALL_H

#include "graphics.h"
#include "globals.h"
#include "rectangle.h"
#include "sprite.h"

#include <vector>

/**
 * @brief Represents a Ball in the game.
 */
class Ball : public Sprite{
public:
    /**
     * @brief Constructs a Ball with graphics context, and initial position.
     * 
     * @param p_graphics Graphics context to draw the ball.
     * @param p_spawnPoint Initial position of the ball.
     */
    Ball(Graphics &p_graphics, Vector2f p_spawnPoint);

    /**
     * @brief Draws the ball on the screen.
     * 
     * @param p_graphics Graphics context to draw the ball.
     */
    void draw(Graphics &p_graphics);

    /**
     * @brief Updates the ball's position and animations.
     * 
     * @param p_elapsedTime Time elapsed since the last update.
     */
    void update(float p_elapsedTime);
    
    void handleBorderCollisions();

    void reverseDirectionX();

    void reverseDirectionY();

    inline float getX() const { return this->_x; }
    inline float getY() const { return this->_y; }
    inline float getDx() const { return this->_dx; }
    inline float getDy() const { return this->_dy; }

    inline void setPosition(float p_x, float p_y) {
        this->_x = p_x;
        this->_y = p_y;
        this->_boundingBox = SpriteRectangle(this->_x, this->_y, this->_boundingBox.getWidth(), this->_boundingBox.getHeight());
    }

    inline void setVelocity(float p_dx, float p_dy) {
        this->_dx = p_dx;
        this->_dy = p_dy;
    }

    inline void reset(float p_x = globals::SCREEN_WIDTH / 2.0f, float p_y = globals::SCREEN_HEIGHT / 2.0f) {
        this->_x = p_x;
        this->_y = p_y;
        this->_dx = globals::ballDx;
        this->_dy = (rand() % 2 == 0) ? globals::ballDy : -globals::ballDy;
        this->_hasLost = false;
        this->_boundingBox = SpriteRectangle(this->_x, this->_y, this->_boundingBox.getWidth(), this->_boundingBox.getHeight());
    }

    inline bool getLostStatus() const { return this->_hasLost; }
private:
    float _dx, _dy; // Velocity of the ball

    bool _hasLost;
};

#endif // BALL_H