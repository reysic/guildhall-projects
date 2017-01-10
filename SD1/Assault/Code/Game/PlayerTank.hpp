//-----------------------------------------------------------------------------------------------
// PlayerTank.hpp
// Tank controlled by player


#pragma once


//-----------------------------------------------------------------------------------------------
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
typedef Vector2 WorldCoords;


//-----------------------------------------------------------------------------------------------
const float TANK_SPEED = 3.0f;
const float TANK_DEGREES_TO_TURN_PER_SECOND = 90.0f;
const std::string TANK_TEXTURE_FILE = "Data/Images/PlayerTank.png";
const int PLAYER_TANK_STARTING_HEALTH = 10;


//-----------------------------------------------------------------------------------------------
class PlayerTank : public Entity
{
public:
	// Constructors
	PlayerTank( const WorldCoords& turretPosition );

	// Methods
	void Update( float deltaSeconds );
	void MoveForward();
	void MoveBackward();
	void Turn( float deltaSeconds );
	void Render() const;
	bool UsesCorrectivePhysics() const;
	void Respawn();

	bool m_isCloaked;

private:
	// Members
	Rgba m_tint;
};