#include "AIFriendlyTank.h"
#include <iostream>

AIFriendlyTank::AIFriendlyTank(AssetManager& t_assetManager, std::vector<sf::Sprite>& t_wallSprites)
	: m_assetManager(t_assetManager),
	m_tankBase(m_assetManager.getTexture("tankAtlas")),
	m_wallSprites(t_wallSprites)
{
	initSprites();
}

void AIFriendlyTank::update(double dt, sf::RenderWindow& m_window)
{
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
		handleKeyInput(m_window);

		if (m_rotate == true)
		{
			sf::Vector2f tankPos = m_tankBase.getPosition();

			float dx = m_targetPosition.x - tankPos.x;
			float dy = m_targetPosition.y - tankPos.y;

			float distance = std::sqrt(dx * dx + dy * dy);

			m_speed = std::clamp(m_speed, MAX_REVERSE_SPEED, MAX_FORWARD_SPEED);
			//If number is bigger than max forward speed number is automatically max speed (same with max reverse speed)

			sf::Vector2f direction(dx / distance, dy / distance);

			m_tankBase.move(direction * static_cast<float>(m_speed * dt / 1000.0));

			// Stop if close enough
			if (distance < 5.f)
			{
				if (added == false)
				{
					moved++;
					added = true;
				}
				m_move = false;
				m_rotate == true;
				m_speed = 0.0f;
			}
		}

		m_tankBase.setRotation(m_rotation);
		break;

	case TankState::COLLIDING:
		break;

	default:
		break;
	}
}

void AIFriendlyTank::render(sf::RenderWindow& window)
{
	window.draw(m_tankBase);
}

void AIFriendlyTank::setPosition(sf::Vector2f t_position)
{
	m_tankBase.setPosition(t_position);
}

sf::Vector2f AIFriendlyTank::getPosition()
{
	return m_tankBase.getPosition();
}


void AIFriendlyTank::setScale(sf::Vector2f t_scale)
{
	m_tankBase.setScale(t_scale);
}

void AIFriendlyTank::increaseSpeed()
{
	m_speed += 1.0f;
}

void AIFriendlyTank::decreaseSpeed()
{
	m_speed -= 1.0f;
}

void AIFriendlyTank::increaseRotation()
{
	m_rotation += sf::degrees(1.0);
	if (m_rotation.asDegrees() == 360.0)
	{
		m_rotation = sf::degrees(0.0);
	}
}

void AIFriendlyTank::decreaseRotation()
{
	m_rotation -= sf::degrees(1.0);
	if (m_rotation.asDegrees() == 0.0)
	{
		m_rotation = sf::degrees(359.0);
	}
}

void AIFriendlyTank::rotateTowardsMouseClick(sf::Vector2f mousePos)
{
	sf::Vector2f tankPos = m_tankBase.getPosition();
	float x = mousePos.x - tankPos.x;
	float y = mousePos.y - tankPos.y;

	sf::Angle angleToMouse = sf::radians(std::atan2(y, x));
	angleToMouse = angleToMouse - sf::degrees(90.0f);
	sf::Angle difference = angleToMouse - m_rotation;

	while (difference > sf::degrees(180.f))
		difference -= sf::degrees(360.f);

	while (difference < sf::degrees(-180.f))
		difference += sf::degrees(360.f);

	m_speed = 30.0f;

	if (std::abs(difference.asDegrees()) > (2))
	{
		if (difference > sf::degrees(0))
		{
			increaseRotation();
		}
		else
		{
			decreaseRotation();
		}
	}
	else
	{
		m_rotate = true;
		added = false;
		m_rotation = sf::Angle(angleToMouse);
		m_targetPosition = mousePos;
	}
}


void AIFriendlyTank::handleKeyInput(sf::RenderWindow& m_window)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
	{
		sf::Vector2i getmousePos = sf::Mouse::getPosition(m_window);
		sf::Vector2f mousePos = m_window.mapPixelToCoords(getmousePos);
		rotateTowardsMouseClick(mousePos);
	}
	else if(m_rotate == false)
	{
		m_move = true;
	}

	//DO NOT USE ELSE IF'S
	//Tank moves with multiple key presses
}

bool AIFriendlyTank::checkWallCollision()
{
	for (sf::Sprite const& sprite : m_wallSprites)
	{
		if (CollisionDetector::collision(m_tankBase, sprite))
		{
			if (CollisionDetector::pixelPerfectTest(m_tankBase, sprite))
			{
				// Get contact normal vector between tank base and the wall
				m_contactNormal = m_tankBase.getPosition() - sprite.getPosition();
				return true;
			}
		}
	}
	return false;
}

sf::Sprite& AIFriendlyTank::getSprite()
{
	return m_tankBase;
}

void AIFriendlyTank::initSprites()
{
	m_tankBase.setTextureRect(sf::IntRect({ 439,0 }, { 96,207 }));
	m_tankBase.setOrigin({ 48, 104});
}
