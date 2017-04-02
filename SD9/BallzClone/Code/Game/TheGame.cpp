#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Particles/Emitter.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/TheGame.hpp"
#include "Game/PlayerBall.hpp"
#include "Game/PowerUp.hpp"
#include "Game/Tile.hpp"


//-----------------------------------------------------------------------------------------------
bool g_waitForTildeKeyUp = false;
static float g_updateLoopStart = 0.0f;
static float g_updateLoopEnd = 0.0f;
static float g_renderLoopStart = 0.0f;
static float g_renderLoopEnd = 0.0f;
static float g_loopCounterAge = 0.0f;
static float g_loopCounterDuration = 1.0f;
static bool g_updateLoopTimes = false;
float g_deltaSeconds = 0.0f;
float g_timePassed = 0.0f;
float g_fps = 0.0f;


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_turnNumber( 1 )
	, m_currentState( STATE_MAIN_MENU )
	, m_canDrawAimLine( false )
	, m_waitToShootPlayerBall( false )
	, m_canShootBall( false )
	, m_forwardDirection( Vector2::ZERO )
	, m_timeBetweenBallShotsSec( 1.0f / PLAYER_BALL_SHOOT_RATE_BALLS_PER_SEC )
	, m_timeLastBallShotSec( 0.0f )
{
	InitializeArrays();

	SpawnPlayerBall();
	SpawnMore();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Input()
{
	HandleKeyboardInput();
	HandleControllerInput();
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleKeyboardInput()
{
	if ( g_theInputSystem->IsKeyDown( 0xC0 ) && !g_waitForTildeKeyUp )
	{
		g_theDeveloperConsole->m_isOpen = !g_theDeveloperConsole->m_isOpen;
		g_waitForTildeKeyUp = true;
	}

	if ( !( g_theInputSystem->IsKeyDown( 0xC0 ) ) )
	{
		g_waitForTildeKeyUp = false;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleControllerInput()
{
	if ( GetGameState() == STATE_MAIN_MENU )
	{
		if ( g_theInputSystem->GetXboxADownStatus( 0 ) )
		{
			SetGameState( STATE_PLAYING );
		}
	}

	Vector2 xboxLeftStickPosition = g_theInputSystem->GetXboxLeftStickPosition( 0 );
	PolarCoordinates xboxLeftStickPositionPolar = g_theInputSystem->GetXboxLeftStickPositionPolar( 0 );

	// Calculate forward direction for launching
	if ( !m_canShootBall )
	{
		float orientationRadians = xboxLeftStickPositionPolar.theta;
		m_forwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
		m_forwardDirection.Negate();
	}

	if ( AreAllPlayerBallsNotMoving() )
	{
		if ( ( xboxLeftStickPosition.Length() > 0 ) && xboxLeftStickPositionPolar.radius > 0.5f && xboxLeftStickPositionPolar.theta < 0.0f && xboxLeftStickPositionPolar.theta > -3.14f )
		{
			// Enable drawing of aim line
			m_canDrawAimLine = true;
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
	}

	if ( ( xboxLeftStickPosition.Length() > 0 ) && xboxLeftStickPositionPolar.radius > 0.5f && xboxLeftStickPositionPolar.theta < 0.0f && xboxLeftStickPositionPolar.theta > -3.14f && g_theInputSystem->GetXboxADownStatus( 0 ) )
	{
		m_canShootBall = true;
	}
	else if ( !g_theInputSystem->GetXboxADownStatus( 0 ) && !IsAPlayerBallNotMoving() )
	{
		m_canShootBall = false;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	if ( g_updateLoopTimes == true )
	{
		g_updateLoopTimes = false;
		g_loopCounterAge = 0.0f;
	}

	if ( g_loopCounterAge > g_loopCounterDuration ) { g_updateLoopTimes = true; }

	if ( g_updateLoopTimes ) { g_updateLoopStart = ( float ) GetCurrentTimeSeconds() * 1000.0f; }

	g_loopCounterAge += deltaSeconds;

	if ( m_canShootBall )
	{
		if ( IsAPlayerBallNotMoving() )
		{
			if ( !m_waitToShootPlayerBall )
			{
				ShootPlayerBall();
			}
		}
	}

	// Call Update() for each player ball
	UpdatePlayerBalls( deltaSeconds );

	// Call Update() for each tile
	UpdateTiles( deltaSeconds );

	// Call Update() for each emitter
	UpdateEmitters( deltaSeconds );

	CheckTilesForCollisions();
	CheckPowerUpsForCollisions();

	float currentTime = ( float ) GetCurrentTimeSeconds();
	float timeNextBallCanBeShotSec = m_timeLastBallShotSec + m_timeBetweenBallShotsSec;
	if ( m_waitToShootPlayerBall && ( currentTime > timeNextBallCanBeShotSec ) )
	{
		m_waitToShootPlayerBall = false;
	}

	UpdateFPSCounter( deltaSeconds );

	if ( g_updateLoopTimes ) { g_updateLoopEnd = ( float ) GetCurrentTimeSeconds() * 1000.0f; }
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdatePlayerBalls( float deltaSeconds )
{
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] != nullptr )
		{
			m_playerBalls[ playerBallIndex ]->Update( deltaSeconds );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateTiles( float deltaSeconds )
{
	for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
	{
		if ( m_tiles[ tileIndex ] != nullptr )
		{
			m_tiles[ tileIndex ]->Update( deltaSeconds );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateEmitters( float deltaSeconds )
{
	for ( int emitterIndex = 0; emitterIndex < MAX_EMITTER_COUNT; ++emitterIndex )
	{
		if ( m_emitters[ emitterIndex ] != nullptr )
		{
			m_emitters[ emitterIndex ]->Update( deltaSeconds );
		}
	}

	DeleteDeadEmitters();
}


//-----------------------------------------------------------------------------------------------
void TheGame::DeleteDeadEmitters()
{
	for ( int emitterIndex = 0; emitterIndex < MAX_EMITTER_COUNT; ++emitterIndex )
	{
		if ( m_emitters[ emitterIndex ] != nullptr )
		{
			if ( m_emitters[ emitterIndex ]->m_isDead )
			{
				delete m_emitters[ emitterIndex ];
				m_emitters[ emitterIndex ] = nullptr;
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateFPSCounter( float deltaSeconds )
{
	g_timePassed += deltaSeconds;

	static int frames = 0;
	static float startTime = 0;
	static bool first = true;

	if ( first )
	{
		frames = 0;
		startTime = g_timePassed;
		first = false;
		return;
	}

	frames++;

	if ( g_timePassed - startTime > 0.25 && frames > 10 )
	{
		g_fps = frames / ( g_timePassed - startTime );
		startTime = g_timePassed;
		frames = 0;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	if ( g_updateLoopTimes ) { g_renderLoopStart = ( float ) GetCurrentTimeSeconds() * 1000.0f; }

	g_theRenderer->ClearScreen( Rgba::BLACK );

	if ( GetGameState() == STATE_MAIN_MENU )
	{
		RenderMainMenuState();
	}
	else if ( GetGameState() == STATE_PLAYING )
	{
		RenderPlayingState();
	}

	if ( g_debugMode )
	{
		RenderDebugInfo();
	}

	if ( g_updateLoopTimes ) { g_renderLoopEnd = ( float ) GetCurrentTimeSeconds() * 1000.0f; }
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderMainMenuState() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	// "BallzClone"
	g_theRenderer->DrawText2D( Vector2( 100.0f, RangeMap( sin( ( float ) GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "B", 50.0f, Rgba::MAGENTA, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 150.0f, RangeMap( sin( ( float ) GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "a", 50.0f, Rgba::ORANGE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 200.0f, RangeMap( sin( ( float ) GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "l", 50.0f, Rgba::BLUE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 250.0f, RangeMap( sin( ( float ) GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "l", 50.0f, Rgba::CYAN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 300.0f, RangeMap( sin( ( float ) GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "z", 50.0f, Rgba::LIGHTOLIVEGREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 350.0f, RangeMap( sin( ( float ) GetCurrentTimeSeconds() ), -1.0f, 1.0f, 550.0f, 600.0f ) ), "Clone", 50.0f, Rgba::WHITE, fixedFont );

	// "Aim"
	g_theRenderer->DrawText2D( Vector2( 310.0f, 410.0f ), "Aim", 20.0f, Rgba::WHITE, fixedFont );

	// "Shoot"
	g_theRenderer->DrawText2D( Vector2( 310.0f, 350.0f ), "Shoot", 20.0f, Rgba::WHITE, fixedFont );

	// Left stick
	g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( 250.0f, 400.0f ), Vector2( 300.0f, 450.0f ) ), *Texture::CreateOrGetTexture( STICK_LEFT_TEXTURE_FILE.c_str() ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), Rgba::WHITE );

	// A button
	g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( 250.0f, 340.0f ), Vector2( 295.0f, 385.0f ) ), *Texture::CreateOrGetTexture( BUTTON_A_TEXTURE_FILE.c_str() ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), Rgba::WHITE );
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderPlayingState() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	DrawGrid();

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	// "Turn Number"
	g_theRenderer->DrawText2D( Vector2( 10.0f, 10.0f ), std::to_string( m_turnNumber ), 20.0f, Rgba::WHITE, fixedFont );

	// "Ball Count"
	if ( AreAllPlayerBallsNotMoving() )
	{
		g_theRenderer->DrawText2D( Vector2( PLAYER_BALL_INITIAL_POSITION.x, PLAYER_BALL_INITIAL_POSITION.y + 20.0f ), "x" + std::to_string( GetNumBalls() ), 10.0f, Rgba::WHITE, fixedFont );
	}

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

	// Call Render() for each player ball
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] != nullptr )
		{
			m_playerBalls[ playerBallIndex ]->Render();
		}
	}

	// Call Render() for each emitter
	for ( int emitterIndex = 0; emitterIndex < MAX_EMITTER_COUNT; ++emitterIndex )
	{
		if ( m_emitters[ emitterIndex ] != nullptr )
		{
			m_emitters[ emitterIndex ]->Render();
		}
	}

	DrawAimLine();
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderDebugInfo() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	std::string fpsString = Stringf( "FPS:%.0f", g_fps );

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	g_theRenderer->DrawText2D( Vector2( 0.0f, 880.0f ), "Update():" + Stringf( "%.02f", ( g_updateLoopEnd - g_updateLoopStart ) ) + "ms", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 300.0f, 880.0f ), "Render():" + Stringf( "%.02f", ( abs( g_renderLoopEnd - g_renderLoopStart ) ) ) + "ms", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 600.0f, 880.0f ), fpsString, 15.0f, Rgba::GREEN, fixedFont );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawGrid() const
{
	if ( !g_debugMode )
	{
		return;
	}

	Vector2 screenSize( 7.0f, 9.0f );
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), screenSize );

	Rgba gridColor( 255, 255, 255, 75 );

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
void TheGame::DrawAimLine() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	// Draw the aiming line
	if ( m_canDrawAimLine )
	{
		Vector2 lineStartPosition = PLAYER_BALL_INITIAL_POSITION;
		Vector2 lineEndPosition( PLAYER_BALL_INITIAL_POSITION.x + m_forwardDirection.x * 300.0f, PLAYER_BALL_INITIAL_POSITION.y + m_forwardDirection.y * 300.0f );
		Vector2 aimLineVector = lineEndPosition - lineStartPosition;
		aimLineVector.Normalize();

		PolarCoordinates xboxLeftStickPositionPolar = g_theInputSystem->GetXboxLeftStickPositionPolar( 0 );
		float distanceIncrement = RangeMap( xboxLeftStickPositionPolar.radius, 0.5f, 1.0f, 10.0f, 75.0f );
		float lineBallRadius = RangeMap( xboxLeftStickPositionPolar.radius, 0.5f, 1.0f, 2.0f, 10.0f );

		for ( float distanceAlongAimLine = distanceIncrement; distanceAlongAimLine <= ( distanceIncrement * 10.0f ); distanceAlongAimLine += distanceIncrement )
		{
			g_theRenderer->DrawFilledPolygon( 20, lineStartPosition.x + distanceAlongAimLine * aimLineVector.x, lineStartPosition.y + distanceAlongAimLine * aimLineVector.y, lineBallRadius, Rgba::WHITE );
		}
		//g_theRenderer->DrawLine3D( Vector3( m_position.x, m_position.y, 0.0f ), Vector3( lineEndPosition.x, lineEndPosition.y, 0.0f ), Rgba::WHITE, 2.5f );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnPlayerBall()
{
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] == nullptr )
		{
			m_playerBalls[ playerBallIndex ] = new PlayerBall();
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnMore()
{
	int tilesSpawned = 0;

	for ( int spawnPositionX = 0; spawnPositionX <= 6; ++spawnPositionX )
	{
		bool shouldSpawnPowerUp = GetRandomChance( POWER_UP_SPAWN_CHANCE );
		bool shouldSpawnTile = GetRandomChance( TILE_SPAWN_CHANCE );

		if ( shouldSpawnPowerUp )
		{
			SpawnPowerUp( spawnPositionX );
		}
		else if ( shouldSpawnTile && tilesSpawned < 7)
		{
			SpawnTile( spawnPositionX );
			tilesSpawned++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnTile( int xPosition )
{
	// Tile Spawn X: 0 - 6
	// Tile Spawn Y: 1 - 8
	for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
	{
		if ( m_tiles[ tileIndex ] == nullptr )
		{
			m_tiles[ tileIndex ] = new Tile( GetRandomIntInRange( 1.0f, m_turnNumber * 1.5f ), IntVector2( xPosition, 7 ) );
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnPowerUp( int xPosition )
{
	// Power Up Spawn X: 0 - 6
	// Power Up Spawn Y: 1 - 8
	for ( int powerUpIndex = 0; powerUpIndex < MAX_POWER_UP_COUNT; ++powerUpIndex )
	{
		if ( m_powerUps[ powerUpIndex ] == nullptr )
		{
			m_powerUps[ powerUpIndex ] = new PowerUp( IntVector2( xPosition, 7 ) );
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnEmitter( Vector2 emitterLocation )
{
	for ( int emitterIndex = 0; emitterIndex < MAX_EMITTER_COUNT; ++emitterIndex )
	{
		if ( m_emitters[ emitterIndex ] == nullptr )
		{
			m_emitters[ emitterIndex ] = new Emitter( emitterLocation, Vector2( 1.0f, 1.0f ), 0.5f, 10,
				EMITTER_TYPE_EXPLOSION );
			return;
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
			for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
			{
				if ( m_playerBalls[ playerBallIndex ] != nullptr )
				{
					PlayerBall* thisBall = m_playerBalls[ playerBallIndex ];
					
					float circleDistanceX = abs( thisBall->m_position.x - tileWorldPosition.x );
					float circleDistanceY = abs( thisBall->m_position.y - tileWorldPosition.y );

					if ( !( circleDistanceX > ( 50.0f + thisBall->m_radius ) ) &&
						!( circleDistanceY > ( 50.0f + thisBall->m_radius ) ) )
					{
						if ( circleDistanceX <= 50.0f || circleDistanceY <= 50.0f || ( ( ( circleDistanceX - 50.0f ) * ( circleDistanceX - 50.0f ) + ( circleDistanceY - 50.0f ) * ( circleDistanceY - 50.0f ) ) <= thisBall->m_radius ) )
						{
							g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( "Data/Sounds/Toy_PopGun_Shot.wav" ), 1.0f );

							// Ball intersects tile
							if ( thisBall->m_position.y >= tileWorldPosition.y + 50.0f )
							{
								// Top side of tile
								thisBall->m_velocity = Vector2( thisBall->m_velocity.x, -thisBall->m_velocity.y );
								CheckForTileDestroy( m_tiles[ tileIndex ] );
							}
							else if ( thisBall->m_position.y < tileWorldPosition.y - 50.0f )
							{
								// Bottom side of tile
								thisBall->m_velocity = Vector2( thisBall->m_velocity.x, -thisBall->m_velocity.y );
								CheckForTileDestroy( m_tiles[ tileIndex ] );
							}
							else if ( thisBall->m_position.x < tileWorldPosition.x )
							{
								// Left side of tile
								thisBall->m_velocity = Vector2( -thisBall->m_velocity.x, thisBall->m_velocity.y );
								CheckForTileDestroy( m_tiles[ tileIndex ] );
							}
							else if ( thisBall->m_position.x > tileWorldPosition.x )
							{
								// Right side of tile
								thisBall->m_velocity = Vector2( -thisBall->m_velocity.x, thisBall->m_velocity.y );
								CheckForTileDestroy( m_tiles[ tileIndex ] );
							}
						}
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::CheckForTileDestroy( Tile* tile )
{
	if ( tile == nullptr )
	{
		return;
	}

	tile->m_numHitsRemaining -= 1;

	if ( tile->m_numHitsRemaining < 1 )
	{
		for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
		{
			if ( m_tiles[ tileIndex ] == tile )
			{
				Vector2 tileWorldPosition = m_tiles[ tileIndex ]->m_worldPosition;
				delete m_tiles[ tileIndex ];
				m_tiles[ tileIndex ] = nullptr;
				SpawnEmitter( tileWorldPosition );
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
			for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
			{
				if ( m_playerBalls[ playerBallIndex ] != nullptr )
				{
					PlayerBall* thisBall = m_playerBalls[ playerBallIndex ];
					if ( m_powerUps[ powerUpIndex ] != nullptr )
					{
						if ( DoDiscsOverlap( thisBall->m_position, thisBall->m_radius, m_powerUps[ powerUpIndex ]->m_worldPosition, m_powerUps[ powerUpIndex ]->m_radius ) )
						{
							DestroyPowerUp( m_powerUps[ powerUpIndex ] );
							SpawnPlayerBall();
						}
					}
				}
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
void TheGame::AdvanceTurn()
{
	m_turnNumber++;
	AdvancePowerUps();
	AdvanceTiles();
	if ( m_turnNumber != 1 )
	{
		SpawnMore();
	}
	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( "Data/Sounds/162476__kastenfrosch__gotitem.mp3" ), 1.0f );
}


//-----------------------------------------------------------------------------------------------
void TheGame::AdvanceTiles()
{
	for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
	{
		if ( m_tiles[ tileIndex ] != nullptr )
		{
			IntVector2 currentGridPosition = m_tiles[ tileIndex ]->m_gridPosition;
			Vector2 currentWorldPosition = m_tiles[ tileIndex ]->m_worldPosition;

			IntVector2 newGridPosition( currentGridPosition.x, currentGridPosition.y - 1 );
			Vector2 newWorldPosition( newGridPosition.x * 100.0f + 50.0f, newGridPosition.y * 100.0f + 50.0f );

			// Go ahead and check for game end here to avoid having to loop through tiles again
			if ( newGridPosition.y < 1 )
			{
				EndGame();
				return;
			}

			// Move tile down to new position
			m_tiles[ tileIndex ]->m_gridPosition = newGridPosition;
			m_tiles[ tileIndex ]->m_worldPosition = newWorldPosition;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::AdvancePowerUps()
{
	for ( int powerUpIndex = 0; powerUpIndex < MAX_POWER_UP_COUNT; ++powerUpIndex )
	{
		if ( m_powerUps[ powerUpIndex ] != nullptr )
		{
			IntVector2 currentGridPosition = m_powerUps[ powerUpIndex ]->m_gridPosition;
			Vector2 currentWorldPosition = m_powerUps[ powerUpIndex ]->m_worldPosition;

			IntVector2 newGridPosition( currentGridPosition.x, currentGridPosition.y - 1 );
			Vector2 newWorldPosition( newGridPosition.x * 100.0f + 50.0f, newGridPosition.y * 100.0f + 50.0f );

			// Move power up down to new position
			m_powerUps[ powerUpIndex ]->m_gridPosition = newGridPosition;
			m_powerUps[ powerUpIndex ]->m_worldPosition = newWorldPosition;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::EndGame()
{
	SetGameState( STATE_MAIN_MENU );
}


//-----------------------------------------------------------------------------------------------
void TheGame::Reset()
{
	m_turnNumber = 1;

	InitializeArrays();

	SpawnPlayerBall();
	SpawnMore();
}


//-----------------------------------------------------------------------------------------------
bool TheGame::AreAllPlayerBallsNotMoving() const
{
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] != nullptr )
		{
			if ( m_playerBalls[ playerBallIndex ]->m_velocity != Vector2( 0.0f, 0.0f ) )
			{
				return false;
			}
		}
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool TheGame::IsAPlayerBallNotMoving()
{
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] != nullptr )
		{
			if ( m_playerBalls[ playerBallIndex ]->m_velocity == Vector2( 0.0f, 0.0f ) )
			{
				return true;
			}
		}
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
void TheGame::ShootPlayerBall()
{
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] != nullptr && m_playerBalls[ playerBallIndex ]->m_velocity == Vector2::ZERO )
		{
			m_playerBalls[ playerBallIndex ]->m_velocity = m_forwardDirection * PLAYER_BALL_SPEED;
			m_waitToShootPlayerBall = true;
			m_timeLastBallShotSec = ( float ) GetCurrentTimeSeconds();
			break;
		}
	}
}



//-----------------------------------------------------------------------------------------------
int TheGame::GetNumBalls() const
{
	int numBalls = 0;

	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] != nullptr )
		{
			numBalls++;
		}
	}

	return numBalls;
}


//-----------------------------------------------------------------------------------------------
bool TheGame::SetGameState( GameState newState )
{
	if ( GetGameState() == STATE_PLAYING && newState == STATE_MAIN_MENU )
	{
		Reset();
	}

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


//-----------------------------------------------------------------------------------------------
void TheGame::InitializeArrays()
{
	// Set all player ball pointers to nullptr
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		m_playerBalls[ playerBallIndex ] = nullptr;
	}

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

	// Set all emitter pointers to nullptr
	for ( int emitterIndex = 0; emitterIndex < MAX_EMITTER_COUNT; ++emitterIndex )
	{
		m_emitters[ emitterIndex ] = nullptr;
	}
}