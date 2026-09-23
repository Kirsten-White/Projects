#include "Game.h"
#include <iostream>

// Our target FPS
static float const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game(AssetManager & t_assetManager)	
	: m_window(sf::VideoMode({ ScreenSize::s_width, ScreenSize::s_height }, 32), "SFML Playground", sf::Style::Default),
	m_assetManager(t_assetManager), m_tank(t_assetManager, m_wallSprites), m_timerText(m_arialFont),
	m_gameOverText(m_arialFont), m_highScore(m_arialFont), m_AIFriendlyTank(t_assetManager, m_wallSprites)
{
	init();
}

////////////////////////////////////////////////////////////
void Game::init()
{
	int currentLevel = 1;

	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch(std::exception & e)
	{
		std::cout << "Level Laoding failure." << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}
	
	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);

	if (!m_arialFont.openFromFile("BebasNeue.otf"))
	{
		std::cerr << "Error loading font file";
		return;
	}

	m_timeRemaining = sf::seconds(1 * 60);

	m_timerText.setFont(m_arialFont);
	m_timerText.setCharacterSize(24); // Size of the timer text
	m_timerText.setFillColor(sf::Color::White); // White color for the timer text
	m_timerText.setPosition({ 20.f, 20.f });

	m_assetManager.loadTexture("backGround", "resources/images/Background.jpg");
	sf::Texture const& bgtexture = m_assetManager.getTexture("backGround");
	m_bgSprite.setTexture(bgtexture, true);

#ifdef TEST_FPS
	m_tank.setPosition(m_level.m_tank.m_position);
	m_tank.setScale(m_level.m_tank.m_scale);

	m_AIFriendlyTank.setPosition(m_level.m_AIFriendlyTank.m_position);
	m_AIFriendlyTank.setScale(m_level.m_AIFriendlyTank.m_scale);

	for (auto const& enemyData : m_level.m_enemyTanks)
	{
		m_enemyTanks.push_back(
			std::make_unique<TankEnemy>(m_assetManager, m_wallSprites)
		);

		m_enemyTanks.back()->setPosition(enemyData.m_position);
		m_enemyTanks.back()->setSpawnPosition(enemyData.m_position);
		m_enemyTanks.back()->setScale(enemyData.m_scale);
	}

	generateWalls();

	x_updateFPS.setFont(m_arialFont);
	x_updateFPS.setPosition(sf::Vector2f{ 20.0f, 300.0f });
	x_updateFPS.setCharacterSize(24);
	x_updateFPS.setFillColor(sf::Color::White);
	x_drawFPS.setFont(m_arialFont);
	x_drawFPS.setPosition(sf::Vector2f{ 20.0f, 350.0f });
	x_drawFPS.setCharacterSize(24);
	x_drawFPS.setFillColor(sf::Color::White);

	//m_assetManager.releaseTexture("tankAtlas");
	//We get a debug error
#endif
}

////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	
	sf::Time timePerFrame = sf::seconds(1.0f / FPS); // 60 fps

	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame.asMilliseconds()); //60 fps
#ifdef TEST_FPS
			x_secondTime += timePerFrame;
			x_updateFrameCount++;
			if (x_secondTime.asSeconds() > 1)
			{
				std::string updatesPS = "UPS " + std::to_string(x_updateFrameCount - 1);
				x_updateFPS.setString(updatesPS);
				std::string drawsPS = "DPS " + std::to_string(x_drawFrameCount);
				x_drawFPS.setString(drawsPS);
				x_updateFrameCount = 0;
				x_drawFrameCount = 0;
				x_secondTime = sf::Time::Zero;
			}
#endif
		}
		render(); // as many as possible
#ifdef TEST_FPS
		x_drawFrameCount++;
#endif
	}
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{

	// get the next event from the event queue (if there is one)
	while (const std::optional event = m_window.pollEvent())
	{
		// "close requested" event: we close the window
		if (event->is<sf::Event::Closed>())
		{
			m_window.close();
		}
		if (event->is<sf::Event::KeyPressed>())
		{
			processKeyPressed(event);
		}		
	}
}

////////////////////////////////////////////////////////////
void Game::processKeyPressed(const std::optional<sf::Event>& t_event)
{	
	
	sf::Event::KeyPressed const* keyPressed = t_event->getIf<sf::Event::KeyPressed>();
	if (keyPressed != nullptr)
	{
		switch (keyPressed->code)
		{
			case sf::Keyboard::Key::Escape:
				m_window.close();
				break;
			default:
				break;
		}
	}
}

////////////////////////////////////////////////////////////
void Game::generateWalls()
{
	sf::IntRect wallRect{ sf::Vector2i(213,251) , sf::Vector2i(32,22) };
	sf::Texture const& texture = m_assetManager.getTexture("tankAtlas");
	for (auto const& obstacle : m_level.m_obstacles)
	{
		sf::Sprite sprite(texture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(sf::Vector2f{ static_cast<float>(wallRect.size.x),static_cast<float>(wallRect.size.y) } / 2.0f);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	if (gameOver == false)
	{
		m_tank.update(dt);
		m_AIFriendlyTank.update(dt, m_window);
		//m_tankEnemy.update(dt, m_tank.getPosition());
		for (auto& enemy : m_enemyTanks)
		{
			enemy->update(dt, m_tank.getPosition());
		}

		for (auto& bullet : m_tank.getBullets()) {
			bullet->update(dt);
		}
		for (auto& enemy : m_enemyTanks)
		{
			for (auto& bullet : enemy->getBullets()) {
				bullet->update(dt);
			}
		}

		if (m_AIFriendlyTank.moved > lastMoved)
		{
			m_Score += 10;
		}
		lastMoved = m_AIFriendlyTank.moved;

		m_highScore.setPosition({ 15, 100 });
		m_highScore.setString("Score: " + std::to_string(m_Score));

		checkCollision(dt);

		m_timeRemaining -= sf::seconds(static_cast<float>(dt) / 1000.0f);

		if (m_timeRemaining < sf::Time::Zero)
		{
			m_timeRemaining = sf::Time::Zero;
			gameOver = true;
		}

		int seconds = static_cast<int>(m_timeRemaining.asSeconds());
		int minutes = seconds / 60;
		seconds = seconds % 60;

		m_timerText.setString(
			"Time: " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds)
		);
	}
}

void Game::checkCollision(double dt)
{
	///Bullet collision off screen
	//DO NOT TOUCH
	for (auto it = m_tank.getBullets().begin(); it != m_tank.getBullets().end();)
	{
		(*it)->update(dt);

		if ((*it)->getPosition().x < 0 || (*it)->getPosition().x > ScreenSize::s_width || (*it)->getPosition().y < 0 || (*it)->getPosition().y > ScreenSize::s_height)
		{
			it = m_tank.getBullets().erase(it);
		}
		else
		{
			++it;
		}
	}
	
	for (auto& enemy : m_enemyTanks)
	{
		for (auto it = enemy->getBullets().begin(); it != enemy->getBullets().end();)
		{
			(*it)->update(dt);

			if ((*it)->getPosition().x < 0 || (*it)->getPosition().x > ScreenSize::s_width || (*it)->getPosition().y < 0 || (*it)->getPosition().y > ScreenSize::s_height)
			{
				it = enemy->getBullets().erase(it);
			}
			else
			{
				++it;
			}
		}
		
		for (auto it = m_tank.getBullets().begin(); it != m_tank.getBullets().end();)
		{
			bool isDeadEnemy = false;
			if ((*it)->checkTankCollision(enemy->getSprite())) {
				enemy->m_isDead = true;
				m_Score += 5;
				it = m_tank.getBullets().erase(it);  // Remove the bullet if it hits the enemy tank
			}
			else {
				++it;  // Continue to next bullet if no collision
			}
		}

		for (auto it = enemy->getBullets().begin(); it != enemy->getBullets().end();)
		{
			if ((*it)->checkTankCollision(m_tank.getSprite())) {
				m_tank.firstHit = true;
				m_Score -= 5;
				it = enemy->getBullets().erase(it);  // Remove the bullet if it hits the player tank
			}
			else {
				++it;  // Continue to next bullet if no collision
			}
		}

		for (auto it = enemy->getBullets().begin(); it != enemy->getBullets().end();)
		{
			if ((*it)->checkTankCollision(m_AIFriendlyTank.getSprite())) {
				m_Score -= 35;
				it = enemy->getBullets().erase(it);  // Remove the bullet if it hits the player tank
			}
			else {
				++it;  // Continue to next bullet if no collision
			}
		}
	}
}

////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
#ifdef TEST_FPS
	if (gameOver == false)
	{
		m_window.draw(m_bgSprite);
		for (sf::Sprite sprite : m_wallSprites)
		{
			m_window.draw(sprite);
		}
		m_window.draw(x_updateFPS);
		m_window.draw(x_drawFPS);
		m_tank.render(m_window);
		m_AIFriendlyTank.render(m_window);
		//m_tankEnemy.render(m_window);
		for (auto& enemy : m_enemyTanks)
		{
			enemy->render(m_window);
		}
		m_window.draw(m_timerText);
		m_window.draw(m_highScore);
	}
	else
	{
		displayGameOverScreen();
	}
#endif
	m_window.display();
}

void Game::displayGameOverScreen()
{
	if (m_Score <= 0)
	{
		m_gameOverText.setFillColor(sf::Color::Red);
		m_highScore.setFillColor(sf::Color::Red);
	}
	else
	{
		m_gameOverText.setFillColor(sf::Color::Green);
		m_highScore.setFillColor(sf::Color::Green);
	}
	
	m_gameOverText.setFont(m_arialFont);
	m_gameOverText.setCharacterSize(96);  // Big font for game over message
	m_gameOverText.setString("Game Over");
	m_gameOverText.setPosition({ ScreenSize::s_width / 2.5f, 300 });

	m_highScore.setFont(m_arialFont);
	m_highScore.setCharacterSize(48);  // Big font for game over message
	m_highScore.setString("Score: " + std::to_string(m_Score));
	m_highScore.setPosition({ ScreenSize::s_width / 2.5f, 400 });

	m_window.draw(m_gameOverText);
	m_window.draw(m_highScore);
}
