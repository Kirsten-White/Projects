#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#pragma warning( push )
#pragma warning( disable : 4275 )

#include "AssetManager.h"
#include <SFML/Graphics.hpp>
#include <string>
#include "ScreenSize.h"
#include "LevelLoader.h"
#include "Tank.h"
#include "EnemyTank.h"
#include "Bullet.h"
#include "AIFriendlyTank.h"

/// <summary>
/// @author RP
/// @date January 2026
/// @version 1.0
/// 
/// </summary>

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>

class Game
{
public:
	/// <summary>
	/// @brief Default constructor that initialises the SFML window, 
	///   and sets vertical sync enabled. 
	/// </summary>
	Game(AssetManager & t_assetManager);

	/// <summary>
	/// @brief the main game loop.
	/// 
	/// A complete loop involves processing SFML events, updating and drawing all game objects.
	/// The actual elapsed time for a single game loop is calculated. If this value is 
	///  greater than the target time for one loop (1 / 60), then (and only then) is an update 
	///  operation performed.
	/// The target is at least one update and one render cycle per game loop, but typically 
	///  more render than update operations will be performed as we expect our game loop to
	///  complete in less than the target time.
	/// </summary>
	void run();

protected:
	/// <summary>
	/// @brief Once-off game initialisation code
	/// </summary>	
	void init();
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(double dt);

	void checkCollision(double dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

	/// <summary>
	/// @brief Checks for events.
	/// Allows window to function and exit. 
	/// Key down events are passed on to Game::processKeyPressed().
	/// </summary>	
	void processEvents();

	/// <summary>
	/// @brief Handles all user input.
	/// </summary>
	/// <param name="t_event">key pressed event</param>
	void processKeyPressed(const std::optional<sf::Event>& t_event);

	//Creates walls
	void generateWalls();

	// Font used for all text
	sf::Font m_arialFont;
	// main window
	sf::RenderWindow m_window;

	void displayGameOverScreen();

#ifdef TEST_FPS
	LevelData m_level;
	AssetManager & m_assetManager;
	Tank m_tank;
	AIFriendlyTank m_AIFriendlyTank;
	//TankEnemy m_tankEnemy;
	//Bullet m_bullet;
	std::vector<std::unique_ptr<TankEnemy>> m_enemyTanks;
	std::vector<std::unique_ptr<Bullet>> m_bullets;
	std::vector<sf::Sprite> m_wallSprites;
	sf::Sprite m_bgSprite{ m_BlankTexture };
	sf::Texture m_BlankTexture{ "resources/images/check.jpg" };
	//sf::Sprite m_blueTank{ m_BlankTexture };
	//sf::Sprite m_blueCannon{ m_BlankTexture };
	sf::Text x_updateFPS{ m_arialFont };	// text used to display updates per second.
	sf::Text x_drawFPS{ m_arialFont };		// text used to display draw calls per second.
	sf::Time x_secondTime {sf::Time::Zero};	// counter used to establish when a second has passed.
	int x_updateFrameCount{ 0 };			// updates per second counter.
	int x_drawFrameCount{ 0 };				// draws per second counter.
	sf::Clock m_timerClock;  // Clock
	sf::Time m_timeRemaining;
	sf::Text m_timerText;    // Text to display the timer
	bool gameOver = false;
	sf::Text m_gameOverText;
	sf::Text m_highScore;
	int m_Score = 0;
	int lastMoved = 0;
#endif // TEST_FPS

};
