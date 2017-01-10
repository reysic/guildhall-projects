//#include "Engine/Math/MathUtils.hpp"
#include "Game/AI/SteeringManager.hpp"


//-----------------------------------------------------------------------------------------------
SteeringManager::SteeringManager( Entity* owner )
	: m_owningEntity( owner )
	, m_steering( Vector2( 0.0f, 0.0f ) )
{
}


//-----------------------------------------------------------------------------------------------
SteeringManager::~SteeringManager()
{
	delete m_owningEntity;
	m_owningEntity = nullptr;
}


//-----------------------------------------------------------------------------------------------
void SteeringManager::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void SteeringManager::Seek( const Vector2& targetPosition, float maxAcceleration )
{
	Vector2 myPosition = m_owningEntity->GetPosition();
	Vector2 vectorToTarget = targetPosition - myPosition;
	vectorToTarget.Normalize();

	Vector2 myVelocity = m_owningEntity->GetVelocity();
	m_owningEntity->SetVelocity( myVelocity + ( vectorToTarget * maxAcceleration ) );
}


//-----------------------------------------------------------------------------------------------
void SteeringManager::Flee( const Vector2& targetPosition, float maxAcceleration )
{
	Vector2 myPosition = m_owningEntity->GetPosition();
	Vector2 vectorToTarget = targetPosition - myPosition;
	vectorToTarget.Normalize();
	vectorToTarget.Negate(); // Only difference from Seek

	Vector2 myVelocity = m_owningEntity->GetVelocity();
	m_owningEntity->SetVelocity( myVelocity + ( vectorToTarget * maxAcceleration ) );
}