#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <math.h>
#include <stdlib.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerDisc.hpp"


//-----------------------------------------------------------------------------------------------
PlayerDisc::PlayerDisc()
{
	m_position.x = 800.f;
	m_position.y = 450.f;

	m_velocity.x = 0.f;
	m_velocity.y = 0.f;

	m_orientationDegrees = 0.f;
	m_angularVelocityDegreesPerSecond = 0.f;
	m_physicsRadius = 25.f;
	m_cosmeticRadius = 25.f;

	m_tint = Rgba( 0.3f, 0.1f, 1.0f, 1.0f );

	m_health = 3;
}


//-----------------------------------------------------------------------------------------------
void PlayerDisc::Update( float deltaSeconds )
{
	Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();
	PolarCoordinates xboxLeftStickPositionPolar = g_theApp->GetXboxLeftStickPositionPolar();

	if ( ( xboxLeftStickPosition.Length() > 0 ) && !m_isDead && xboxLeftStickPositionPolar.radius > 0.5f)
	{
		float orientationRadians = xboxLeftStickPositionPolar.theta;
		Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

		m_velocity = forwardDirection * PLAYER_DISC_SPEED * RangeMapZeroToOne( xboxLeftStickPositionPolar.radius, 0.5f, 1.0f );
	}
	else
	{
		m_velocity = new Vector2( 0.0f, 0.0f );
	}

	Entity::Update( deltaSeconds );

	CheckForScreenEdge();
}


//-----------------------------------------------------------------------------------------------
void PlayerDisc::Render() const
{
	if ( m_health < 1 )
		return;
	g_theRenderer->SetDrawColor( m_tint.redValue, m_tint.greenValue, m_tint.blueValue, m_tint.alphaValue );
	g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_physicsRadius );
}


//-----------------------------------------------------------------------------------------------
void PlayerDisc::CheckForScreenEdge()
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