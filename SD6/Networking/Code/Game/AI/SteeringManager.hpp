#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class SteeringManager
{
public:
	SteeringManager( Entity* owner );
	~SteeringManager();

	void Update( float deltaSeconds );

	void Seek( const Vector2& targetPosition, float maxAcceleration );
	void Flee( const Vector2& targetPosition, float maxAcceleration );

public:
	Entity* m_owningEntity;
	Vector2 m_steering;
};