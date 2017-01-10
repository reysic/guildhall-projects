#include <math.h>
#include <stdlib.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/Bullet.hpp"


//-----------------------------------------------------------------------------------------------
Bullet::Bullet( const Vector2& shipPosition, float shipOrientationDegrees, float shipCosmeticRadius )
{
	m_position = shipPosition;

	m_orientationDegrees = shipOrientationDegrees;

	// #Eiserloh: Which way is forward?
	float orientationRadians = ConvertDegreesToRadians( m_orientationDegrees );
	Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

	// Move bullet to tip of ship
	m_position += forwardDirection * shipCosmeticRadius;

	m_velocity = forwardDirection * BULLET_SPEED;

	m_angularVelocityDegreesPerSecond = 0.0f;
	m_physicsRadius = 4.0f;
	m_cosmeticRadius = 4.0f;
}


//-----------------------------------------------------------------------------------------------
void Bullet::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	if ( m_ageInSeconds >= 2.0f )
	{
		m_isDead = true;
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void Bullet::Render() const
{
	// Save current "graph paper"
	g_theRenderer->PushView();

	if (g_theGame->GetDebugModeStatus())
	{
		// Draw physics radius
		g_theRenderer->SetLineWidth( 0.25f );
		g_theRenderer->SetDrawColor( Rgba::RED );
		g_theRenderer->DrawHollowPolygon( 20, m_position.x, m_position.y, m_physicsRadius );

		// Draw cosmetic radius
		g_theRenderer->SetLineWidth( 0.25f );
		g_theRenderer->SetDrawColor( Rgba::GREEN );
		g_theRenderer->DrawHollowPolygon( 20, m_position.x, m_position.y, m_cosmeticRadius );
	}

	g_theRenderer->SetLineWidth( 1.5f );
	g_theRenderer->SetDrawColor( Rgba::WHITE );
	g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_cosmeticRadius );

	// Set up the "graph paper"
	g_theRenderer->TranslateView( Vector3( m_position.x, m_position.y, 0.0f ) );
	g_theRenderer->ScaleView( m_cosmeticRadius );
	g_theRenderer->RotateView( m_orientationDegrees, Vector3( 0.0f, 0.0f, 1.0f ) );

	//g_theRenderer->SetLineWidth( 1.5f );
	//g_theRenderer->SetDrawColor( 1.0f, 1.0f, 1.0f, 1.0f );
	//g_theRenderer->DrawHollowPolygon( 20, m_position.x, m_position.y, m_cosmeticRadius );

	/*
	g_theRenderer->DrawLine( Vector2( 0.9f, 0.9f ), Vector2( -0.9f, 0.9f ) );
	g_theRenderer->DrawLine( Vector2( -0.9f, 0.9f ), Vector2( -0.9f, -0.9f ) );
	g_theRenderer->DrawLine( Vector2( -0.9f, -0.9f ), Vector2( 0.9f, -0.9f ) );
	g_theRenderer->DrawLine( Vector2( 0.9f, -0.9f ), Vector2( 0.9f, 0.9f ) );
	*/

	// Restore the "graph paper"
	g_theRenderer->PopView();
}