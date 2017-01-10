#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity()
	: m_isDead( false )
	, m_ageInSeconds( 0.0f )
	, m_correctivePhysicsAppliedThisFrame( false )
{
}


//-----------------------------------------------------------------------------------------------
Entity::~Entity()
{
// 	delete m_texture;
// 	m_texture = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Entity::Update( float deltaSeconds )
{
	m_ageInSeconds += deltaSeconds;

	Move( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Entity::Move( float deltaSeconds )
{
	m_position += m_velocity * deltaSeconds;
	m_orientationDegrees += m_angularVelocityDegreesPerSecond * deltaSeconds;
}