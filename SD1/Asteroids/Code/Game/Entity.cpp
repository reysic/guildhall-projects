#include <math.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/Entity.hpp"
#include "Game/Ship.hpp"
#include "Game/Asteroid.hpp"


//-----------------------------------------------------------------------------------------------
Entity::Entity()
	: m_isDead( false )
	, m_ageInSeconds( 0.0f )
{
}


//-----------------------------------------------------------------------------------------------
void Entity::Update( float deltaSeconds )
{
	m_ageInSeconds += deltaSeconds;

	CheckForWrapAroundWorld();

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
void Entity::CheckForWrapAroundWorld()
{
	if (m_position.x + m_cosmeticRadius < 0.f)
	{
		m_position.x = 1600.f;
	}
	if (m_position.x - m_cosmeticRadius > 1600.f)
	{
		m_position.x = 0.f;
	}
	if (m_position.y + m_cosmeticRadius < 0.f)
	{
		m_position.y = 900.f;
	}
	if (m_position.y - m_cosmeticRadius > 900.f)
	{
		m_position.y = 0.f;
	}
}