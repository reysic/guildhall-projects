//-----------------------------------------------------------------------------------------------
// Turret.hpp
// Turret that turns toward and fires at player tank


#pragma once


//----------------------------------------------------------------------------------------------
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
typedef Vector2 WorldCoords;


//-----------------------------------------------------------------------------------------------
const float TURRET_DEGREES_TO_TURN_PER_SECOND = 90.0f;
const std::string TURRET_TEXTURE_FILE = "Data/Images/Turret.png";
const float TURRET_FIRING_RATE_BULLETS_PER_SECOND = 1.0f;
const int TURRET_STARTING_HEALTH = 1;
const float TURRET_DETECTION_RADIUS = 5.0f;
const float TURRET_HALF_OF_DETECTION_CONE_IN_DEGREES = 5.0f;


//-----------------------------------------------------------------------------------------------
class Turret : public Entity
{
public:
	// Constructors
	Turret( const WorldCoords& turretPosition );

	// Methods
	void Update( float deltaSeconds );
	void Turn( float deltaSeconds );
	void Render() const;
	bool UsesCorrectivePhysics() const;

private:
	// Methods
	void ShootAtPlayerTank();

	// Members
	bool m_waitToSpawnNewTurretBullet;
	float m_timeBetweenTurretBullets;
	float m_timeLastTurretBulletFired;
	float m_goalOrientationDegrees;
};