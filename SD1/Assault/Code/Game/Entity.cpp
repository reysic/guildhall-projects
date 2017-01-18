//-----------------------------------------------------------------------------------------------
// Entity.cpp
// Base class for in-game entities


//-----------------------------------------------------------------------------------------------
#include <math.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/Entity.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity()
	: m_isDead( false )
	, m_ageInSeconds( 0.0f )
	, m_correctivePhysicsAppliedThisFrame( false )
{
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
}


//-----------------------------------------------------------------------------------------------
void Entity::Render() const
{
}


//-----------------------------------------------------------------------------------------------
bool Entity::UsesCorrectivePhysics() const
{
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Entity::IsProjectile() const
{
	return false;
}


//-----------------------------------------------------------------------------------------------
Vector2 Entity::GetPosition() const
{
	return m_position;
}


//-----------------------------------------------------------------------------------------------
float Entity::GetOrientationDegrees() const
{
	return m_orientationDegrees;
}


//-----------------------------------------------------------------------------------------------
float Entity::GetPhysicsRadius() const
{
	return m_physicsRadius;
}


//-----------------------------------------------------------------------------------------------
float Entity::GetCosmeticRadius() const
{
	return m_cosmeticRadius;
}


//-----------------------------------------------------------------------------------------------
bool Entity::GetIsDead() const
{
	return m_isDead;
}


//-----------------------------------------------------------------------------------------------
AABB2 Entity::GetBoundingBox() const
{
	return m_bounds;
}

//-----------------------------------------------------------------------------------------------
Texture* Entity::GetTexture() const
{
	return m_texture;
}


//-----------------------------------------------------------------------------------------------
Rgba Entity::GetTint() const
{
	return m_tint;
}


//-----------------------------------------------------------------------------------------------
int Entity::GetHealth() const
{
	return m_health;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetPosition( Vector2 newPosition )
{
	m_position = newPosition;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetVelocity( Vector2 newVelocity )
{
	m_velocity = newVelocity;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetOrientationDegrees( float newOrientationDegrees )
{
	m_orientationDegrees = newOrientationDegrees;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetAngularVelocityDegreesPerSecond( float newAngularVelocityDegreesPerSecond )
{
	m_angularVelocityDegreesPerSecond = newAngularVelocityDegreesPerSecond;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetPhysicsRadius( float newPhysicsRadius )
{
	m_physicsRadius = newPhysicsRadius;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetCosmeticRadius( float newCosmeticRadius )
{
	m_cosmeticRadius = newCosmeticRadius;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetIsDead( bool newIsDead )
{
	m_isDead = newIsDead;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetBoundingBoxMins( Vector2 newBoundingBoxMins )
{
	m_bounds.mins = newBoundingBoxMins;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetBoundingBoxMaxs( Vector2 newBoundingBoxMaxs )
{
	m_bounds.maxs = newBoundingBoxMaxs;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetTexture( Texture* newTexture )
{
	m_texture = newTexture;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetTint( Rgba newTint )
{
	m_tint = newTint;
}


//-----------------------------------------------------------------------------------------------
void Entity::SetHealth( int newHealth )
{
	m_health = newHealth;
}