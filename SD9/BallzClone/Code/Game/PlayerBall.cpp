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
#include "Game/PlayerBall.hpp"


//-----------------------------------------------------------------------------------------------
PlayerBall::PlayerBall()
{
	m_position.x = 350.0f;
	m_position.y = 10.0f;

	m_velocity.x = 0.f;
	m_velocity.y = 0.0f;

	m_orientationDegrees = 0.0f;
	m_angularVelocityDegreesPerSecond = 0.0f;
	m_physicsRadius = 10.0f;
	m_cosmeticRadius = 10.0f;

	m_tint = Rgba::WHITE;

	m_health = 3;

	m_canDrawAimLine = false;
	m_forwardDirection = Vector2::ZERO;
}


//-----------------------------------------------------------------------------------------------
void PlayerBall::Update( float deltaSeconds )
{
	// If not moving
	if ( m_velocity == Vector2( 0.0f, 0.0f ) )
	{
		Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();
		PolarCoordinates xboxLeftStickPositionPolar = g_theApp->GetXboxLeftStickPositionPolar();

		if ( ( xboxLeftStickPosition.Length() > 0 ) && xboxLeftStickPositionPolar.radius > 0.5f )
		{
			// Enable drawing of aim line
			m_canDrawAimLine = true;

			// Calculate forward direction for launching
			float orientationRadians = xboxLeftStickPositionPolar.theta;
			m_forwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );

			if ( g_theApp->GetXboxADownStatus() )
			{
				m_velocity = m_forwardDirection * PLAYER_BALL_SPEED;
			}
		}
		else
		{
			// Disable drawing of aim line
			m_canDrawAimLine = false;
		}
	}
	else
	{
		// Disable drawing of aim line
		m_canDrawAimLine = false;

		// Push down slightly
		// #TODO: Decide if this should always be applied while moving, or just when m_velocity.y
		// is under a certain amount
		m_velocity.y -= 0.01f;
	}

	// Move
	m_position += m_velocity * deltaSeconds;

	CheckForScreenEdgeCollision();
}


//-----------------------------------------------------------------------------------------------
void PlayerBall::Render() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	if ( m_health < 1 )
	{
		return;
	}

	// Draw the ball
	g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_physicsRadius, m_tint );

	// Draw the aiming line
	if ( m_canDrawAimLine )
	{
		Vector2 lineStartPosition( m_position.x, m_position.y );
		Vector2 lineEndPosition( m_position.x + m_forwardDirection.x * 300.0f, m_position.y + m_forwardDirection.y * 300.0f );
		Vector2 aimLineVector = lineEndPosition - lineStartPosition;
		aimLineVector.Normalize();

		for ( float distanceAlongAimLine = 30.0f; distanceAlongAimLine <= 300.0f; distanceAlongAimLine += 30.0f )
		{
			g_theRenderer->DrawFilledPolygon( 20, lineStartPosition.x + distanceAlongAimLine * aimLineVector.x, lineStartPosition.y + distanceAlongAimLine* aimLineVector.y, 5.0f, Rgba::WHITE );
		}
		//g_theRenderer->DrawLine3D( Vector3( m_position.x, m_position.y, 0.0f ), Vector3( lineEndPosition.x, lineEndPosition.y, 0.0f ), Rgba::WHITE, 2.5f );
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerBall::CheckForScreenEdgeCollision()
{
	// Left side of screen
	if ( m_position.x - m_cosmeticRadius < 0.0f )
	{
		m_velocity = Vector2( -m_velocity.x, m_velocity.y );
		m_position.x = 0.0f + m_cosmeticRadius;
	}

	// Right side of screen
	if ( m_position.x + m_cosmeticRadius > 700.0f )
	{
		m_velocity = Vector2( -m_velocity.x, m_velocity.y );
		m_position.x = 700.0f - m_cosmeticRadius;
	}

	// Bottom side of screen
	if ( m_position.y - m_cosmeticRadius < 0.0f )
	{
		// Reset
		m_velocity = Vector2( 0.0f, 0.0f );
		m_position = Vector2( 350.0f, 10.0f );
	}

	// Top side of screen
	if ( m_position.y + m_cosmeticRadius > 900.0f )
	{
		m_velocity = Vector2( m_velocity.x, -m_velocity.y );
		m_position.y = 900.0f - m_cosmeticRadius;
	}
}