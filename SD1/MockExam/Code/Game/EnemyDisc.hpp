#pragma once


#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
class Vector2; // Forward declaration of Vector2
class Game;


//-----------------------------------------------------------------------------------------------
const float ENEMY_DISC_SPEED = 40.0f;
const std::string ENEMY_DISC_TEXTURE_FILE = "Data/Images/Enemy.png";
const int ENEMY_DISC_STARTING_HEALTH = 1;


//-----------------------------------------------------------------------------------------------
class EnemyDisc : public Entity
{
public:
	EnemyDisc();
	void Update( float deltaSeconds );
	void Render() const;

private:
};