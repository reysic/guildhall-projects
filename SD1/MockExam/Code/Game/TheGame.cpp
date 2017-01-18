#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerDisc.hpp"
#include "Game/PlayerBullet.hpp"
#include "Game/EnemyDisc.hpp"

const float SCREEN_FLASH_DURATION = 0.5f;

//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_player( nullptr )
	, m_waitToSpawnNewPlayerBullet( false )
	, m_timeBetweenPlayerBullets( 1.0f / PLAYER_DISC_FIRING_RATE_BULLETS_PER_SECOND )
	, m_timeLastPlayerBulletFired( 0.0f )
	, m_screenIsFlashing( false )
	, m_screenFlashStartTime( 0.0f )
{
	m_player = new PlayerDisc();

	// Set all bullet pointers to nullptr
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLET_COUNT; ++bulletIndex )
	{
		m_bullets[ bulletIndex ] = nullptr;
	}

	// Set all enemy pointers to nullptr
	for ( int enemyIndex = 0; enemyIndex < MAX_ENEMY_COUNT; ++enemyIndex )
	{
		m_enemies[ enemyIndex ] = nullptr;
	}

	SpawnEnemyDisc();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Input()
{
	// Check Xbox right stick position to determine if bullet should be spawned
	Vector2 xboxRightStickPosition = g_theApp->GetXboxRightStickPosition();

	if ( ( xboxRightStickPosition.Length() > 0 ) && !m_waitToSpawnNewPlayerBullet && !( m_player->m_health < 1 ) )
	{
		SpawnPlayerBullet();
		m_waitToSpawnNewPlayerBullet = true;
		m_timeLastPlayerBulletFired = ( float ) GetCurrentTimeSeconds();
	}


	if ( m_waitToSpawnNewPlayerBullet && ( ( float ) GetCurrentTimeSeconds() ) > ( m_timeLastPlayerBulletFired + m_timeBetweenPlayerBullets ) )
	{
		m_waitToSpawnNewPlayerBullet = false;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	// Call Update() for player
	m_player->Update( deltaSeconds );


	// Call Update() for each enemy
	for ( int enemyIndex = 0; enemyIndex < MAX_ENEMY_COUNT; ++enemyIndex )
	{
		if ( m_enemies[ enemyIndex ] != nullptr )
		{
			m_enemies[ enemyIndex ]->Update( deltaSeconds );
		}
	}

	// Call Update() for each bullet
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLET_COUNT; ++bulletIndex )
	{
		if ( m_bullets[ bulletIndex ] != nullptr )
		{
			m_bullets[ bulletIndex ]->Update( deltaSeconds );
		}
	}


	CheckEnemiesForCollisions();

	DestroyDeadBullets();

	if ( m_screenIsFlashing )
	{
		float secondsSinceStart = ( float ) GetCurrentTimeSeconds() - m_screenFlashStartTime;

		if ( secondsSinceStart > SCREEN_FLASH_DURATION )
		{
			m_screenIsFlashing = false;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{

	m_player->Render();


	// Call Render() for each enemy
	for ( int enemyIndex = 0; enemyIndex < MAX_ENEMY_COUNT; ++enemyIndex )
	{
		if ( m_enemies[ enemyIndex ] != nullptr )
		{
			m_enemies[ enemyIndex ]->Render();
		}
	}


	// Call Render() for each bullet
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLET_COUNT; ++bulletIndex )
	{
		if ( m_bullets[ bulletIndex ] != nullptr )
		{
			m_bullets[ bulletIndex ]->Render();
		}
	}

	if ( m_screenIsFlashing )
	{
		float secondsSinceStart = ( float ) GetCurrentTimeSeconds() - m_screenFlashStartTime;

		float fractionComplete = secondsSinceStart / SCREEN_FLASH_DURATION;
		fractionComplete = ClampFloatZeroToOne( fractionComplete );

		g_theRenderer->DrawQuad( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ), Rgba( 1.0f, 0.0f, 0.0f, 1 - fractionComplete ) );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnPlayerBullet()
{
	PolarCoordinates xboxRightStickPositionPolar = g_theApp->GetXboxRightStickPositionPolar();

	for ( int bulletIndex = 0; bulletIndex < MAX_BULLET_COUNT; ++bulletIndex )
	{
		if ( m_bullets[ bulletIndex ] == nullptr )
		{
			m_bullets[ bulletIndex ] = new PlayerBullet( m_player->m_position,  xboxRightStickPositionPolar.theta );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DestroyDeadBullets()
{
	for ( int bulletIndex = 0; bulletIndex < MAX_BULLET_COUNT; ++bulletIndex )
	{
		if ( m_bullets[ bulletIndex ] != nullptr )
		{
			if ( m_bullets[ bulletIndex ]->m_isDead )
			{
				delete m_bullets[ bulletIndex ];
				m_bullets[ bulletIndex ] = nullptr;
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnEnemyDisc()
{
	for ( int enemyIndex = 0; enemyIndex < MAX_ENEMY_COUNT; ++enemyIndex )
	{
		if ( m_enemies[ enemyIndex ] == nullptr )
		{
			m_enemies[ enemyIndex ] = new EnemyDisc();
			return;
		}
	}
}



//-----------------------------------------------------------------------------------------------
void TheGame::CheckEnemiesForCollisions()
{
	for ( int enemyIndex = 0; enemyIndex < MAX_ENEMY_COUNT; ++enemyIndex )
	{
		if ( m_enemies[ enemyIndex ] != nullptr )
		{
			// First check for collisions with the player disc
			if ( DoDiscsOverlap( m_player->m_position, m_player->m_physicsRadius, m_enemies[ enemyIndex ]->m_position, m_enemies[ enemyIndex ]->m_physicsRadius ) )
			{
				DestroyEnemy( m_enemies[ enemyIndex ] );
				m_player->m_health = ( m_player->m_health - 1 );
				SpawnEnemyDisc();
				SpawnEnemyDisc();
				if ( !m_screenIsFlashing )
				{
					m_screenFlashStartTime = ( float ) GetCurrentTimeSeconds();
					m_screenIsFlashing = true;
				}
			}
			else
			{
				// Then check for collisions with bullets
				for ( int bulletIndex = 0; bulletIndex < MAX_BULLET_COUNT; ++bulletIndex )
				{
					if ( m_bullets[ bulletIndex ] != nullptr && m_enemies[ enemyIndex ] != nullptr )
					{
						if ( DoDiscsOverlap( m_bullets[ bulletIndex ]->m_position, m_bullets[ bulletIndex ]->m_physicsRadius, m_enemies[ enemyIndex ]->m_position, m_enemies[ enemyIndex ]->m_physicsRadius ) )
						{
							DestroyEnemy( m_enemies[ enemyIndex ] );
							SpawnEnemyDisc();
							SpawnEnemyDisc();
						}
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DestroyEnemy( EnemyDisc* enemy )
{
	for ( int enemyIndex = 0; enemyIndex < MAX_ENEMY_COUNT; ++enemyIndex )
	{
		if ( m_enemies[ enemyIndex ] == enemy )
		{
			delete m_enemies[ enemyIndex ];
			m_enemies[ enemyIndex ] = nullptr;
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool TheGame::IsPlayerDiskAlive() const
{
	return !( m_player->m_isDead );
}


//-----------------------------------------------------------------------------------------------
Vector2 TheGame::GetPlayerDiskPosition() const
{
	return m_player->m_position;
}

bool TheGame::IsQuitting()
{
	if ( m_player->m_health < 1 )
	{
		return true;
	}
	return false;
}
