#include "Engine/Core/Time.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
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
	, m_currentState( STATE_MAIN_MENU )
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
	if ( GetGameState() == STATE_MAIN_MENU )
	{
		if ( g_theApp->GetXboxADownStatus() )
		{
			SetGameState( STATE_PLAYING );
		}
	}
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
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	if ( GetGameState() == STATE_MAIN_MENU )
	{
		// "BallzClone"
		g_theRenderer->DrawText2D( Vector2( 100.0f, RangeMap( sin( GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "B", 50.0f, Rgba::MAGENTA, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 150.0f, RangeMap( sin( GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "a", 50.0f, Rgba::ORANGE, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 200.0f, RangeMap( sin( GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "l", 50.0f, Rgba::BLUE, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 250.0f, RangeMap( sin( GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "l", 50.0f, Rgba::CYAN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 300.0f, RangeMap( sin( GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "z", 50.0f, Rgba::LIGHTOLIVEGREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 350.0f, RangeMap( sin( GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "Clone", 50.0f, Rgba::WHITE, fixedFont );

		// "Aim"
		g_theRenderer->DrawText2D( Vector2( 310.0f, 410.0f ), "Aim", 20.0f, Rgba::WHITE, fixedFont );

		// "Shoot"
		g_theRenderer->DrawText2D( Vector2( 310.0f, 350.0f ), "Shoot", 20.0f, Rgba::WHITE, fixedFont );

		// Left stick
		g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( 250.0f, 400.0f ), Vector2( 300.0f, 450.0f ) ), *Texture::CreateOrGetTexture( STICK_LEFT_TEXTURE_FILE.c_str() ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), Rgba::WHITE );

		// A button
		g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( 250.0f, 340.0f ), Vector2( 295.0f, 385.0f ) ), *Texture::CreateOrGetTexture( BUTTON_A_TEXTURE_FILE.c_str() ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), Rgba::WHITE );
	}
	else if ( GetGameState() == STATE_PLAYING )
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

		// Call Render() for each power up
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
					CheckForTileDestroy( m_tiles[ tileIndex ] );
				}
				if ( m_playerBall->m_position.x > tileWorldPosition.x + 40.0f )
				{
					// Right side of tile
					m_playerBall->m_velocity = Vector2( -m_playerBall->m_velocity.x, m_playerBall->m_velocity.y );
					CheckForTileDestroy( m_tiles[ tileIndex ] );
				}
				if ( m_playerBall->m_position.y < tileWorldPosition.y - 40.0f )
				{
					// Bottom side of tile
					m_playerBall->m_velocity = Vector2( m_playerBall->m_velocity.x, -m_playerBall->m_velocity.y );
					CheckForTileDestroy( m_tiles[ tileIndex ] );
				}
				if ( m_playerBall->m_position.y > tileWorldPosition.y + 40.0f )
				{
					// Top side of tile
					m_playerBall->m_velocity = Vector2( m_playerBall->m_velocity.x, -m_playerBall->m_velocity.y );
					CheckForTileDestroy( m_tiles[ tileIndex ] );
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::CheckForTileDestroy( Tile* tile )
{
	tile->m_numHitsRemaining -= 1;

	if ( tile->m_numHitsRemaining < 1 )
	{
		for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
		{
			if ( m_tiles[ tileIndex ] == tile )
			{
				delete m_tiles[ tileIndex ];
				m_tiles[ tileIndex ] = nullptr;
				return;
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
			if ( DoDiscsOverlap( m_playerBall->m_position, m_playerBall->m_radius, m_powerUps[ powerUpIndex ]->m_worldPosition, m_powerUps[ powerUpIndex ]->m_radius ) )
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
bool TheGame::SetGameState( GameState newState )
{
	bool didChange = false;

	if ( GetGameState() != newState )
	{
		m_currentState = newState;
		didChange = true;
	}
	else
	{
		ASSERT_OR_DIE( false, "Attempted to set new state to same state!" );
	}

	return didChange;
}