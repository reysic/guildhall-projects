#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/PlayerBall.hpp"
#include "Game/PowerUp.hpp"
#include "Game/Tile.hpp"


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_turnNumber( 0 )
	, m_playerBall( nullptr )
{
	m_playerBall = new PlayerBall();

	// Set all tile pointers to nullptr
	for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
	{
		m_tiles[ tileIndex ] = nullptr;
	}

	// Set all power up pointers to nullptr
	for ( int powerUpIndex = 0; powerUpIndex < MAX_POWER_UP_COUNT; ++powerUpIndex )
	{
		m_powerUps[ powerUpIndex ] = nullptr;
	}

	SpawnTiles();
	SpawnPowerUps();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Input()
{
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	// Call Update() for player
	m_playerBall->Update( deltaSeconds );

	CheckTilesForCollisions();
	CheckPowerUpsForCollisions();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	//DrawGrid();

	// Call Render() for each tile
	for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
	{
		if ( m_tiles[ tileIndex ] != nullptr )
		{
			m_tiles[ tileIndex ]->Render();
		}
	}

	// Call Render(0 for each power up
	for ( int powerUpIndex = 0; powerUpIndex < MAX_POWER_UP_COUNT; ++powerUpIndex )
	{
		if ( m_powerUps[ powerUpIndex ] != nullptr )
		{
			m_powerUps[ powerUpIndex ]->Render();
		}
	}

	// Call Render() for the player
	m_playerBall->Render();
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawGrid() const
{
	Vector2 screenSize( 7.0f, 9.0f );
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), screenSize );

	Rgba gridColor = Rgba::WHITE;

	int gridSizeX = 7;
	int gridSizeY = 9;

	for ( int x = 0; x < gridSizeX; x++ )
	{
		g_theRenderer->DrawLine3D( Vector3( ( float ) x, 0.0f, 0.0f ), Vector3( ( float ) x, ( float ) gridSizeY, 0.0f ), gridColor, 2.0f );
	}

	for ( int y = 0; y < gridSizeY; y++ )
	{
		g_theRenderer->DrawLine3D( Vector3( 0.0f, ( float ) y, 0.0f ), Vector3( ( float ) gridSizeX, ( float ) y, 0.0f ), gridColor, 2.0f );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnTiles()
{
	// Tile Spawn X: 0 - 6
	// Tile Spawn Y: 1 - 8
	for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
	{
		if ( m_tiles[ tileIndex ] == nullptr )
		{
			m_tiles[ tileIndex ] = new Tile( 1, IntVector2( 4, 6 ) );
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnPowerUps()
{
	// Power Up Spawn X: 0 - 6
	// Power Up Spawn Y: 1 - 8
	for ( int powerUpIndex = 0; powerUpIndex < MAX_POWER_UP_COUNT; ++powerUpIndex )
	{
		if ( m_powerUps[ powerUpIndex ] == nullptr )
		{
			m_powerUps[ powerUpIndex ] = new PowerUp( IntVector2( 2, 8 ) );
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::CheckTilesForCollisions()
{
	for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
	{
		if ( m_tiles[ tileIndex ] != nullptr )
		{
			Vector2 tileWorldPosition = m_tiles[ tileIndex ]->m_worldPosition;
			if ( m_playerBall->m_position.x > tileWorldPosition.x - 50.0f && m_playerBall->m_position.x < tileWorldPosition.x + 50.0f
				&& m_playerBall->m_position.y > tileWorldPosition.y - 50.0f && m_playerBall->m_position.y < tileWorldPosition.y + 50.0f )
			{
				// Ball is inside tile, so push out
				if ( m_playerBall->m_position.x < tileWorldPosition.x - 45.0f )
				{
					// Left side of tile
					m_playerBall->m_velocity = Vector2( -m_playerBall->m_velocity.x, m_playerBall->m_velocity.y );
				}
				if ( m_playerBall->m_position.x > tileWorldPosition.x + 40.0f )
				{
					// Right side of tile
					m_playerBall->m_velocity = Vector2( -m_playerBall->m_velocity.x, m_playerBall->m_velocity.y );
				}
				if ( m_playerBall->m_position.y < tileWorldPosition.y - 40.0f )
				{
					// Bottom side of tile
					m_playerBall->m_velocity = Vector2( m_playerBall->m_velocity.x, -m_playerBall->m_velocity.y );
				}
				if ( m_playerBall->m_position.y > tileWorldPosition.y + 40.0f )
				{
					// Top side of tile
					m_playerBall->m_velocity = Vector2( m_playerBall->m_velocity.x, -m_playerBall->m_velocity.y );
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::CheckPowerUpsForCollisions()
{
	for ( int powerUpIndex = 0; powerUpIndex < MAX_POWER_UP_COUNT; ++powerUpIndex )
	{
		if ( m_powerUps[ powerUpIndex ] != nullptr )
		{
			if ( DoDiscsOverlap( m_playerBall->m_position, m_playerBall->m_physicsRadius, m_powerUps[ powerUpIndex ]->m_worldPosition, m_powerUps[ powerUpIndex ]->m_radius ) )
			{
				DestroyPowerUp( m_powerUps[ powerUpIndex ] );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DestroyPowerUp( PowerUp* powerUp )
{
	for ( int powerUpIndex = 0; powerUpIndex < MAX_POWER_UP_COUNT; ++powerUpIndex )
	{
		if ( m_powerUps[ powerUpIndex ] == powerUp )
		{
			delete m_powerUps[ powerUpIndex ];
			m_powerUps[ powerUpIndex ] = nullptr;
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool TheGame::IsPlayerDiskAlive() const
{
	return !( m_playerBall->m_isDead );
}


//-----------------------------------------------------------------------------------------------
Vector2 TheGame::GetPlayerDiskPosition() const
{
	return m_playerBall->m_position;
}

bool TheGame::IsQuitting()
{
	if ( m_playerBall->m_health < 1 )
	{
		return true;
	}
	return false;
}
