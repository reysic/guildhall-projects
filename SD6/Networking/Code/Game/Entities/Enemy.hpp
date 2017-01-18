#pragma once

#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
// Forward Declarations
class SteeringManager;


//-----------------------------------------------------------------------------------------------
enum SteeringBehavior
{
	STEERING_BEHAVIOR_NONE = 0,
	STEERING_BEHAVIOR_WANDER,
	STEERING_BEHAVIOR_SEEK,
	STEERING_BEHAVIOR_PURSUE,
	STEERING_BEHAVIOR_PATH_FOLLOW,
	NUM_STEERING_BEHAVIORS
};


//-----------------------------------------------------------------------------------------------
class Enemy : public Entity
{
public:
	// Constructors & Destructors
	Enemy( const WorldCoords& enemyCarPosition, EntityType type );
	~Enemy();

	// Methods
	void Update( float deltaSeconds );
	void UpdateTintBasedOnRemainingHealth();
	void UpdateSteering( float deltaSeconds );
	void Render() const;
	bool UsesCorrectivePhysics() const { return true; }
	void Wander( float deltaSeconds );
	void Seek( float deltaSeconds );
	void Arrive( float deltaSeconds );
	void ApplySeparation( float deltaSeconds );
	void ApplyCollisionAvoidance( float deltaSeconds );
	void ApplyObstacleAvoidance( float deltaSeconds );
	void Pursue( float deltaSeconds );
	void FaceTarget( float deltaSeconds, const Vector2& targetPosition );
	void PathFollow( float deltaSeconds );

private:
	// Methods
	virtual void Shoot();

	// Members
	float m_goalOrientationDegrees;
	float m_satisfactionRadius;
	float m_slowRadius;
	float m_timeBetweenShots;
	float m_timeLastShotFired;
	bool m_waitToShoot;
	SteeringManager* m_steeringManager;
	SteeringBehavior m_steeringBehavior;
};