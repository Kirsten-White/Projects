#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#pragma warning( push )
#pragma warning( disable : 4275 )

#include <SFML/Graphics.hpp>
#include <string>
#include "ScreenSize.h"

/// <summary>
/// author Kirsten White
/// </summary>

#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <vector>

const int MAX_ROCKS = 5;

class Game
{
public:
    Game();
    ~Game();

    /// <summary>
    /// Main method for game
    /// </summary>
    void run();

private:

    void processEvents();

    // SFML 3 uses the keyboard key directly
    void processKeys(sf::Keyboard::Key t_key);

    void update(sf::Time t_deltaTime);
    void render();

    void setupFontAndText();
    void setupSprite();

    // Player functions
    void movePlayerDown();
    void movePlayerUp();
    bool checkTopBorder();
    bool checkBottomBorder();

    // Enemy functions
    void arrayStart();
    void move();
    void resetRock();
    bool checkCollistion();
    void setScore();


    // ---------------------------------------------------------
    // Window
    // ---------------------------------------------------------

    sf::RenderWindow m_window;
    bool m_exitGame;


    // ---------------------------------------------------------
    // Font / Text
    // ---------------------------------------------------------

    sf::Font m_ArialBlackfont;

    // Font MUST be declared before Text in SFML 3
    sf::Text m_scoreDisplay;
    sf::Text m_gameover;


    // ---------------------------------------------------------
    // Jellyfish / Player
    // ---------------------------------------------------------

    sf::RectangleShape m_player;

    // Texture MUST be declared before Sprite
    sf::Texture m_jellyfishTexture;
    sf::Sprite m_jellyfishSprite;

    sf::Vector2f m_jellyfishLocation;
    sf::Vector2f m_jellyfishSpeed{ 0.0f, 10.0f };

    bool onScreen{ true };
    bool onScreen2{ true };


    // ---------------------------------------------------------
    // Background
    // ---------------------------------------------------------

    // Texture MUST be declared before Sprite
    sf::Texture m_backgroundTexture;
    sf::Sprite m_backgroundSprite;

    bool m_graphics{ false };


    // ---------------------------------------------------------
    // Rock / Enemy
    // ---------------------------------------------------------

    sf::RectangleShape m_enemy;

    // Texture MUST be declared before Sprite
    sf::Texture m_rockTexture;
    sf::Sprite m_rockSprite;

    sf::Vector2f m_rockLocation;


    // SFML 3 sprites cannot be default constructed.
    // Therefore we use a vector and construct each sprite
    // using the rock texture.
    std::vector<sf::Sprite> m_arrayRocks;

    sf::RectangleShape m_arrayEnemies[MAX_ROCKS];

    sf::Vector2f m_arrayOfRockSpeed[MAX_ROCKS];

    bool m_arrayAliveRocks[MAX_ROCKS];


    // ---------------------------------------------------------
    // Game variables
    // ---------------------------------------------------------

    int score{ 0 };
    int increaseScore{ 0 };

    const int MAX_TIME = 60;

    bool gameOver{ false };
};

#endif // GAME_HPP
