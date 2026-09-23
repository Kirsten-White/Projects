#include "LevelLoader.h"

/// The various operator >> overloads below are non-member functions used to extract
///  the game data from the YAML data structure.

/// <summary>
/// @brief Extracts the obstacle type, position and rotation values.
/// 
/// </summary>
/// <param name="t_obstacleNode">A YAML node</param>
/// <param name="t_obstacle">A simple struct to store the obstacle data</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& t_obstacleNode, ObstacleData& t_obstacle)
{
	t_obstacle.m_type = t_obstacleNode["type"].as<std::string>();
	t_obstacle.m_position.x = t_obstacleNode["position"]["x"].as<float>();
	t_obstacle.m_position.y = t_obstacleNode["position"]["y"].as<float>();
	t_obstacle.m_rotation = sf::degrees(t_obstacleNode["rotation"].as<float>());	
}

/// <summary>
/// @brief Extracts the filename for the game background texture.
/// 
/// </summary>
/// <param name="t_backgroundNode">A YAML node</param>
/// <param name="t_background">A simple struct to store background related data</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& t_backgroundNode, BackgroundData& t_background)
{
	t_background.m_fileName = t_backgroundNode["file"].as<std::string>();
}

/// <summary>
/// @brief Extracts the initial screen position for the player tank.
/// 
/// </summary>
/// <param name="t_tankNode">A YAML node</param>
/// <param name="t_tank">A simple struct to store data related to the player tank</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& t_tankNode, TankData& t_tank)
{
	t_tank.m_position.x = t_tankNode["position"]["x"].as<float>(); //Sets x position
	t_tank.m_position.y = t_tankNode["position"]["y"].as<float>(); //Sets y position
	t_tank.m_scale.x = t_tankNode["scale"]["x"].as<float>();
	t_tank.m_scale.y = t_tankNode["scale"]["y"].as<float>();
}

/*void operator >> (const YAML::Node& t_tankEnemyNode, std::vector<TankEnemyData>& t_enemyTanks)
{
	for (unsigned i = 0; i < t_tankEnemyNode.size(); ++i)
	{
		TankEnemyData tankEnemy;
		tankEnemy.m_position.x = t_tankEnemyNode[i]["position"]["x"].as<float>();
		tankEnemy.m_position.y = t_tankEnemyNode[i]["position"]["y"].as<float>();
		tankEnemy.m_scale.x = t_tankEnemyNode[i]["scale"]["x"].as<float>();
		tankEnemy.m_scale.y = t_tankEnemyNode[i]["scale"]["y"].as<float>();
		t_enemyTanks.push_back(tankEnemy);
	}
}*/

void operator >> (const YAML::Node& t_tankEnemyNode, TankEnemyData& t_tankEnemy)
{
	t_tankEnemy.m_position.x = t_tankEnemyNode["position"]["x"].as<float>(); //Sets x position
	t_tankEnemy.m_position.y = t_tankEnemyNode["position"]["y"].as<float>(); //Sets y position
	t_tankEnemy.m_scale.x = t_tankEnemyNode["scale"]["x"].as<float>();
	t_tankEnemy.m_scale.y = t_tankEnemyNode["scale"]["y"].as<float>();
}

void operator >> (const YAML::Node& t_AIFriendlyTankNode, AIFriendlyTankData& t_AIFriendlyTank)
{
	t_AIFriendlyTank.m_position.x = t_AIFriendlyTankNode["position"]["x"].as<float>(); //Sets x position
	t_AIFriendlyTank.m_position.y = t_AIFriendlyTankNode["position"]["y"].as<float>(); //Sets y position
	t_AIFriendlyTank.m_scale.x = t_AIFriendlyTankNode["scale"]["x"].as<float>();
	t_AIFriendlyTank.m_scale.y = t_AIFriendlyTankNode["scale"]["y"].as<float>();
}
/// <summary>
/// @brief Top level function that extracts various game data from the YAML data stucture.
/// 
/// Invokes other functions to extract the background, tank and obstacle data.
//   Because there are multiple obstacles, obstacle data are stored in a vector.
/// </summary>
/// <param name="t_levelNode">A YAML node</param>
/// <param name="t_level">A simple struct to store level data for the game</param>
////////////////////////////////////////////////////////////
void operator >> (const YAML::Node& t_levelNode, LevelData& t_level)
{
	t_levelNode["background"] >> t_level.m_background; //links to 2nd void operator

	t_levelNode["tank"] >> t_level.m_tank;  //links to 3rd void operator

	t_levelNode["AIFriendlyTank"] >> t_level.m_AIFriendlyTank;

	const YAML::Node& enemyNode = t_levelNode["tankEnemy"];

	for (unsigned i = 0; i < enemyNode.size(); ++i)
	{
		TankEnemyData enemy;
		enemyNode[i] >> enemy;  // uses your existing operator >>
		t_level.m_enemyTanks.push_back(enemy);
	}
	//t_levelNode["tankEnemy"] >> t_level.m_tankEnemy;

	const YAML::Node& obstaclesNode = t_levelNode["obstacles"].as<YAML::Node>();
	for (unsigned i = 0; i < obstaclesNode.size(); ++i)
	{
		ObstacleData obstacle;
		obstaclesNode[i] >> obstacle;
		t_level.m_obstacles.push_back(obstacle);
	}//links to 1st void operator
}

////////////////////////////////////////////////////////////
void LevelLoader::load(int t_levelNr, LevelData& t_level)
{
	std::string filename = "./resources/levelData/level" + std::to_string(t_levelNr) + ".yaml";

	try
	{
		YAML::Node baseNode = YAML::LoadFile(filename);
		if (baseNode.IsNull())
		{
			std::string message("File: " + filename + " not found");
			throw std::exception(message.c_str());
		}
		baseNode >> t_level; //links to 4th/last void operator
	}
	catch (YAML::ParserException& e)
	{
		std::string message(e.what());
		message = "YAML Parser Error: " + message;
		throw std::exception(message.c_str());
	}
	catch (std::exception& e)
	{
		std::string message(e.what());
		message = "Unexpected Error: " + message;
		throw std::exception(message.c_str());
	}
}


