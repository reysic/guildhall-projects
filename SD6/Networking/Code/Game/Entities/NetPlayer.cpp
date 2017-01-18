#include "Game/Entities/NetPlayer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/DeveloperConsole.hpp"


//-----------------------------------------------------------------------------------------------
NetPlayer::NetPlayer( uint8_t ownerConnectionIndex, uint16_t netID, const Vector2& position )
	: NetObject( ownerConnectionIndex, netID, position )
{
	m_cosmeticRadius = 20.0f;

	if ( m_ownerConnectionIndex == 0 )
	{
		m_position.x = 600.0f;
	}
	else if ( m_ownerConnectionIndex == 1 )
	{
		m_position.x = 1000.0f;
	}
	else if ( m_ownerConnectionIndex == 2 )
	{
		m_position.y = 250.0f;
	}
}


//-----------------------------------------------------------------------------------------------
void NetPlayer::Update( float deltaSeconds )
{
	Vector2 xboxLeftStickPosition = g_theInputSystem->GetXboxLeftStickPosition( m_ownerConnectionIndex );
	PolarCoordinates xboxLeftStickPositionPolar = g_theInputSystem->GetXboxLeftStickPositionPolar( m_ownerConnectionIndex );

	if ( ( xboxLeftStickPosition.Length() > 0 ) && !m_isDead && xboxLeftStickPositionPolar.radius > 0.5f )
	{
		float orientationRadians = xboxLeftStickPositionPolar.theta;
		Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

		m_velocity = forwardDirection * NET_PLAYER_SPEED * RangeMapZeroToOne( xboxLeftStickPositionPolar.radius, 0.5f, 1.0f );
	}
	else
	{
		m_velocity = new Vector2( 0.0f, 0.0f );
	}

	NetObject::Update( deltaSeconds );

	CheckForScreenEdge();
}


//-----------------------------------------------------------------------------------------------
void NetPlayer::Render() const
{
	if ( m_ownerConnectionIndex == 0 )
	{
		g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y,
			m_cosmeticRadius, Rgba::RED );
	}
	else if ( m_ownerConnectionIndex == 1 )
	{
		g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y,
			m_cosmeticRadius, Rgba::GREEN );
	}
	else if ( m_ownerConnectionIndex == 2 )
	{
		g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y,
			m_cosmeticRadius, Rgba::BLUE );
	}
	else
	{
		g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y,
			m_cosmeticRadius, Rgba::MAGENTA );
	}
}


//-----------------------------------------------------------------------------------------------
void NetPlayer::CheckForScreenEdge()
{
	if ( m_position.x - m_cosmeticRadius < 0.0f )
	{
		m_position.x = 0.0f + m_cosmeticRadius;
	}
	if ( m_position.x + m_cosmeticRadius > 1600.0f )
	{
		m_position.x = 1600.0f - m_cosmeticRadius;
	}
	if ( m_position.y - m_cosmeticRadius < 0.0f )
	{
		m_position.y = 0.0f + m_cosmeticRadius;
	}
	if ( m_position.y + m_cosmeticRadius > 900.0f )
	{
		m_position.y = 900.0f - m_cosmeticRadius;
	}
}
