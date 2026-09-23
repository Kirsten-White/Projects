#include "EnemyTank.h"
#include <iostream>

TankEnemy::TankEnemy(AssetManager& t_assetManager, std::vector<sf::Sprite>& t_wallSprites)
	: m_assetManager(t_assetManager),
	m_tankBase(m_assetManager.getTexture("tankAtlas")),
	m_turret(m_assetManager.getTexture("tankAtlas")),
	m_bullet(m_assetManager.getTexture("tankAtlas"), m_wallSprites),
	m_wallSprites(t_wallSprites),
	m_shootCooldown(0.5f),
	m_timeSinceLastShot(0.0f),
	m_isDead(false),
	m_respawnTime(3.0f),
	m_respawnTimer(0.0f)
{
	initSprites();
}

void TankEnemy::update(double dt, sf::Vector2f playerPos)
{
	if (m_isDead == true)
	{
		m_respawnTimer += static_cast<float>(dt) / 1000.0f;

		if (m_respawnTimer >= m_respawnTime)
		{
			respawnTank();
		}

		return;
	}
	else
	{
		m_timeSinceLastShot += static_cast<float>(dt) / 1000;

		m_aiDecisionTimer += static_cast<float>(dt) / 1000;

		if (m_aiDecisionTimer >= 1.0f)
		{
			m_aiDecisionTimer = 0.0f;

			int moveChoice = std::rand() % 2;

			switch (moveChoice)
			{
			case 2: 
			case 10: increaseSpeed(); break;
			////
			case 11: decreaseSpeed(); break;
			case 8: 
			case 6:
			case 1: moveTowardsPlayer(playerPos); break;
			////
			case 7: increaseTurrentRotation(); break;
			case 4: decreaseTurrentRotation(); break;
			////
			case 9: 
			case 3:
			case 5:
			case 0:
				lineupTurrent(playerPos); break;
			}

			// 60% chance to shoot
			int shootChance = std::rand() % 100;
			if (shootChance > 40)
			{
				spawnBullet();
			}
		}

		if (checkWallCollision())
		{
			m_state = TankState::COLLIDING;
		}
		else
		{
			m_state = TankState::NORMAL;
		}

		switch (m_state)
		{
		case TankState::NORMAL:

			m_speed = std::clamp(m_speed, MAX_REVERSE_SPEED, MAX_FORWARD_SPEED);
			//If number is bigger than max forward speed number is automatically max speed (same with max reverse speed)

			m_tankBase.setPosition({ m_tankBase.getPosition().x + cos(m_rotation.asRadians()) * static_cast<float>(m_speed * (dt / 1000)) , m_tankBase.getPosition().y + sin(m_rotation.asRadians()) * static_cast<float>(m_speed * (dt / 1000)) });
			m_tankBase.setRotation(m_rotation);

			m_turret.setPosition(m_tankBase.getPosition());
			m_turret.setRotation(m_turrentRotation + m_rotation);

			break;

		case TankState::COLLIDING:
			deflect(dt);
			break;

		default:
			break;
		}
	}
}

void TankEnemy::render(sf::RenderWindow& window)
{
	if (m_isDead == false)
	{
		window.draw(m_tankBase);
		window.draw(m_turret);
		for (auto& bullet : m_bullets)
		{
			bullet->render(window);
		}
	}
}

void TankEnemy::setPosition(sf::Vector2f t_position)
{
	m_tankBase.setPosition(t_position);
	m_turret.setPosition(t_position);
}

sf::Vector2f TankEnemy::getPosition()
{
	return m_tankBase.getPosition();
}


void TankEnemy::setScale(sf::Vector2f t_scale)
{
	m_tankBase.setScale(t_scale);
	m_turret.setScale(t_scale);
}

void TankEnemy::increaseSpeed()
{
	m_speed += 7;
}

void TankEnemy::decreaseSpeed()
{
	m_speed -= 4;
}

void TankEnemy::increaseRotation()
{
	m_rotation += sf::degrees(18.0);
	if (m_rotation.asDegrees() == 360.0)
	{
		m_rotation = sf::degrees(0.0);
	}
}

void TankEnemy::decreaseRotation()
{
	m_rotation -= sf::degrees(18.0);
	if (m_rotation.asDegrees() == 0.0)
	{
		m_rotation = sf::degrees(359.0);
	}
}

void TankEnemy::increaseTurrentRotation()
{
	m_turrentRotation += sf::degrees(15.0);
	if (m_turrentRotation.asDegrees() == 360.0)
	{
		m_turrentRotation = sf::degrees(0.0);
	}
}

void TankEnemy::decreaseTurrentRotation()
{
	m_turrentRotation -= sf::degrees(15.0);
	if (m_turrentRotation.asDegrees() == 0.0)
	{
		m_turrentRotation = sf::degrees(359.0);
	}
}

void TankEnemy::lineupTurrent(sf::Vector2f playerPos)
{
	sf::Vector2f toPlayer = playerPos - m_tankBase.getPosition();

	float desiredAngle = std::atan2(toPlayer.y, toPlayer.x);

	// Convert world angle to local turret rotation
	float turretTarget = desiredAngle - m_rotation.asRadians();

	float current = m_turrentRotation.asRadians();

	float difference = turretTarget - current;

	// Keep shortest rotation direction
	if (difference > 3.14159f) difference -= 2 * 3.14159f;
	if (difference < -3.14159f) difference += 2 * 3.14159f;

	float rotationSpeed = 5.0f; // radians per second

	if (difference > 0.05f)
	{
		m_turrentRotation += sf::radians(rotationSpeed * 0.016f);
	}
	else if (difference < -0.05f)
	{
		m_turrentRotation -= sf::radians(rotationSpeed * 0.016f);
	}

}

void TankEnemy::moveTowardsPlayer(sf::Vector2f playerPos)
{
	sf::Vector2f toPlayer = playerPos - m_tankBase.getPosition();

	//Get angle toward player
	sf::Angle desiredAngle = sf::radians(std::atan2(toPlayer.y, toPlayer.x));

	//Rotate smoothly toward player
	float difference = desiredAngle.asDegrees() - m_rotation.asDegrees();

	// Keep shortest turn direction
	if (difference > 180.0f) difference -= 360.0f;
	if (difference < -180.0f) difference += 360.0f;

	if (difference > 1.0f)
	{
		increaseRotation();
	}
	else if (difference < -1.0f)
	{
		decreaseRotation();
	}

	increaseSpeed();
}

bool TankEnemy::checkWallCollision()
{
	for (sf::Sprite const& sprite : m_wallSprites)
	{
		// Checks if the tank base or turret has collided with 1 of the wall sprites
		if (CollisionDetector::collision(m_turret, sprite))
		{
			// Now perform a pixel perfect collision test...
			if (CollisionDetector::pixelPerfectTest(m_turret, sprite))
			{
				// Get contact normal vector
				m_contactNormal = m_turret.getPosition() - sprite.getPosition();

				return true;
			}
		}

		//Turrent wall collision is not acerate because this is based on a square on square collision
		else if (CollisionDetector::collision(m_tankBase, sprite))
		{
			// Get contact normal vector between tank base and the wall
			m_contactNormal = m_tankBase.getPosition() - sprite.getPosition();

			return true;
		}
	}
	return false;
}

void TankEnemy::deflect(double dt)
{
	// Calculate push back based on speed
	float impulseMagnitude = std::abs(m_speed + 30);

	// Normalise the contact vector
	m_contactNormal = m_contactNormal.normalized();

	// Calculate deflect amount
	sf::Vector2f deflectVector =
	{
		impulseMagnitude * m_contactNormal.x * (static_cast<float>(dt) / 1000),
		impulseMagnitude * m_contactNormal.y * (static_cast<float>(dt) / 1000)
	};

	// Apply push back along the contact normal
	m_tankBase.move(sf::Vector2f{ deflectVector.x, deflectVector.y });
	m_turret.move(sf::Vector2f{ deflectVector.x, deflectVector.y });
}

void TankEnemy::spawnBullet()
{
	if (m_bullets.size() >= 5)
	{
		return; //Do not spawn bullet if >= 5
	}

	float y = std::sin(m_turrentRotation.asRadians() + m_rotation.asRadians());
	float x = std::cos(m_turrentRotation.asRadians() + m_rotation.asRadians());

	sf::Angle angle = (m_turrentRotation + m_rotation);

	sf::Vector2f spawnPos =
	{
		m_tankBase.getPosition().x + (75 * x),
		m_tankBase.getPosition().y + (50 * y)
	};

	auto newBullet = std::make_unique<Bullet>(m_assetManager.getTexture("tankAtlas"), m_wallSprites);
	newBullet->spawnBullet(spawnPos, angle);
	m_bullets.push_back(std::move(newBullet));

	//m_bullet.spawnBullet(spawnPos, angle);
}

void TankEnemy::respawnTank()
{
	// Wait for a few seconds before respawning
	if (m_respawnTimer >= 2.0f) // 2-second delay before respawn
	{
		setPosition(m_spawnPosition);
		m_speed = 0.0f;
		m_state = TankState::NORMAL;
		m_isDead = false;
		m_respawnTimer = 0.0f;  // Reset the dead time
	}
	else 
	{
		m_respawnTimer += 1;  // Update the dead time
	}
}

bool TankEnemy::getAlive()
{
	return m_isDead;
}

std::vector<std::unique_ptr<Bullet>>& TankEnemy::getBullets()
{
	return m_bullets;
}

sf::Sprite& TankEnemy::getSprite()
{
	return m_tankBase;
}

void TankEnemy::setSpawnPosition(sf::Vector2f position)
{
	m_spawnPosition = position;
}

void TankEnemy::initSprites()
{
	m_tankBase.setTextureRect(sf::IntRect({ 0,136 }, { 244,114 }));
	m_tankBase.setOrigin({ 98,55 });

	m_turret.setTextureRect(sf::IntRect({ 0,251 }, { 212,94 }));
	m_turret.setOrigin({ 47, 45 });

	std::srand(static_cast<unsigned>(std::time(nullptr)));
}

