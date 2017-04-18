#include <math.h>
#include <stdlib.h>

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/TheGame.hpp"
#include "Game/IndicatorBall.hpp"


//-----------------------------------------------------------------------------------------------
IndicatorBall::IndicatorBall()
	: m_position( PLAYER_BALL_INITIAL_POSITION )
	, m_velocity( Vector2::ZERO )
	, m_radius( 10.0f )
	, m_tint( Rgba::LIGHTOLIVEGREEN )
{
}


//-----------------------------------------------------------------------------------------------
void IndicatorBall::Update( float deltaSeconds )
{
	// Bottom side of screen
	if ( ( m_position.y - m_radius ) < 0.0f )
	{
		m_velocity = Vector2::ZERO;
		m_position = Vector2( m_position.x, 10.0f );
	}
	else
	{
		m_velocity.y -= 10.0f;
	}

	// Move
	m_position += m_velocity * deltaSeconds;

}


//-----------------------------------------------------------------------------------------------
void IndicatorBall::Render() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	// Draw the ball
	g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_radius, m_tint );
}