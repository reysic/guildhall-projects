//-----------------------------------------------------------------------------------------------
// EnemyTank.hpp
// Tank controlled by AI


#pragma once


//----------------------------------------------------------------------------------------------
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
typedef Vector2 WorldCoords;


//-----------------------------------------------------------------------------------------------
const float ENEMY_TANK_SPEED = 0.5f;
const float ENEMY_TANK_DEGREES_TO_TURN_PER_SECOND = 90.0f;
const std::string ENEMY_TANK_TEXTURE_FILE = "Data/Images/EnemyTank.png";
const int ENEMY_TANK_STARTING_HEALTH = 3;
const float ENEMY_TANK_TIME_BETWEEN_GOAL_ORIENTATION_CHANGES_IN_SECONDS = 3.0f;
const float ENEMY_TANK_FIRING_RATE_BULLETS_PER_SECOND = 2.0f;
const float ENEMY_TANK_DETECTION_RADIUS = 5.0f;
const float ENEMY_TANK_HALF_OF_DETECTION_CONE_IN_DEGREES = 5.0f;
const float ENEMY_TANK_HALF_OF_PURSUIT_CONE_IN_DEGREES = 45.0f;


//-----------------------------------------------------------------------------------------------
class EnemyTank : public Entity
{
public:
	// Constructors
	EnemyTank( const WorldCoords& enemyTankPosition );

	// Methods
	void Update( float deltaSeconds );
	void Turn( float deltaSeconds );
	void Render() const;
	bool UsesCorrectivePhysics() const;

private:
	// Methods
	void ShootAtPlayerTank();

	// Members
	float m_goalOrientationDegrees;
	bool m_waitToPickNewGoalOrientation;
	float m_timeBetweenGoalOrientationChanges;
	float m_timeLastGoalOrientationChange;
	bool m_waitToSpawnNewEnemyTankBullet;
	float m_timeBetweenEnemyTankBullets;
	float m_timeLastEnemyTankBulletFired;
};