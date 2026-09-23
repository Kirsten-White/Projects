#include "Bullet.h"
#include <iostream>

Bullet::Bullet(const sf::Texture& texture, std::vector<sf::Sprite>& t_wallSprites)
	: m_wallSprites(t_wallSprites),
	m_bullet(texture)
{
	initSprites();
}

void Bullet::update(double dt)
{
	if (fire == true)
	{
		collision = checkWallCollision();
		if (collision == false)
		{
			moveBullet();
		}
		else
		{
			fire = false;
			despawnBullet();
		}
	}
}

void Bullet::render(sf::RenderWindow& window)
{
	window.draw(m_bullet);
}

void Bullet::setPosition(sf::Vector2f t_position)
{
	m_bullet.setPosition({ t_position });
}

sf::Vector2f Bullet::getPosition()
{
	return m_bullet.getPosition();
}

bool Bullet::checkWallCollision()
{
	for (sf::Sprite & sprite : m_wallSprites)
	{
		// Checks if the tank base or turret has collided with 1 of the wall sprites
		if (CollisionDetector::collision(m_bullet, sprite))
		{
			// Now perform a pixel perfect collision test...
			if (CollisionDetector::pixelPerfectTest(m_bullet, sprite))
			{
				// Get contact normal vector
				m_contactNormal = m_bullet.getPosition() - sprite.getPosition();

				sf::Vector2f currentScale = sprite.getScale();
				if (currentScale.x > 0.3f && currentScale.y > 0.3f)
				{
					sprite.setScale({ currentScale.x * 0.9f, currentScale.y * 0.9f });
				}
				else
				{
					sprite.setScale({ 0.0f, 0.0f });
				}
				return true;
			}
		}
	}
	return false;
}

bool Bullet::checkTankCollision(sf::Sprite& sprite)
{	
	if (CollisionDetector::pixelPerfectTest(m_bullet, sprite))
	{
		return true;
	}
	return false;
}

void Bullet::spawnBullet(sf::Vector2f position, sf::Angle rotation)
{
	bulletLocation = { position };
	m_bullet.setRotation(rotation);
	m_bullet.setPosition(bulletLocation);
	fire = true;
}

void Bullet::despawnBullet()
{
	m_bullet.setPosition({ 1000,1000 });
}

void Bullet::moveBullet()
{
	float y = std::sin(m_bullet.getRotation().asRadians());
	float x = std::cos(m_bullet.getRotation().asRadians());
	bulletLocation += { (x * 5.0f), (y * 5.0f)};
	m_bullet.setPosition(bulletLocation);
}

void Bullet::initSprites()
{
	m_bullet.setTextureRect(sf::IntRect({ 247,95 }, { 128, 128 }));
	m_bullet.setOrigin({ 64, 64 });
}
