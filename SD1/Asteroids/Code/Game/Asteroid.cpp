#include <math.h>
#include <stdlib.h>

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/Asteroid.hpp"


//-----------------------------------------------------------------------------------------------
Asteroid::Asteroid()
{
	int randomBool = rand() % 2;

	if ( randomBool == 0 )
	{
		// Spawn along left edge of screen
		m_position.x = 0.f - m_cosmeticRadius;
		m_position.y = GetRandomFloatInRange( m_cosmeticRadius, 900.f - m_cosmeticRadius );
	}
	else
	{
		// Spawn along top edge of screen
		m_position.x = GetRandomFloatInRange( m_cosmeticRadius, 1600.f - m_cosmeticRadius );
		m_position.y = 0.f + m_cosmeticRadius;
	}


	float vectorX = GetRandomFloatInRange( -1.0f, 1.0f );
	float vectorY = GetRandomFloatInRange( -1.0f, 1.0f );

	float vectorLength = sqrt( vectorX * vectorX + vectorY * vectorY );

	vectorX = vectorX / vectorLength;
	vectorY = vectorY / vectorLength;

	vectorX = vectorX * 100.f;
	vectorY = vectorY * 100.f;

	m_velocity.x = vectorX;
	m_velocity.y = vectorY;

	m_orientationDegrees = 0.f;
	m_angularVelocityDegreesPerSecond = GetRandomFloatInRange( -50.f, 50.f);
	m_physicsRadius = 50.f;
	m_cosmeticRadius = 60.f;

	m_asteroidSize = 3; // Default constructor spawns large asteroids

	randomBool = rand() % 4;

	switch ( randomBool )
	{
		case 0:
			m_asteroidType = 1;
			break;
		case 1:
			m_asteroidType = 2;
			break;
		case 2:
			m_asteroidType = 3;
			break;
		case 3:
			m_asteroidType = 4;
			break;
	}
}


//-----------------------------------------------------------------------------------------------
Asteroid::Asteroid( const Vector2 & spawnPosition, int asteroidSize )
{
	m_position = spawnPosition;

	float vectorX = GetRandomFloatInRange( -1.0f, 1.0f );
	float vectorY = GetRandomFloatInRange( -1.0f, 1.0f );

	float vectorLength = sqrt( vectorX * vectorX + vectorY * vectorY );

	vectorX = vectorX / vectorLength;
	vectorY = vectorY / vectorLength;

	vectorX = vectorX * 100.f;
	vectorY = vectorY * 100.f;

	m_velocity.x = vectorX;
	m_velocity.y = vectorY;

	m_orientationDegrees = 0.f;
	m_angularVelocityDegreesPerSecond = GetRandomFloatInRange( -50.f, 50.f );

	switch ( asteroidSize )
	{
		case 2:
			m_physicsRadius = 35.f;
			m_cosmeticRadius = 45.f;
			break;
		case 1:
			m_physicsRadius = 15.f;
			m_cosmeticRadius = 20.f;
			break;
	}

	m_asteroidSize = asteroidSize;

	int randomBool = rand() % 4;

	switch (randomBool)
	{
	case 0:
		m_asteroidType = 1;
		break;
	case 1:
		m_asteroidType = 2;
		break;
	case 2:
		m_asteroidType = 3;
		break;
	case 3:
		m_asteroidType = 4;
		break;
	}
}


//-----------------------------------------------------------------------------------------------
void Asteroid::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	Turn( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Asteroid::Turn( float deltaSeconds )
{
	float degreesToTurnThisFrame = m_angularVelocityDegreesPerSecond * deltaSeconds;
	m_orientationDegrees += degreesToTurnThisFrame;
}


//-----------------------------------------------------------------------------------------------
void Asteroid::Render() const
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

	// Set up the "graph paper"
	g_theRenderer->TranslateView( Vector3( m_position.x, m_position.y, 0.0f ) );
	g_theRenderer->ScaleView( m_cosmeticRadius );
	g_theRenderer->RotateView( m_orientationDegrees, Vector3( 0.0f, 0.0f, 1.0f ) );

	g_theRenderer->SetLineWidth( 0.25f );
	g_theRenderer->SetDrawColor( Rgba::WHITE );
	switch ( m_asteroidType )
	{
		case 1:
			g_theRenderer->DrawLine3D( Vector3( 0.4f, 0.9f, 0.0f ), Vector3( -0.4f, 0.7f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.4f, 0.7f, 0.0f ), Vector3( -0.5f, 0.1f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.5f, 0.1f, 0.0f ), Vector3( -0.8f, -0.2f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.8f, -0.2f, 0.0f ), Vector3( -0.4f, -0.6f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.4f, -0.6f, 0.0f ), Vector3( 0.2f, -0.7f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.2f, -0.7f, 0.0f ), Vector3( 0.7f, -0.5f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.7f, -0.5f, 0.0f ), Vector3( 0.8f, -0.1f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.8f, -0.1f, 0.0f ), Vector3( 0.5f, 0.1f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.5f, 0.1f, 0.0f ), Vector3( 0.4f, 0.9f, 0.0f ), Rgba::WHITE, 0.25f );

			break;
		case 2:
			g_theRenderer->DrawLine3D( Vector3( 0.8f, 0.4f, 0.0f ), Vector3( 0.3f, 0.4f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.3f, 0.4f, 0.0f ), Vector3( 0.2f, 0.9f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.2f, 0.9f, 0.0f ), Vector3( -0.1f, 0.8f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.1f, 0.8f, 0.0f ), Vector3( -0.2f, 0.3f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.2f, 0.3f, 0.0f ), Vector3( -0.8f, 0.2f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.8f, 0.2f, 0.0f ), Vector3( -0.9f, -0.3f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.9f, -0.3f, 0.0f ), Vector3( -0.4f, -0.3f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.4f, -0.3f, 0.0f ), Vector3( -0.3f, -0.9f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.3f, -0.9f, 0.0f ), Vector3( 0.3f, -0.9f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.3f, -0.9f, 0.0f ), Vector3( 0.4f, -0.5f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.4f, -0.5f, 0.0f ), Vector3( 0.8f, -0.3f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.8f, -0.3f, 0.0f ), Vector3( 0.8f, 0.4f, 0.0f ), Rgba::WHITE, 0.25f );
			break;
		case 3:
			g_theRenderer->DrawLine3D( Vector3( 0.8f, 0.3f, 0.0f ), Vector3( 0.2f, 0.3f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.2f, 0.3f, 0.0f ), Vector3( 0.7f, 0.7f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.7f, 0.7f, 0.0f ), Vector3( 0.6f, 0.9f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.6f, 0.9f, 0.0f ), Vector3( 0.0f, 1.0f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.0f, 1.0f, 0.0f ), Vector3( -0.8f, 0.3f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.8f, 0.3f, 0.0f ), Vector3( -0.9f, -0.2f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.9f, -0.2f, 0.0f ), Vector3( -0.1f, -0.1f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.1f, -0.1f, 0.0f ), Vector3( -0.4f, -0.8f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.4f, -0.8f, 0.0f ), Vector3( 0.4f, -0.7f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.4f, -0.7f, 0.0f ), Vector3( 0.8f, -0.4f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.8f, -0.4f, 0.0f ), Vector3( 0.8f, 0.3f, 0.0f ), Rgba::WHITE, 0.25f );
			break;
		case 4:
			g_theRenderer->DrawLine3D( Vector3( 0.6f, 0.6f, 0.0f ), Vector3( -0.3f, 0.8f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.3f, 0.8f, 0.0f ), Vector3( -0.7f, 0.6f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.7f, 0.6f, 0.0f ), Vector3( -0.8f, -0.3f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.8f, -0.3f, 0.0f ), Vector3( -0.8f, -0.7f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.8f, -0.7f, 0.0f ), Vector3( -0.4f, -0.7f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( -0.4f, -0.7f, 0.0f ), Vector3( 0.1f, -0.2f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.1f, -0.2f, 0.0f ), Vector3( 0.6f, -0.4f, 0.0f ), Rgba::WHITE, 0.25f );

			g_theRenderer->DrawLine3D( Vector3( 0.6f, -0.4f, 0.0f ), Vector3( 0.6f, 0.6f, 0.0f ), Rgba::WHITE, 0.25f );
			break;
	}
	
	// Restore the "graph paper"
	g_theRenderer->PopView();
}