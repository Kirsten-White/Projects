#pragma once
#include <SFML/Graphics.hpp>
#include "AssetManager.h"
#include "CollisionDetector.h"
#include "ScreenSize.h"
#include "Bullet.h"
#include <cmath>
/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	
	/// <summary>
	/// @brief Constructor function to initialise sprite members.
	/// <param name="t_assetManager">The asset manager initialised with the texture atlas</param>
	/// </summary>
	Tank(AssetManager & t_assetManager, std::vector<sf::Sprite> & t_wallSprites);
	
	/// <summary>
	/// @brief Handle all the movement and general update logic for the tank.
	/// <param name="dt">The time between frames in milliseconds</param>
	/// </summary>
	void update(double dt);
	
	/// <summary>
	/// @brief To draw the turret and tank base.
	/// <param name="window">The SFML render window</param>
	/// </summary>
	void render(sf::RenderWindow & window);
	
	/// <summary>
	/// @brief Simple setter for the tank base and turret position
	/// <param name="t_position">A position value</param>
	/// </summary>
	void setPosition(sf::Vector2f t_position);
	
	sf::Vector2f getPosition();
	
	/// <summary>
	/// @brief Scaling factor to apply to the tank base and turret
	/// <param name="t_scale">The x,y scaling value</param>
	/// </summary>
	void setScale(sf::Vector2f t_scale);

	//Increase speed
	void increaseSpeed();

	//Decrease speed
	void decreaseSpeed();

	//Increase rotation
	void increaseRotation();

	//Decrease rotation
	void decreaseRotation();

	//Handle key press
	void handleKeyInput();

	//Turrent increase rotation
	void increaseTurrentRotation();

	//Turrent decrease rotation
	void decreaseTurrentRotation();

	//Recentre turrent
	void centreTurrent();

	//Check collision with wall
	bool checkWallCollision();

	//Bounce back from collision
	void deflect(double dt);

	enum class TankState { NORMAL, COLLIDING };

	//Spawn Bullet
	void spawnBullet();

	void firstDamage();

	std::vector<std::unique_ptr<Bullet>>& getBullets();  // Access bullets
	sf::Sprite& getSprite();

	//Damage
	bool firstHit = false;

private:	
	void initSprites();
	
	ScreenSize screen;

	TankState m_state{ TankState::NORMAL };
	AssetManager & m_assetManager;
	std::vector <sf::Sprite>& m_wallSprites;
	sf::Sprite m_tankBase;
	sf::Sprite m_turret;

	Bullet m_bullet;
	std::vector<std::unique_ptr<Bullet>> m_bullets;

	static double constexpr MAX_FORWARD_SPEED{ 100.0 };
	static double constexpr MAX_REVERSE_SPEED{ -100.0 };

	//for collision
	sf::Vector2f m_contactNormal = { 0,0 };

	// The tank speed.
	double m_speed{ 0.0 };

	// The current rotation as applied to tank base.
	sf::Angle m_rotation{ sf::degrees(0.0) };

	//Turrent rotation
	sf::Angle m_turrentRotation{ sf::degrees(0.0) };

	//bool turrent
	bool centreTurrentBool = false;

	float cross = 0.0f;

	//Bullet cooldown
	float m_shootCooldown;
	float m_timeSinceLastShot;
};
