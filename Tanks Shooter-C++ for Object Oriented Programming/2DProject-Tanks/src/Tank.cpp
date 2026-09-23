#include "Tank.h"
#include <iostream>

Tank::Tank(AssetManager& t_assetManager, std::vector<sf::Sprite>& t_wallSprites)
	: m_assetManager(t_assetManager),
	m_tankBase(m_assetManager.getTexture("tankAtlas")),
	m_turret(m_assetManager.getTexture("tankAtlas")),
	m_bullet(m_assetManager.getTexture("tankAtlas"), m_wallSprites),
	m_wallSprites(t_wallSprites),
	m_shootCooldown(0.5f),
	m_timeSinceLastShot(0.0f)
{
	initSprites();
}

void Tank::update(double dt)
{	
	m_timeSinceLastShot += static_cast<float>(dt) / 1000;
	
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
		handleKeyInput();

		if (centreTurrentBool == true)
		{
			centreTurrent();
		}

		if (firstHit == true)
		{
			firstDamage();
		}

		m_speed = std::clamp(m_speed, MAX_REVERSE_SPEED, MAX_FORWARD_SPEED);
		//If number is bigger than max forward speed number is automatically max speed (same with max reverse speed)

		m_tankBase.setPosition({ m_tankBase.getPosition().x + cos(m_rotation.asRadians()) * static_cast<float>(m_speed * (dt / 1000)) , m_tankBase.getPosition().y + sin(m_rotation.asRadians()) * static_cast<float>(m_speed * (dt / 1000)) });
		m_tankBase.setRotation(m_rotation);

		m_turret.setPosition(m_tankBase.getPosition());
		m_turret.setRotation(m_turrentRotation + m_rotation);

		m_speed = m_speed * 0.999;
		break;

	case TankState::COLLIDING:
		deflect(dt);
		break;

	default:
		break;
	}
}

void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_tankBase);
	window.draw(m_turret);
	for (auto& bullet : m_bullets)
	{
		bullet->render(window);
	}
}

void Tank::setPosition(sf::Vector2f t_position)
{
	m_tankBase.setPosition(t_position);
	m_turret.setPosition(t_position);
}

sf::Vector2f Tank::getPosition()
{
	return m_tankBase.getPosition();
}


void Tank::setScale(sf::Vector2f t_scale)
{
	m_tankBase.setScale(t_scale);
	m_turret.setScale(t_scale);
}

void Tank::increaseSpeed()
{
	m_speed += 1;
}

void Tank::decreaseSpeed()
{
	m_speed -= 1;
}

void Tank::increaseRotation()
{
	m_rotation += sf::degrees(1.0);
	if (m_rotation.asDegrees() == 360.0)
	{
		m_rotation = sf::degrees(0.0);
	}
}

void Tank::decreaseRotation()
{
	m_rotation -= sf::degrees(1.0);
	if (m_rotation.asDegrees() == 0.0)
	{
		m_rotation = sf::degrees(359.0);
	}
}

void Tank::handleKeyInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
	{
		// Up key was pressed...
		increaseSpeed();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
	{
		decreaseSpeed();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
	{
		decreaseRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
	{
		increaseRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
	{
		increaseTurrentRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X))
	{
		decreaseTurrentRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C))
	{
		centreTurrentBool = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && m_timeSinceLastShot >= m_shootCooldown)
	{
		spawnBullet();
		m_timeSinceLastShot = 0.0f;
	}

	//DO NOT USE ELSE IF'S
	//Tank moves with multiple key presses
}

void Tank::increaseTurrentRotation()
{
	m_turrentRotation += sf::degrees(1.0);
	if (m_turrentRotation.asDegrees() == 360.0)
	{
		m_turrentRotation = sf::degrees(0.0);
	}
}

void Tank::decreaseTurrentRotation()
{
	m_turrentRotation -= sf::degrees(1.0);
	if (m_turrentRotation.asDegrees() == 0.0)
	{
		m_turrentRotation = sf::degrees(359.0);
	}
}

void Tank::centreTurrent()
{
	sf::Vector2f tankDir{ std::cos(m_rotation.asRadians()/730), std::sin(m_rotation.asRadians()/730) };
	sf::Vector2f turrentDir{ std::cos(m_turrentRotation.asRadians()), std::sin(m_turrentRotation.asRadians()) };

	cross = (tankDir.x * turrentDir.y) - (tankDir.y * turrentDir.x);

	if (std::abs(cross) <= 0.01)
	{
		cross = 0.0;
		centreTurrentBool = false;		
	}
	else if (cross > 0)
	{
		decreaseTurrentRotation();
	}
	else
	{
		increaseTurrentRotation();
	}
}

bool Tank::checkWallCollision()
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

void Tank::deflect(double dt)
{
	// Calculate push back based on speed
	float impulseMagnitude = std::abs(m_speed);

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

void Tank::spawnBullet()
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

void Tank::firstDamage()
{
	m_turrentRotation += sf::degrees(0.1f);
}

std::vector<std::unique_ptr<Bullet>>& Tank::getBullets()
{
	return m_bullets;
}

sf::Sprite& Tank::getSprite()
{
	return m_tankBase;
}

void Tank::initSprites()
{
	m_tankBase.setTextureRect(sf::IntRect({ 0,0 }, { 246 , 114 }));
	m_tankBase.setOrigin({ 98,55 });
	
	m_turret.setTextureRect(sf::IntRect({ 247,0 }, { 191 , 94 }));
	m_turret.setOrigin({ 47, 45 });
}
