#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/Ship.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_ship( nullptr )
	, m_numAliveAsteroids( 0 )
	, m_numAsteroidsInWave( 5 )
{
	m_ship = new Ship();

	// Set all asteroid pointers to nullptr
	for ( int index = 0; index < MAX_ASTEROID_COUNT; ++index )
	{
		m_asteroids[ index ] = nullptr;
	}

	// Set all bullet pointers to nullptr
	for ( int index = 0; index < MAX_BULLET_COUNT; ++index )
	{
		m_bullets[ index ] = nullptr;
	}

	for (int i = 0; i < m_numAsteroidsInWave; i++)
	{
		SpawnAsteroid();
	}

	m_waitToSpawnNewAsteroid = false;
	m_waitToDestroyRandomAsteroid = false;
	m_waitToSpawnNewBullet = false;
	m_waitToSpawnNewBulletXbox = false;
	m_debugModeOn = false;
	m_waitToToggleDebugMode = false;
}


//-----------------------------------------------------------------------------------------------
void TheGame::Input()
{
	// Check for 'O', which spawns another asteroid
	if (g_theApp->GetKeyDownStatus( 'O' ) && !m_waitToSpawnNewAsteroid)
	{
		SpawnAsteroid();
		m_waitToSpawnNewAsteroid = true;
	}

	if (!( g_theApp->GetKeyDownStatus( 'O' ) ) && m_waitToSpawnNewAsteroid)
	{
		m_waitToSpawnNewAsteroid = false;
	}

	// Check for 'L', which destroys a random asteroid
	if (g_theApp->GetKeyDownStatus( 'L' ) && !m_waitToDestroyRandomAsteroid)
	{
		DestroyRandomAsteroid();
		m_waitToDestroyRandomAsteroid = true;
	}

	if (!( g_theApp->GetKeyDownStatus( 'L' ) ) && m_waitToDestroyRandomAsteroid)
	{
		m_waitToDestroyRandomAsteroid = false;
	}

	// Check for 'VK_SPACE', which spawns another bullet
	if (g_theApp->GetKeyDownStatus( VK_SPACE ) && !m_waitToSpawnNewBullet && !m_ship->m_isDead)
	{
		SpawnBullet();
		m_waitToSpawnNewBullet = true;
	}

	if (!( g_theApp->GetKeyDownStatus( VK_SPACE ) ) && m_waitToSpawnNewBullet)
	{
		m_waitToSpawnNewBullet = false;
	}

	// Check for Xbox 'A', which spawns another bullet
	if (g_theApp->GetXboxADownStatus() && !m_waitToSpawnNewBulletXbox && !m_ship->m_isDead)
	{
		SpawnBullet();
		m_waitToSpawnNewBulletXbox = true;
	}

	if (!( g_theApp->GetXboxADownStatus() ) && m_waitToSpawnNewBulletXbox)
	{
		m_waitToSpawnNewBulletXbox = false;
	}

	// Check for 'K', which toggles debug mode
	if (g_theApp->GetKeyDownStatus( 'K' ) && !m_waitToToggleDebugMode)
	{
		m_debugModeOn = !m_debugModeOn;
		m_waitToToggleDebugMode = true;
	}

	if (!( g_theApp->GetKeyDownStatus( 'K' ) ) && m_waitToToggleDebugMode)
	{
		m_waitToToggleDebugMode = false;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds)
{
	m_ship->Update( deltaSeconds );

	// Check if ship is dead
	if ( !( m_ship->m_isDead ) )
	{
		CheckAsteroidsForCollisions();
	}
	else if ( g_theApp->GetKeyDownStatus( 'P' )  || g_theApp->GetXboxStartDownStatus() )
	{
		m_ship->Reset();
	}

	// Call Update() for each asteroid
	for ( int index = 0; index < MAX_ASTEROID_COUNT; ++index )
	{
		if ( m_asteroids[ index ] != nullptr )
		{
			m_asteroids[ index ]->Update( deltaSeconds );
		}
	}

	// Call Update() for each bullet
	for ( int index = 0; index < MAX_BULLET_COUNT; ++index )
	{
		if ( m_bullets[ index ] != nullptr )
		{
			m_bullets[ index ]->Update( deltaSeconds );
		}
	}

	DestroyDeadBullets();

	if ( m_numAliveAsteroids == 0 )
	{
		m_numAsteroidsInWave += 5;

		for (int i = 0; i < m_numAsteroidsInWave; i++)
		{
			SpawnAsteroid();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{

	m_ship->Render();

	if ( m_ship->m_isDead )
		g_theGame->ShakeScreen();

	// Call Render() for each asteroid
	for ( int index = 0; index < MAX_ASTEROID_COUNT; ++index )
	{
		if ( m_asteroids[ index ] != nullptr )
		{
			m_asteroids[ index ]->Render();
		}
	}

	// Call Render() for each bullet
	for ( int index = 0; index < MAX_BULLET_COUNT; ++index )
	{
		if ( m_bullets[ index ] != nullptr )
		{
			m_bullets[ index ]->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnAsteroid()
{
	for ( int index = 0; index < MAX_ASTEROID_COUNT; ++index )
	{
		if ( m_asteroids[ index ] == nullptr )
		{
			m_asteroids[ index ] = new Asteroid();
			m_numAliveAsteroids++;
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnAsteroid( const Vector2& spawnPosition, int asteroidSize )
{
	for (int index = 0; index < MAX_ASTEROID_COUNT; ++index)
	{
		if (m_asteroids[ index ] == nullptr)
		{
			m_asteroids[ index ] = new Asteroid( spawnPosition, asteroidSize );
			m_numAliveAsteroids++;
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DestroyRandomAsteroid()
{
	for ( int index = 0; index < MAX_ASTEROID_COUNT; ++index )
	{
		if ( m_asteroids[ index ] != nullptr )
		{
			delete m_asteroids[ index ];
			m_asteroids[ index ] = nullptr;
			m_numAliveAsteroids--;
			return;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void TheGame::DestroySpecificAsteroid( Asteroid* asteroid )
{
	for ( int index = 0; index < MAX_ASTEROID_COUNT; ++index )
	{
		if ( m_asteroids[ index ] == asteroid )
		{
			if ( m_asteroids[ index ]->m_asteroidSize == 3 )
			{
				SpawnAsteroid( m_asteroids[ index ]->m_position, 2 );
				SpawnAsteroid( m_asteroids[ index ]->m_position, 2 );
			}
			else if ( m_asteroids[ index ]->m_asteroidSize == 2 )
			{
				SpawnAsteroid( m_asteroids[ index ]->m_position, 1 );
				SpawnAsteroid( m_asteroids[ index ]->m_position, 1 );
			}

			delete m_asteroids [index];
			m_asteroids[ index ] = nullptr;
			m_numAliveAsteroids--;
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::CheckAsteroidsForCollisions()
{
	for ( int asteroidIndex = 0; asteroidIndex < MAX_ASTEROID_COUNT; ++asteroidIndex )
	{
		if ( m_asteroids[ asteroidIndex ] != nullptr )
		{
			// First check for collisions with the ship
			if ( DoDiscsOverlap( m_ship->m_position, m_ship->m_physicsRadius, m_asteroids[ asteroidIndex ]->m_position, m_asteroids[ asteroidIndex ]->m_physicsRadius ) )
			{
				DestroySpecificAsteroid( m_asteroids[ asteroidIndex ] );
				m_ship->DestroyShip();
			}
			else
			{
				// Then check for collisions with bullets
				for ( int bulletIndex = 0; bulletIndex < MAX_BULLET_COUNT; ++bulletIndex )
				{
					if ( m_bullets[ bulletIndex ] != nullptr && m_asteroids[ asteroidIndex ] != nullptr)
					{
						if ( DoDiscsOverlap( m_bullets[ bulletIndex ]->m_position, m_bullets[ bulletIndex ]->m_physicsRadius, m_asteroids[ asteroidIndex ]->m_position, m_asteroids[ asteroidIndex ]->m_physicsRadius ) )
						{
							DestroySpecificAsteroid( m_asteroids[ asteroidIndex ] );
							m_bullets[ bulletIndex ]->m_isDead = true;
						}
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnBullet()
{
	for ( int index = 0; index < MAX_BULLET_COUNT; ++index )
	{
		if ( m_bullets[ index ] == nullptr )
		{
			m_bullets[ index ] = new Bullet( m_ship->m_position, m_ship->m_orientationDegrees, m_ship->m_cosmeticRadius );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DestroyDeadBullets()
{
	for ( int index = 0; index < MAX_BULLET_COUNT; ++index )
	{
		if  ( m_bullets[ index ] != nullptr )
		{
			if ( m_bullets[ index ]->m_isDead )
			{
				delete m_bullets[ index ];
				m_bullets[ index ] = nullptr;
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool TheGame::GetDebugModeStatus()
{
	return m_debugModeOn;
}


//-----------------------------------------------------------------------------------------------
void TheGame::ShakeScreen()
{

	const float MAX_SHAKE_DISPLACEMENT = 20.f;

	float cameraShakeMagnitude = 1.0f - m_ship->GetFractionDeathAnimComplete();
	
	cameraShakeMagnitude *= MAX_SHAKE_DISPLACEMENT;
	
	Vector3 screenShakeOffset( GetRandomFloatNegativeOneToOne() * cameraShakeMagnitude, GetRandomFloatNegativeOneToOne() * cameraShakeMagnitude, 0.0f );

	g_theRenderer->TranslateView( screenShakeOffset );
}