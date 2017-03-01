#include <math.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerDisc.hpp"
#include "Game/EnemyDisc.hpp"


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