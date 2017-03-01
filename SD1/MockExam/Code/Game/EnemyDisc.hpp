#pragma once


#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
class Vector2; // Forward declaration of Vector2
class Game;


//-----------------------------------------------------------------------------------------------
class EnemyDisc : public Entity
{
public:
	EnemyDisc();
	void Update( float deltaSeconds );
	void Render() const;

private:
};