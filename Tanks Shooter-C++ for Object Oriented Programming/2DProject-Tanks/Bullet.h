#pragma once
#include <SFML/Graphics.hpp>
#include "AssetManager.h"
#include "CollisionDetector.h"
#include <cmath>

class Bullet
{
public:
	Bullet(const sf::Texture& texture, std::vector<sf::Sprite>& t_wallSprites);

	void update(double dt);

	void render(sf::RenderWindow& window);

	void setPosition(sf::Vector2f t_position);

	sf::Vector2f getPosition();

	//Check collision with wall
	bool checkWallCollision();

	//Check with tank
	bool checkTankCollision(sf::Sprite& sprite);

	//Spawn Bullet
	void spawnBullet(sf::Vector2f position, sf::Angle rotation);

	//despawn Bullet
	void despawnBullet();

	//Move bullet
	void moveBullet();

private:
	void initSprites();

	//AssetManager& m_assetManager;
	std::vector <sf::Sprite>& m_wallSprites;
	sf::Sprite m_bullet;

	//for collision
	sf::Vector2f m_contactNormal = { 0,0 };

	// The current rotation as applied to tank base.
	sf::Angle m_rotation{ sf::degrees(0.0) };

	//Turrent rotation
	sf::Angle m_turrentRotation{ sf::degrees(0.0) };

	//Bullet
	bool fire = false;
	bool collision = false;
	sf::Vector2f bulletLocation{ 1000,1000 };
};