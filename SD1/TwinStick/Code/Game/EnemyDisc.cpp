#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <math.h>
#include <stdlib.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/EnemyDisc.hpp"


//-----------------------------------------------------------------------------------------------
const float ENEMY_DISC_SPEED = 40.0f;
const char* ENEMY_DISC_TEXTURE_FILE = "Data/Images/Enemy.png";
const int ENEMY_DISC_STARTING_HEALTH = 1;


//-----------------------------------------------------------------------------------------------
EnemyDisc::EnemyDisc()
{
	int randomBool = rand() % 4;

	if ( randomBool == 0 )
	{
		// Spawn along left edge of screen
		m_position.x = -25.0f;
		m_position.y = GetRandomFloatInRange( 25.0f, 875.0f );
	}
	else if ( randomBool == 1 )
	{
		// Spawn along top edge of screen
		m_position.x = GetRandomFloatInRange( 25.0f, 1575.0f );
		m_position.y = 925.0f;
	}
	else if ( randomBool == 3 )
	{
		// Spawn along right edge of screen
		m_position.x = 1625.0f;
		m_position.y = GetRandomFloatInRange( 25.0f, 875.0f );
	}
	else
	{
		// Spawn along bottom edge of screen
		m_position.x = GetRandomFloatInRange( 25.0f, 1575.0f );
		m_position.y = -25.0f;
	}

	m_velocity.x = 0.f;
	m_velocity.y = 0.f;

	m_orientationDegrees = 0.f;
	m_angularVelocityDegreesPerSecond = 0.f;
	m_physicsRadius = 25.f;
	m_cosmeticRadius = 25.f;

	m_texture = Texture::CreateOrGetTexture( ENEMY_DISC_TEXTURE_FILE );
	m_tint = Rgba( 247, 117, 10, 255 );

	m_health = 1;

	m_bounds = AABB2( Vector2( -25.0f, -25.0f ), Vector2( 25.0f, 25.0f ) );
}


//-----------------------------------------------------------------------------------------------
void EnemyDisc::Update( float deltaSeconds )
{
	if ( g_theGame->IsPlayerDiskAlive() )
	{
		Vector2 vectorToPlayerDisc = g_theGame->GetPlayerDiskPosition() - m_position;
		m_orientationDegrees = ConvertRadiansToDegrees( atan2( vectorToPlayerDisc.y, vectorToPlayerDisc.x ) );

		float orientationRadians = ConvertDegreesToRadians( m_orientationDegrees );
		Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

		m_velocity = forwardDirection * ENEMY_DISC_SPEED;
	}
	else
	{
		float orientationRadians = ConvertDegreesToRadians( m_orientationDegrees );
		Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

		m_velocity = forwardDirection * ENEMY_DISC_SPEED;
	}

	Entity::Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void EnemyDisc::Render() const
{
	g_theRenderer->DrawFilledPolygon( 20, m_position.x, m_position.y, m_physicsRadius, m_tint );

	g_theRenderer->PushView();
	g_theRenderer->TranslateView( Vector3( m_position.x, m_position.y, 0.0f ) );
	g_theRenderer->DrawTexturedAABB2( m_bounds, *m_texture, Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), m_tint );
	g_theRenderer->PopView();
}