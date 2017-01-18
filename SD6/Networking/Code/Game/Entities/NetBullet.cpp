#include "Game/Entities/NetBullet.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/DeveloperConsole.hpp"


//-----------------------------------------------------------------------------------------------
NetBullet::NetBullet( uint8_t ownerConnectionIndex, uint16_t netID, const Vector2& position, 
	float xboxRightStickPositionPolarThetaRadians )
	: NetObject( ownerConnectionIndex, netID, position )
{
	m_cosmeticRadius = 5.0f;

	Vector2 forwardDirection( cos( xboxRightStickPositionPolarThetaRadians ), 
		sin( xboxRightStickPositionPolarThetaRadians ) );

	m_velocity = forwardDirection * NET_BULLET_SPEED;
}


//-----------------------------------------------------------------------------------------------
void NetBullet::Update( float deltaSeconds )
{
	NetObject::Update( deltaSeconds );

	if ( m_ageInSeconds >= 1.0f )
	{
		m_isDead = true;
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void NetBullet::Render() const
{
	if ( m_ownerConnectionIndex == 0 )
	{
		g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_cosmeticRadius, Rgba::RED );
	}
	else if ( m_ownerConnectionIndex == 1 )
	{
		g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_cosmeticRadius, Rgba::GREEN );
	}
	else if ( m_ownerConnectionIndex == 2 )
	{
		g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_cosmeticRadius, Rgba::BLUE );
	}
	else
	{
		g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_cosmeticRadius, Rgba::MAGENTA );
	}
}