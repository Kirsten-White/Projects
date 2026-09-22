#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>
#include <vector>

const int GAME_WIDTH = 1000;
const int GAME_HEIGHT = 800;

/// <summary>
/// Default constructor
/// Setup the window properties
/// Load and setup the text
/// Load and setup the image
/// </summary>
Game::Game() :
    m_window{ sf::VideoMode({ GAME_WIDTH, GAME_HEIGHT }, 32U), "Jellyfish Game" },
    m_exitGame{ false },
    m_scoreDisplay{ m_ArialBlackfont },
    m_gameover{ m_ArialBlackfont },
    m_backgroundSprite{ m_backgroundTexture },
    m_jellyfishSprite{ m_jellyfishTexture },
    m_rockSprite{ m_rockTexture }
{
    setupFontAndText();
    setupSprite();
    arrayStart();
}

/// <summary>
/// Default destructor
/// </summary>
Game::~Game()
{
}

/// <summary>
/// Main game loop
/// </summary>
void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    const float fps{ 60.0f };
    sf::Time timePerFrame = sf::seconds(1.0f / fps);

    while (m_window.isOpen())
    {
        processEvents();

        timeSinceLastUpdate += clock.restart();

        while (timeSinceLastUpdate > timePerFrame)
        {
            timeSinceLastUpdate -= timePerFrame;

            processEvents();
            update(timePerFrame);

#ifdef _DEBUG
            render();
#endif
        }

        render();
    }
}

/// <summary>
/// Handle user and system events/input
/// </summary>
void Game::processEvents()
{
    while (const std::optional event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_exitGame = true;
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            processKeys(keyPressed->code);
        }
    }
}

/// <summary>
/// Deal with key presses from the user
/// </summary>
void Game::processKeys(sf::Keyboard::Key t_key)
{
    if (t_key == sf::Keyboard::Key::Escape)
    {
        m_exitGame = true;
    }

    if (t_key == sf::Keyboard::Key::G)
    {
        m_graphics = !m_graphics;
    }

    if (t_key == sf::Keyboard::Key::Down)
    {
        movePlayerDown();
    }

    if (t_key == sf::Keyboard::Key::Up)
    {
        movePlayerUp();
    }
}

/// <summary>
/// Update the game world
/// </summary>
void Game::update(sf::Time t_deltaTime)
{
    if (t_deltaTime.asMilliseconds() != 16)
    {
        std::cout << "time warp" << std::endl;
    }

    if (m_exitGame)
    {
        m_window.close();
    }

    move();
    resetRock();
}

/// <summary>
/// Draw the frame
/// </summary>
void Game::render()
{
    m_window.clear(sf::Color::White);

    if (checkCollistion())
    {
        m_window.clear(sf::Color::White);
        m_window.draw(m_gameover);

        gameOver = true;

        m_window.display();
    }
    else
    {
        if (!gameOver)
        {
            if (m_graphics)
            {
                m_window.draw(m_backgroundSprite);
                m_window.draw(m_jellyfishSprite);

                for (int i = 0; i < MAX_ROCKS; i++)
                {
                    m_window.draw(m_arrayRocks[i]);
                }
            }
            else
            {
                m_window.draw(m_player);

                for (int i = 0; i < MAX_ROCKS; i++)
                {
                    m_window.draw(m_arrayEnemies[i]);
                }
            }

            m_window.draw(m_scoreDisplay);
        }

        m_window.display();
    }
}

/// <summary>
/// Load the font and setup the text
/// </summary>
void Game::setupFontAndText()
{
    if (!m_ArialBlackfont.openFromFile("ASSETS\\FONTS\\ariblk.ttf"))
    {
        std::cout << "problem loading arial black font" << std::endl;
    }

    m_scoreDisplay.setString("Score: " + std::to_string(score));

    m_scoreDisplay.setStyle(
        sf::Text::Underlined |
        sf::Text::Italic |
        sf::Text::Bold
    );

    m_scoreDisplay.setPosition({ 40.0f, 40.0f });
    m_scoreDisplay.setCharacterSize(20U);
    m_scoreDisplay.setOutlineColor(sf::Color::Red);
    m_scoreDisplay.setFillColor(sf::Color::Black);
    m_scoreDisplay.setOutlineThickness(3.0f);

    m_gameover.setString("Game Over");
    m_gameover.setCharacterSize(100U);
    m_gameover.setPosition({ 200.0f, 200.0f });
    m_gameover.setFillColor(sf::Color::Black);
}

/// <summary>
/// Load the textures and setup the sprites
/// </summary>
void Game::setupSprite()
{
    // Background setup
    if (!m_backgroundTexture.loadFromFile(
        "ASSETS/IMAGES/background(SpringProject).jpg")) //C:\Users\Kirsten\Desktop\CV\Portfolio\Jellyfish Swim- C++ for Game design\SFML_Playground-main\ASSETS\IMAGES
    {
        std::cout << "problem loading background image" << std::endl;
    }

    m_backgroundSprite.setTexture(m_backgroundTexture, true);
    m_backgroundSprite.setScale({ 2.5f, 2.5f });

    m_backgroundSprite.setScale({ 2.5f, 2.5f });

    // Setup box for jellyfish
    m_player.setSize({ 90.0f, 120.0f });
    m_player.setFillColor(sf::Color::Cyan);

    m_jellyfishLocation =
    { 150.0f, GAME_HEIGHT / 2.5f };

    m_player.setPosition(m_jellyfishLocation);

    // Setup jellyfish texture
    if (!m_jellyfishTexture.loadFromFile(
        "ASSETS/IMAGES/JellyFish(SpringProject).png"))
    {
        std::cout << "problem loading jellyfish" << std::endl;
    }

    m_jellyfishSprite.setPosition(m_jellyfishLocation);

    // Setup box for rock
    m_enemy.setSize({ 90.0f, 155.0f });
    m_enemy.setFillColor(sf::Color::Magenta);

    m_rockLocation =
    { GAME_WIDTH - 100.0f, GAME_HEIGHT / 2.5f };

    m_enemy.setPosition(m_rockLocation);

    // Setup rock texture
    if (!m_rockTexture.loadFromFile(
        "ASSETS/IMAGES/Rock(SpringProject).png"))
    {
        std::cout << "problem loading rock" << std::endl;
    }

    m_jellyfishSprite.setTexture(m_jellyfishTexture, true);
    m_jellyfishSprite.setPosition(m_jellyfishLocation);


    m_rockSprite.setPosition(m_rockLocation);
}

void Game::movePlayerDown()
{
    checkBottomBorder();

    if (onScreen2)
    {
        m_jellyfishLocation += m_jellyfishSpeed;

        m_player.setPosition(m_jellyfishLocation);
        m_jellyfishSprite.setPosition(m_jellyfishLocation);
    }
}

void Game::movePlayerUp()
{
    checkTopBorder();

    if (onScreen)
    {
        m_jellyfishLocation -= m_jellyfishSpeed;

        m_player.setPosition(m_jellyfishLocation);
        m_jellyfishSprite.setPosition(m_jellyfishLocation);
    }
}

bool Game::checkTopBorder()
{
    if (m_jellyfishLocation.y <= 0)
    {
        onScreen = false;
    }
    else
    {
        onScreen = true;
    }

    return onScreen;
}

bool Game::checkBottomBorder()
{
    if ((m_jellyfishLocation.y + 119.0f) >= GAME_HEIGHT)
    {
        onScreen2 = false;
    }
    else
    {
        onScreen2 = true;
    }

    return onScreen2;
}

void Game::arrayStart()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Create the rock sprites now that the texture exists.
    m_arrayRocks.clear();

    for (int i = 0; i < MAX_ROCKS; i++)
    {
        m_arrayRocks.emplace_back(m_rockTexture);

        sf::Vector2f randomPos =
        {
            GAME_WIDTH - 10.0f,
            static_cast<float>((std::rand() % GAME_HEIGHT) + 1)
        };

        m_arrayEnemies[i] = m_enemy;

        m_arrayRocks[i].setPosition(randomPos);
        m_arrayEnemies[i].setPosition(randomPos);

        float speedX =
            static_cast<float>(std::rand() % 5 + 1);

        m_arrayOfRockSpeed[i].x = speedX;

        m_arrayAliveRocks[i] = false;
    }
}

void Game::move()
{
    for (int i = 0; i < MAX_ROCKS; i++)
    {
        if (m_arrayAliveRocks[i])
        {
            sf::Vector2f pos = m_arrayRocks[i].getPosition();
            sf::Vector2f speed = m_arrayOfRockSpeed[i];

            pos.x -= speed.x;

            m_arrayEnemies[i].setPosition(pos);
            m_arrayRocks[i].setPosition(pos);

            if (pos.x < 0)
            {
                m_arrayAliveRocks[i] = false;
            }
        }
    }
}

void Game::resetRock()
{
    for (int i = 0; i < MAX_ROCKS; i++)
    {
        if (!m_arrayAliveRocks[i])
        {
            setScore();

            sf::Vector2f pos =
            {
                GAME_WIDTH - 10.0f,
                static_cast<float>((std::rand() % GAME_HEIGHT) + 1)
            };

            m_arrayRocks[i].setPosition(pos);
            m_arrayEnemies[i].setPosition(pos);

            float speedX =
                static_cast<float>(std::rand() % 5 + 1);

            m_arrayOfRockSpeed[i].x = speedX;

            m_arrayAliveRocks[i] = true;
        }
    }
}

bool Game::checkCollistion()
{
    bool touch = false;

    for (int i = 0; i < MAX_ROCKS; i++)
    {
        if (m_arrayEnemies[i]
            .getGlobalBounds()
            .findIntersection(m_player.getGlobalBounds()))
        {
            touch = true;
        }
    }

    return touch;
}

void Game::setScore()
{
    score++;

    m_scoreDisplay.setString(
        "Score: " + std::to_string(score)
    );
}
