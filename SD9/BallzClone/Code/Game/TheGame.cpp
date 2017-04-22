#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Particles/Emitter.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/TheGame.hpp"
#include "Game/PlayerBall.hpp"
#include "Game/PowerUp.hpp"
#include "Game/Tile.hpp"
#include "Game/IndicatorBall.hpp"
#include "Game/AimLine.hpp"
#include "Game/Neuroevolution.hpp"


//-----------------------------------------------------------------------------------------------
static bool g_waitForTildeKeyUp = false;
static bool g_turnIsActive = false;
static bool g_waitForAButtonUp = false;
static float g_updateLoopStart = 0.0f;
static float g_updateLoopEnd = 0.0f;
static float g_renderLoopStart = 0.0f;
static float g_renderLoopEnd = 0.0f;
static float g_loopCounterAge = 0.0f;
static float g_loopCounterDuration = 1.0f;
static bool g_updateLoopTimes = false;
static float g_deltaSeconds = 0.0f;
static float g_timePassed = 0.0f;
static float g_fps = 0.0f;
const char* STICK_LEFT_TEXTURE_FILE = "Data/Images/StickLeft.png";
const char* BUTTON_A_TEXTURE_FILE = "Data/Images/ButtonA.png";
static int g_numBallsShotThisTurn = 0;
static Vector2 g_newStartingPosition = PLAYER_BALL_INITIAL_POSITION;
static bool g_ballPositionSetThisTurn = false;
static int g_ballsToSpawnAtEndOfTurn = 0;


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_turnNumber( 1 )
	, m_highestTurnNumber( 1 )
	, m_currentState( STATE_MAIN_MENU )
	, m_canDrawAimLine( false )
	, m_waitToShootPlayerBall( false )
	, m_canShootBall( false )
	, m_forwardDirection( Vector2::ZERO )
	, m_timeBetweenBallShotsSec( 1.0f / PLAYER_BALL_SHOOT_RATE_BALLS_PER_SEC )
	, m_timeLastBallShotSec( 0.0f )
	, m_aimLine( nullptr )
{
	InitializeArrays();

	for ( int ballCount = 0; ballCount < NUM_STARTING_BALLS; ballCount++ )
	{
		SpawnPlayerBall();
	}
	SpawnMore();

	// Neuroevolution start
	m_neuroevolution = new Neuroevolution();
	m_generation = m_neuroevolution->CreateNextGeneration();
	m_currentGeneration = 1;
}


//-----------------------------------------------------------------------------------------------
TheGame::~TheGame()
{
	DeinitializeArrays();

	delete m_aimLine;
	m_aimLine = nullptr;
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
		if ( ( xboxLeftStickPosition.Length() > 0 ) && xboxLeftStickPositionPolar.radius > 0.5f 
			&& xboxLeftStickPositionPolar.theta < 0.0f && xboxLeftStickPositionPolar.theta > -3.14f )
		{
			// Enable drawing of aim line
			m_canDrawAimLine = true;
			if ( m_aimLine == nullptr )
			{
				m_aimLine = new AimLine();
			}
		}
		else
		{
			// Disable drawing of aim line
			m_canDrawAimLine = false;
			if ( m_aimLine != nullptr )
			{
				delete m_aimLine;
				m_aimLine = nullptr;
			}
		}
	}
	else
	{
		// Disable drawing of aim line
		m_canDrawAimLine = false;
		if ( m_aimLine != nullptr )
		{
			delete m_aimLine;
			m_aimLine = nullptr;
		}
	}

	if ( ( xboxLeftStickPosition.Length() > 0 ) && xboxLeftStickPositionPolar.radius > 0.5f 
		&& xboxLeftStickPositionPolar.theta < 0.0f && xboxLeftStickPositionPolar.theta > -3.14f 
		&& g_theInputSystem->GetXboxADownStatus( 0 ) && !g_turnIsActive && !g_waitForAButtonUp )
	{
		m_canShootBall = true;
	}
	else if ( !g_theInputSystem->GetXboxADownStatus( 0 ) && !IsAPlayerBallNotMoving() || g_turnIsActive )
	{
		m_canShootBall = false;
	}

	if ( m_canShootBall && ( GetNumBalls() == GetNumMovingBalls() ) && !g_turnIsActive )
	{
		g_turnIsActive = true;
		g_waitForAButtonUp = true;
	}

	if ( g_turnIsActive && AreAllPlayerBallsNotMoving() )
	{
		g_turnIsActive = false;
	}

	if ( !g_theInputSystem->GetXboxADownStatus( 0 ) && !m_canShootBall )
	{
		g_waitForAButtonUp = false;
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

	if ( m_canShootBall && ( g_numBallsShotThisTurn < GetNumBalls() ) )
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

	// Call Update() for each indicator ball
	UpdateIndicatorBalls( deltaSeconds );

	// Call Update() for aim line
	UpdateAimLine();

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
			PlayerBall* thisBall = m_playerBalls[ playerBallIndex ];
			thisBall->Update( deltaSeconds );
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
			Tile* thisTile = m_tiles[ tileIndex ];
			thisTile->Update( deltaSeconds );
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
void TheGame::UpdateIndicatorBalls( float deltaSeconds )
{
	for ( int indicatorBallIndex = 0; indicatorBallIndex < MAX_INDICATOR_BALL_COUNT; ++indicatorBallIndex )
	{
		if ( m_indicatorBalls[ indicatorBallIndex ] != nullptr )
		{
			m_indicatorBalls[ indicatorBallIndex ]->Update( deltaSeconds );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateAimLine()
{
	if ( m_aimLine != nullptr )
	{
		m_aimLine->SetLineStationPosition( g_newStartingPosition );
		m_aimLine->SetForwardDirection( m_forwardDirection );
	}
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
	g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( 250.0f, 400.0f ), Vector2( 300.0f, 450.0f ) ), *Texture::CreateOrGetTexture( STICK_LEFT_TEXTURE_FILE ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), Rgba::WHITE );

	// A button
	g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( 250.0f, 340.0f ), Vector2( 295.0f, 385.0f ) ), *Texture::CreateOrGetTexture( BUTTON_A_TEXTURE_FILE ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), Rgba::WHITE );
}


//-----------------------------------------------------------------------------------------------
void TheGame::RenderPlayingState() const
{
	DrawGrid();

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	// "Turn Number"
	float turnNumberTextCellHeight = 40.0f;
	float turnNumberTextWidth = BitmapFont::GetTextWidth( std::to_string( m_turnNumber ), turnNumberTextCellHeight );
	Vector2 turnNumberTextDrawPosition = Vector2( 350.0f - ( turnNumberTextWidth / 2.0f ), 845.0f - ( turnNumberTextCellHeight / 2.0f ) );
	g_theRenderer->DrawText2D( turnNumberTextDrawPosition, std::to_string( m_turnNumber ), turnNumberTextCellHeight, Rgba::WHITE, fixedFont );

	// "Best"
	float bestTextCellHeight = 15.0f;
	g_theRenderer->DrawText2D( Vector2( 30.0f, 860.0f ), "Best", bestTextCellHeight, Rgba::WHITE, fixedFont );

	// "Highest Turn Number"
	float highestTurnNumberTextCellHeight = 15.0f;
	float highestTurnNumberTextWidth = BitmapFont::GetTextWidth( std::to_string( m_highestTurnNumber ), highestTurnNumberTextCellHeight );
	Vector2 highestTurnNumberTextDrawPosition = Vector2( 60.0f - ( highestTurnNumberTextWidth / 2.0f ), 835.0f - ( highestTurnNumberTextCellHeight / 2.0f ) );
	g_theRenderer->DrawText2D( highestTurnNumberTextDrawPosition, std::to_string( m_highestTurnNumber ), highestTurnNumberTextCellHeight, Rgba::WHITE, fixedFont );

	// "Ball Count"
	if ( AreAllPlayerBallsNotMoving() )
	{
		g_theRenderer->DrawText2D( Vector2( g_newStartingPosition.x, g_newStartingPosition.y + 20.0f ), "x" + std::to_string( GetNumBalls() ), 10.0f, Rgba::WHITE, fixedFont );
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


	// Call Render() for each indicator ball
	for ( int indicatorBallIndex = 0; indicatorBallIndex < MAX_INDICATOR_BALL_COUNT; ++indicatorBallIndex )
	{
		if ( m_indicatorBalls[ indicatorBallIndex ] != nullptr )
		{
			m_indicatorBalls[ indicatorBallIndex ]->Render();
		}
	}

	// Call Render() for aim line
	if ( m_canDrawAimLine )
	{
		if ( m_aimLine != nullptr )
		{
			m_aimLine->Render();
		}
	}
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
void TheGame::SpawnPlayerBall()
{
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] == nullptr )
		{
			m_playerBalls[ playerBallIndex ] = new PlayerBall( g_newStartingPosition );
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
			int randomTileLife;
			if ( m_turnNumber == 1 )
			{
				randomTileLife = 1;
			}
			else
			{
				randomTileLife = GetRandomIntInRange( m_turnNumber, m_turnNumber * 2 );
			}
			m_tiles[ tileIndex ] = new Tile( randomTileLife, IntVector2( xPosition, 7 ) );
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
void TheGame::SpawnTileDestoryEmitter( Vector2 emitterLocation )
{
	for ( int emitterIndex = 0; emitterIndex < MAX_EMITTER_COUNT; ++emitterIndex )
	{
		if ( m_emitters[ emitterIndex ] == nullptr )
		{
			m_emitters[ emitterIndex ] = new Emitter( emitterLocation, Vector2( 1.0f, 1.0f ), 0.5f, 10,
				EMITTER_TYPE_EXPLOSION, Vector2::ZERO, Vector2( 360.0f, 360.0f ), Rgba::RED );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnPowerUpIndicatorEmitter( Vector2 emitterLocation )
{
	for ( int emitterIndex = 0; emitterIndex < MAX_EMITTER_COUNT; ++emitterIndex )
	{
		if ( m_emitters[ emitterIndex ] == nullptr )
		{
			m_emitters[ emitterIndex ] = new Emitter( emitterLocation, Vector2( 1.0f, 1.0f ), 0.5f, 10,
				EMITTER_TYPE_EXPLOSION, Vector2::ZERO, Vector2( 360.0f, 360.0f ), Rgba::LIGHTOLIVEGREEN );
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
					Tile* thisTile = m_tiles[ tileIndex ];
					
					Vector2 ballWorldPosition = thisBall->m_position;
					float distanceX = thisBall->m_position.x - tileWorldPosition.x;
					float distanceY = thisBall->m_position.y - tileWorldPosition.y;
					float tileWidth = 100.0f;
					float tileHeight = 100.0f;
					float tileHalfWidth = tileWidth / 2.0f;
					float tileHalfHeight = tileHeight / 2.0f;
					float ballRadius = thisBall->m_radius;

					//Is Circle in Square (case 5)
					if ( distanceX <= tileHalfWidth &&
						distanceX >= -tileHalfWidth &&
						distanceY <= tileHalfHeight &&
						distanceY >= - tileHalfHeight )
					{
						
						//Bounce Top Right
						if ( distanceX >= 0 && distanceY >= 0 )
						{
							if ( distanceX > distanceY )
							{
								BounceRight( thisBall, thisTile );
							}
							else
							{
								BounceTop( thisBall, thisTile );
							}
						}

						//Bounce Top Left
						else if ( distanceX < 0 && distanceY >= 0 )
						{
							if ( -distanceX > distanceY )
							{
								BounceLeft( thisBall, thisTile );
							}
							else
							{
								BounceTop( thisBall, thisTile );
							}
						}

						//Bounce Bottom Right
						else if ( distanceX >= 0 && distanceY < 0 )
						{
							if ( distanceX > -distanceY )
							{
								BounceRight( thisBall, thisTile );;
							}
							else
							{
								BounceBottom( thisBall, thisTile );
							}
						}

						//Bounce Bottom Left
						else if ( distanceX < 0 && distanceY < 0 )
						{
							if ( -distanceX > -distanceY )
							{
								BounceLeft( thisBall, thisTile );
							}
							else
							{
								BounceBottom( thisBall, thisTile );
							}
						}

						HitTile( thisTile );
					}

					//Left or right side (case 4/6)
					else if ( distanceY < tileHalfHeight && distanceY > -tileHalfHeight )
					{
						//Right side
						if ( distanceX > tileHalfWidth )
						{
							float totalDistance = tileHalfWidth + ballRadius;
							if ( distanceX <= totalDistance )
							{
								BounceRight( thisBall, thisTile );
								HitTile( thisTile );
							}
						}
						//Left side
						else if ( distanceX < -tileHalfWidth )
						{
							float totalDistance = tileHalfWidth + ballRadius;
							if ( distanceX >= -totalDistance )
							{
								BounceLeft( thisBall, thisTile );
								HitTile( thisTile );
							}
						}
					}

					//Top or Bottom (case 2/8)
					else if ( distanceX < tileHalfWidth && distanceX > -tileHalfWidth )
					{
						//Top side (case 2)
						if ( distanceY > tileHalfHeight )
						{
							float totalDistance = tileHalfHeight + ballRadius;
							if ( distanceY <= totalDistance )
							{
								BounceTop( thisBall, thisTile );
								HitTile( thisTile );
							}
						}
						//Bottom side (case 8)
						else if ( distanceY < -tileHalfHeight )
						{
							float totalDistance = tileHalfHeight + ballRadius;
							if ( distanceY >= -totalDistance )
							{
								BounceBottom( thisBall, thisTile );
								HitTile( thisTile );
							}
						}
					}

					// #TODO: Replace with specific corner bounce functions
					//Top right corner (case 3)
					else if ( distanceX > tileHalfWidth && distanceY > tileHalfHeight )
					{
						Vector2 tileTopRight = tileWorldPosition + Vector2( tileHalfWidth, tileHalfHeight );
						Vector2 fromCornerToBall = ballWorldPosition - tileTopRight;
						float distanceFromCornerToBall = fromCornerToBall.Length();
						if ( distanceFromCornerToBall <= ballRadius )
						{
							if ( fromCornerToBall.x > fromCornerToBall.y )
							{
								BounceRight( thisBall, thisTile );
							}
							else
							{
								BounceTop( thisBall, thisTile );
							}
							HitTile( thisTile );
						}
					}

					//Top left corner (case 1)
					else if ( distanceX < -tileHalfWidth && distanceY > tileHalfHeight )
					{
						Vector2 tileTopLeft = tileWorldPosition + Vector2( -tileHalfWidth, tileHalfHeight );
						Vector2 fromCornerToBall = ballWorldPosition - tileTopLeft;
						float distanceFromCornerToBall = fromCornerToBall.Length();
						if ( distanceFromCornerToBall <= ballRadius )
						{
							if ( -fromCornerToBall.x > fromCornerToBall.y )
							{
								BounceLeft( thisBall, thisTile );
							}
							else
							{
								BounceTop( thisBall, thisTile );
							}
							HitTile( thisTile );
						}
					}

					//bottom right corner (case 9)
					else if ( distanceX > tileHalfWidth && distanceY < -tileHalfHeight )
					{
						Vector2 tileBottomRight = tileWorldPosition + Vector2( tileHalfWidth, -tileHalfHeight );
						Vector2 fromCornerToBall = ballWorldPosition - tileBottomRight;
						float distanceFromCornerToBall = fromCornerToBall.Length();
						if ( distanceFromCornerToBall <= ballRadius )
						{
							if ( fromCornerToBall.x > -fromCornerToBall.y )
							{
								BounceRight( thisBall, thisTile );
							}
							else
							{
								BounceBottom( thisBall, thisTile );
							}
							HitTile( thisTile );
						}
					}

					//Bottom left corner (case 7)
					else if ( distanceX < -tileHalfWidth && distanceY < -tileHalfHeight )
					{
						Vector2 tileBottomLeft = tileWorldPosition + Vector2( -tileHalfWidth, -tileHalfHeight );
						Vector2 fromCornerToBall = ballWorldPosition - tileBottomLeft;
						float distanceFromCornerToBall = fromCornerToBall.Length();
						if ( distanceFromCornerToBall <= ballRadius )
						{
							if ( -fromCornerToBall.x > -fromCornerToBall.y )
							{
								BounceLeft( thisBall, thisTile );
							}
							else
							{
								BounceBottom( thisBall, thisTile );
							}
							HitTile( thisTile );
						}
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::BounceRight( PlayerBall* ball, Tile* tile )
{
	if ( tile == nullptr )
	{
		return;
	}

	//Move circle out of square
	float tileHalfWidth = 100.0f / 2.0f;
	float ballRadius = ball->m_radius;
	float offsetX = tileHalfWidth + ballRadius;
	ball->m_position = Vector2( tile->m_worldPosition.x + offsetX, ball->m_position.y );

	//Set new velocity
	ball->m_velocity = Vector2( -ball->m_velocity.x, ball->m_velocity.y );
}


//-----------------------------------------------------------------------------------------------
void TheGame::BounceLeft( PlayerBall* ball, Tile* tile )
{
	if ( tile == nullptr )
	{
		return;
	}

	//Move circle out of square
	float tileHalfWidth = 100.0f / 2.0f;
	float ballRadius = ball->m_radius;
	float offsetX = tileHalfWidth + ballRadius;
	ball->m_position = Vector2( tile->m_worldPosition.x - offsetX, ball->m_position.y );

	//Set new velocity
	ball->m_velocity = Vector2( -ball->m_velocity.x, ball->m_velocity.y );
}


//-----------------------------------------------------------------------------------------------
void TheGame::BounceTop( PlayerBall* ball, Tile* tile )
{
	if ( tile == nullptr )
	{
		return;
	}

	//Move circle out of square
	float tileHalfHeight = 100.0f / 2.0f;
	float ballRadius = ball->m_radius;
	float offsetY = tileHalfHeight + ballRadius;
	ball->m_position = Vector2( ball->m_position.x, tile->m_worldPosition.y + offsetY );

	//Set new velocity
	ball->m_velocity = Vector2( ball->m_velocity.x, -ball->m_velocity.y );
}


//-----------------------------------------------------------------------------------------------
void TheGame::BounceBottom( PlayerBall* ball, Tile* tile )
{
	if ( tile == nullptr )
	{
		return;
	}

	//Move circle out of square
	float tileHalfHeight = 100.0f / 2.0f;
	float ballRadius = ball->m_radius;
	float offsetY = tileHalfHeight + ballRadius;
	ball->m_position = Vector2( ball->m_position.x, tile->m_worldPosition.y - offsetY );

	//Set new velocity
	ball->m_velocity = Vector2( ball->m_velocity.x, -ball->m_velocity.y );
}


//-----------------------------------------------------------------------------------------------
void TheGame::HitTile( Tile* tile )
{
	if ( tile == nullptr )
	{
		return;
	}

	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( "Data/Sounds/Toy_PopGun_Shot.wav" ), 2.0f );

	tile->m_numHitsRemaining -= 1;

	CheckForTileDestroy( tile );
}


//-----------------------------------------------------------------------------------------------
void TheGame::CheckForTileDestroy( Tile* tile )
{
	if ( tile->m_numHitsRemaining < 1 )
	{
		for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
		{
			if ( m_tiles[ tileIndex ] == tile )
			{
				Vector2 tileWorldPosition = m_tiles[ tileIndex ]->m_worldPosition;
				delete m_tiles[ tileIndex ];
				m_tiles[ tileIndex ] = nullptr;
				SpawnTileDestoryEmitter( tileWorldPosition );
				return;
			}
		}
	}
	else
	{
		tile->StartHitAnimation();
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
						if ( DoDiscsOverlap( thisBall->m_position, thisBall->m_radius, 
							m_powerUps[ powerUpIndex ]->m_worldPosition, m_powerUps[ powerUpIndex ]->m_radius ) )
						{
							DestroyPowerUp( m_powerUps[ powerUpIndex ] );
							g_ballsToSpawnAtEndOfTurn++;
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
			SpawnPowerUpIndicatorBall( m_powerUps[ powerUpIndex ]->m_worldPosition );
			delete m_powerUps[ powerUpIndex ];
			m_powerUps[ powerUpIndex ] = nullptr;
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnPowerUpIndicatorBall( const Vector2& spawnLocation )
{
	for ( int indicatorBallIndex = 0; indicatorBallIndex < MAX_INDICATOR_BALL_COUNT; ++indicatorBallIndex )
	{
		if ( m_indicatorBalls[ indicatorBallIndex ] == nullptr )
		{
			m_indicatorBalls[ indicatorBallIndex ] = new IndicatorBall();
			m_indicatorBalls[ indicatorBallIndex ]->m_position = spawnLocation;
			m_indicatorBalls[ indicatorBallIndex ]->m_velocity = Vector2( 0.0f, 200.0f );
			SpawnPowerUpIndicatorEmitter( spawnLocation );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::AdvanceTurn()
{
	CondenseBalls();
	SetBallsToNotShotThisTurn();
	DestroyIndicatorBalls();
	SpawnBallsAtEndOfTurn();
	m_turnNumber++;
	g_ballPositionSetThisTurn = false;
	g_numBallsShotThisTurn = 0;
	m_canShootBall = false;
	if ( m_turnNumber > m_highestTurnNumber )
	{
		m_highestTurnNumber = m_turnNumber;
	}
	AdvancePowerUps();
	AdvanceTiles();
	if ( m_turnNumber != 1 )
	{
		SpawnMore();
	}
	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( "Data/Sounds/162476__kastenfrosch__gotitem.mp3" ), 1.0f );
}


//-----------------------------------------------------------------------------------------------
void TheGame::CondenseBalls()
{
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		PlayerBall* thisBall = m_playerBalls[ playerBallIndex ];
		if ( thisBall != nullptr )
		{
			thisBall->m_position = g_newStartingPosition;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetBallsToNotShotThisTurn()
{
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		PlayerBall* thisBall = m_playerBalls[ playerBallIndex ];
		if ( thisBall != nullptr )
		{
			thisBall->m_hasBeenShotThisTurn = false;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnBallsAtEndOfTurn()
{
	if ( g_ballsToSpawnAtEndOfTurn > 0 )
	{
		for ( int spawnedBallCount = 0; spawnedBallCount < g_ballsToSpawnAtEndOfTurn; spawnedBallCount++ )
		{
			SpawnPlayerBall();
		}
		g_ballsToSpawnAtEndOfTurn = 0;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DestroyIndicatorBalls()
{
	// Delete and set all indicator ball pointers to nullptr
	for ( int indicatorBallIndex = 0; indicatorBallIndex < MAX_INDICATOR_BALL_COUNT; ++indicatorBallIndex )
	{
		if ( m_indicatorBalls[ indicatorBallIndex ] != nullptr )
		{
			delete m_indicatorBalls[ indicatorBallIndex ];
			m_indicatorBalls[ indicatorBallIndex ] = nullptr;
		}
	}
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

			if ( newGridPosition.y < 1 )
			{
				DestroyPowerUp( m_powerUps[ powerUpIndex ] );
				return;
			}

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
	m_currentGeneration++;

	DeinitializeArrays();
	InitializeArrays();

	g_newStartingPosition = PLAYER_BALL_INITIAL_POSITION;
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
		PlayerBall* thisBall = m_playerBalls[ playerBallIndex ];
		if ( thisBall != nullptr && !thisBall->m_hasBeenShotThisTurn )
		{
			thisBall->m_velocity = m_forwardDirection * PLAYER_BALL_SPEED;
			thisBall->m_hasBeenShotThisTurn = true;
			m_waitToShootPlayerBall = true;
			m_timeLastBallShotSec = ( float ) GetCurrentTimeSeconds();
			g_numBallsShotThisTurn++;
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
int TheGame::GetNumMovingBalls() const
{
	int numMovingBalls = 0;

	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] != nullptr )
		{
			if ( m_playerBalls[ playerBallIndex ]->m_velocity != Vector2::ZERO )
			{
				numMovingBalls++;
			}
		}
	}

	return numMovingBalls;
}


//-----------------------------------------------------------------------------------------------
Vector2 TheGame::SetNewPlayerBallStartingPosition( const Vector2& positionOfBallAsking )
{
	if ( !g_ballPositionSetThisTurn )
	{
		g_newStartingPosition = Vector2( positionOfBallAsking.x, 10.0f );
		g_ballPositionSetThisTurn = true;
	}

	return g_newStartingPosition;
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
	
	// Set all indicator ball pointers to nullptr
	for ( int indicatorBallIndex = 0; indicatorBallIndex < MAX_INDICATOR_BALL_COUNT; ++indicatorBallIndex )
	{
		m_indicatorBalls[ indicatorBallIndex ] = nullptr;
	}
}

//-----------------------------------------------------------------------------------------------
void TheGame::DeinitializeArrays()
{
	// Delete and set all player ball pointers to nullptr
	for ( int playerBallIndex = 0; playerBallIndex < MAX_PLAYER_BALL_COUNT; ++playerBallIndex )
	{
		if ( m_playerBalls[ playerBallIndex ] != nullptr )
		{
			delete m_playerBalls[ playerBallIndex ];
			m_playerBalls[ playerBallIndex ] = nullptr;
		}
	}

	// Delete and set all tile pointers to nullptr
	for ( int tileIndex = 0; tileIndex < MAX_TILE_COUNT; ++tileIndex )
	{
		if ( m_tiles[ tileIndex ] != nullptr )
		{
			delete m_tiles[ tileIndex ];
			m_tiles[ tileIndex ] = nullptr;
		}
	}

	// Delete and set all power up pointers to nullptrs
	for ( int powerUpIndex = 0; powerUpIndex < MAX_POWER_UP_COUNT; ++powerUpIndex )
	{
		if ( m_powerUps[ powerUpIndex ] != nullptr )
		{
			delete m_powerUps[ powerUpIndex ];
			m_powerUps[ powerUpIndex ] = nullptr;
		}
	}

	// Delete and set all emitter pointers to nullptr
	for ( int emitterIndex = 0; emitterIndex < MAX_EMITTER_COUNT; ++emitterIndex )
	{
		if ( m_emitters[ emitterIndex ] != nullptr )
		{
			delete m_emitters[ emitterIndex ];
			m_emitters[ emitterIndex ] = nullptr;
		}
	}

	// Delete and set all indicator ball pointers to nullptr
	for ( int indicatorBallIndex = 0; indicatorBallIndex < MAX_INDICATOR_BALL_COUNT; ++indicatorBallIndex )
	{
		if ( m_indicatorBalls[ indicatorBallIndex ] != nullptr )
		{
			delete m_indicatorBalls[ indicatorBallIndex ];
			m_indicatorBalls[ indicatorBallIndex ] = nullptr;
		}
	}
}