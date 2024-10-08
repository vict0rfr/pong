/**
 * @file game.h
 * @brief Defines the Game class responsible for managing the game's main loop and state.
 *
 * This file contains the Game class declaration, including the constructor and destructor,
 * and methods for the game loop, drawing, and updating game state.
 */

#ifndef GAME_H
#define GAME_H

#include "graphics.h"
#include "hud.h"

class Input;
class Player;
class Singleplayer;
class Multiplayer;

#include <memory>

/**
 * @class Game
 * @brief Manages the main game loop, rendering, and updating.
 *
 * The Game class is central to the game application. It initializes the game state,
 * enters the main game loop, and handles updates and rendering.
 */
class Game{
public:
    /**
     * @brief Constructs the Game object and initializes game state.
     */
    Game();

    /**
     * @brief Destructs the Game object, performing necessary cleanup.
     */
    ~Game();

    static void setRunningFalse() { _isRunning = false; }

private:
    /**
     * @brief Contains the game's main loop logic.
     *
     * Calls update and draw methods in a loop until the game ends.
     */
    void gameLoop();

    /**
     * @brief Draws the game's current state to the screen.
     *
     * @param p_graphics Reference to the Graphics object for rendering.
     */
    void draw(float p_fps, int p_elapsedTime);

    /**
     * @brief Updates the game's state.
     *
     * @param p_elapsedTime The time elapsed since the last update call.
     */
    void update(float p_elapsedTime);

    void handleInput(Input &p_input);

    Graphics _graphics; ///< Graphics object used for rendering.
    Hud _hud;
    std::unique_ptr<Singleplayer> _singleplayer;
	std::unique_ptr<Multiplayer> _multiplayer;
    std::shared_ptr<Player> _player;
    Menu _menu;

    static bool _isRunning;
};

#endif // GAME_H