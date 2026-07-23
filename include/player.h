/**
 * @file player.h
 * @brief Player class definition, inheriting from AnimatedSprite.
 * 
 * The Player class represents a player character in the game. It inherits from the AnimatedSprite class,
 * allowing it to have animated movements. The class provides functionality for player movement, including
 * moving left, right, and stopping. It also handles player animations.
 */

#ifndef PLAYER
#define PLAYER

// #include "globals.h"
#include "sprite.h"

// #include <string>
#include <vector>

// class Graphics;
// class Door;

/**
 * @class Player
 * @brief Represents a player character with animated movements.
 * 
 * This class is responsible for handling the player character's animations and movements within the game.
 * It inherits from the AnimatedSprite class to utilize animated sprites for the player character.
 */
class Player : public Sprite {
public:
    /**
     * @brief Default constructor for Player.
     */
    Player();

    /**
     * @brief Constructs a Player with graphics context, and initial position.
     * 
     * @param p_graphics Graphics context to draw the player.
     * @param p_x Initial x-coordinate of the player.
     * @param p_y Initial y-coordinate of the player.
     */
    Player(Graphics &p_graphics, Vector2f p_spawnPoint);

    /**
     * @brief Draws the player on the screen.
     * 
     * @param p_graphics Graphics context to draw the player.
     */
    void draw(Graphics &p_graphics);

    /**
     * @brief Updates the player's position and animations.
     * 
     * @param p_elapsedTime Time elapsed since the last update.
     */
    void update(float p_elapsedTime);

    void moveUp();

    void moveDown();

    /**
     * @brief Stops the player's movement (dx = 0).
     */
    void stopMoving();

    /**
     * @brief Handles collisions with tiles.
     * 
     * @param p_others A vector of rectangles representing the tiles.
     */
    void handleBorderCollisions();

    /**
     * @brief Gets the x-coordinate of the player.
     * 
     * @return float: The x-coordinate.
     */
    // inline float getX() const { return this->_x; }

    /**
     * @brief Gets the y-coordinate of the player.
     * 
     * @return float: The y-coordinate.
     */
    // inline float getY() const { return this->_y; }

    inline float getY() const { return this->_y; }
    inline void setY(float p_y) { 
        this->_y = p_y; 
        this->_boundingBox = SpriteRectangle(this->_x, this->_y, this->_boundingBox.getWidth(), this->_boundingBox.getHeight());
    }
    inline int getPoints() const { return this->_points; }
    inline void setPoints(int p_points) { this->_points = p_points; }
    inline void gainPoint() { this->_points += 1; }

    inline void setLost() { this->_hasLost = true; }

    inline void setWon() { this->_hasWon = true; }

    inline bool getLostStatus() const { return this->_hasLost; }

private:
    int _points;
    float _dy; ///< Delta y for player's movement.

    bool _hasLost;
    bool _hasWon;
};

#endif /* PLAYER */