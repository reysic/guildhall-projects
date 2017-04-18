#include <math.h>
#include <stdlib.h>

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/TheGame.hpp"
#include "Game/PlayerBall.hpp"


//-----------------------------------------------------------------------------------------------
PlayerBall::PlayerBall( const Vector2& spawnPosition )
	: m_position( spawnPosition )
	, m_velocity( Vector2::ZERO )
	, m_radius( 10.0f )
	, m_tint( Rgba::WHITE )
	, m_hasBeenShotThisTurn( false )
{
}


//-----------------------------------------------------------------------------------------------
void PlayerBall::Update( float deltaSeconds )
{
	// If moving
	if ( m_velocity != Vector2( 0.0f, 0.0f ) )
	{
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

	// Draw the ball
	g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_radius, m_tint );
}


//-----------------------------------------------------------------------------------------------
void PlayerBall::CheckForScreenEdgeCollision()
{
	// Left side of screen
	if ( ( m_position.x - m_radius ) < 0.0f )
	{
		m_velocity = Vector2( -m_velocity.x, m_velocity.y );
		m_position.x = 0.0f + m_radius;
	}

	// Right side of screen
	if ( ( m_position.x + m_radius ) > 700.0f )
	{
		m_velocity = Vector2( -m_velocity.x, m_velocity.y );
		m_position.x = 700.0f - m_radius;
	}

	// Top side of screen
	if ( ( m_position.y + m_radius ) > 900.0f )
	{
		m_velocity = Vector2( m_velocity.x, -m_velocity.y );
		m_position.y = 900.0f - m_radius;
	}

	// Bottom side of screen
	if ( ( m_position.y - m_radius ) < 0.0f )
	{
		// Reset
		m_velocity = Vector2::ZERO;
		m_position = Vector2( m_position.x, 10.0f );
		g_theGame->SetNewPlayerBallStartingPosition( m_position );

		// Advance turn
		if ( g_theGame->GetGameState() != STATE_MAIN_MENU && g_theGame->AreAllPlayerBallsNotMoving() )
		{
			g_theGame->AdvanceTurn();
		}
	}
}