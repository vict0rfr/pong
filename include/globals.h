/**
 * @file globals.h
 * @brief This file contains global constants and a structure used throughout the application.
 */

#ifndef GLOBALS
#define GLOBALS

#include <cstdlib>
#include <string>

/**
 * @namespace globals
 * @brief Namespace containing global constants.
 */
namespace globals{
    constexpr int SCREEN_WIDTH = 640; ///< Width of the screen in pixels.
    constexpr int SCREEN_HEIGHT = 480; ///< Height of the screen in pixels.
    constexpr float SPRITE_SCALE = 1.0f; ///< Scale factor for sprites.

    inline std::string clientIpAddress = "127.0.0.1";
    inline float ballDx = 0.15f;
    inline float ballDy = rand() % 2 == 0 ? 0.15f : -0.15f;
    inline float playerSpeed = 0.20f;
    inline float ballSize = 10.0f;

    inline void setBallSpeed(float p_ballD) { ballDx = p_ballD; ballDy = rand() % 2 == 0 ? p_ballD : -p_ballD; }
    inline void setPlayerSpeed(float p_playerSpeed) { playerSpeed = p_playerSpeed; }
    inline void setBallSize(float p_ballSize) { ballSize = p_ballSize; }
}

namespace sides{
    enum Side{
        TOP,
        BOTTOM,
        LEFT,
        RIGHT,
        NONE,
    };

    // const inline Side getOppositeSide(Side p_side){
    //     return
    //             p_side == TOP ? BOTTOM :
    //             p_side == BOTTOM ? TOP :
    //             p_side == LEFT ? RIGHT :
    //             p_side == RIGHT ? LEFT :
    //             NONE;
    // }
}

/**
 * @enum Direction
 * @brief Enumerates possible movement directions.
 * 
 * This enum is used to specify the direction of movement or facing of game entities.
 */
enum Direction{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum Menu{
    MAINMENU,
    SPMENU,
    MPMENU,
    OPTIONS,
    LOSE,
    // PAUSE,
    SPGAME,
    MPGAMECLIENT,
    MPGAMEHOST,
    MPOPTIONHOST,
    MPOPTIONCLIENT,
    MPLOBBY
};

/**
 * @struct Vector2f
 * @brief Represents a 2D vector with floating point coordinates.
 * 
 * This structure is used to represent positions and dimensions in 2D space.
 */
struct Vector2f{
    float x, y; ///< X and Y coordinates of the vector.

    /**
     * @brief Default constructor. Initializes x and y to 0.
     */
    Vector2f():
        x(0),
        y(0)
    {}

    /**
     * @brief Parameterized constructor.
     * @param p_x X coordinate.
     * @param p_y Y coordinate.
     */
    Vector2f(float p_x, float p_y):
        x(p_x),
        y(p_y)
    {}
};

#endif /* GLOBALS */
