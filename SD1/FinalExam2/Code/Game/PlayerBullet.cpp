#include <math.h>
#include <stdlib.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerBullet.hpp"


//-----------------------------------------------------------------------------------------------
PlayerBullet::PlayerBullet( const Vector2& playerPosition, float xboxRightStickPositionPolarThetaRadians )
{
	m_position = playerPosition;
	m_orientationDegrees = 0.0f;

	Vector2 forwardDirection( cos( xboxRightStickPositionPolarThetaRadians ), sin( xboxRightStickPositionPolarThetaRadians ) );

	m_velocity = forwardDirection * PLAYER_BULLET_SPEED;

	m_angularVelocityDegreesPerSecond = 0.0f;

	m_physicsRadius = 5.0f;
	m_cosmeticRadius = 5.0f;

	m_tint = Rgba( 0.0f, 0.75f, 1.0f, 1.0f );

	m_health = 1;
}


//-----------------------------------------------------------------------------------------------
void PlayerBullet::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	if ( m_ageInSeconds >= 1.0f )
	{
		m_isDead = true;
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerBullet::Render() const
{
	g_theRenderer->SetDrawColor( m_tint.redValue, m_tint.greenValue, m_tint.blueValue, m_tint.alphaValue );
	g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_physicsRadius );
}