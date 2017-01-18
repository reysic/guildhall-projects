#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Sprites/SpriteSheet.hpp"
#include "Engine/Renderer/Sprites/SpriteGameRenderer.hpp"
#include "Engine/Renderer/Sprites/SpriteDatabase.hpp"
#include "Engine/Renderer/Sprites/Sprite.hpp"
#include "Engine/Renderer/Fonts/BitmapFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Core/GameCommon.hpp"
#include "Game/Core/TheApp.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
static bool g_cursorSetToSnapBackPos = false;
static bool g_shutDownInitiated = false;
static bool g_renderedPath = false;
static float g_updateLoopStart = 0.0f;
static float g_updateLoopEnd = 0.0f;
static float g_renderLoopStart = 0.0f;
static float g_renderLoopEnd = 0.0f;
static float g_loopCounterAge = 0.0f;
static float g_loopCounterDuration = 1.0f;
static float g_angle = 0.0f;
static bool g_updateLoopTimes = false;


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;
float g_deltaSeconds = 0.0f;
float g_timePassed = 0.0f;
float g_fps = 0.0f;
float g_shutDownTime = 0.0f;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_age( 0.0f )
	, m_score( 0 )
	, m_state( STATE_MAIN_MENU )
	, m_disableShooting( false )
	, m_myConnection( nullptr )
	, m_mySession( nullptr )
	, m_waitToSpawnNewPlayerBullet( false )
	, m_timeBetweenPlayerBullets( 1.0f / NET_PLAYER_FIRING_RATE_BULLETS_PER_SECOND )
	, m_timeLastPlayerBulletFired( 0.0f )
	, m_nextNetID( 3 )
	, m_redScore( 0 )
	, m_greenScore( 0 )
	, m_blueScore( 0 )
{
	PROFILE_LOG_SCOPE( "TheGame()" );

	g_theMap = new Map( IntVector2( 36, 20 ) );
// 	SpriteGameRenderer::Instance();
// 	SpriteDatabase::Instance()->AddSpriteResource( "testsprite", "Data/Images/Police.png", 
// 		AABB2( Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ) ) );
// 	m_testSprite = new Sprite( "testsprite" );
// 	m_testSprite->Enable();
// 	m_testSprite->m_position = Vector2( 3.0f, 3.0f );

	//ShaderTestInitialize();

	// Creation of various emitters for different particle effects
// 	Vector2 minDegrees( 45.0f, 45.0f );
// 	Vector2 maxDegrees( 135.0f, 135.0f );
//	m_emitter = new Emitter( Vector2( 9.5f, 10.5f ), Vector2( 1.0f, 1.0f ), 0.5f, 50, EMITTER_TYPE_EXPLOSION );
// 	m_emitter = new Emitter( Vector2( 9.5f, 10.5f ), Vector2( 1.0f, 1.0f ), 0.5f, 500, EMITTER_TYPE_FIREWORK );
// 	m_emitter = new Emitter( Vector2( 9.5f, 10.5f ), Vector2( 1.0f, 1.0f ), 5.0f, 5, EMITTER_TYPE_SMOKE, minDegrees, maxDegrees );
// 	m_emitter = new Emitter( Vector2( 9.5f, 10.5f ), Vector2( 1.0f, 1.0f ), 5.0f, 10, EMITTER_TYPE_FOUNTAIN, minDegrees, maxDegrees );

	Job* job = g_theJobSystem->JobCreate( JOB_CATEGORY_GENERIC, TestJobFunction );

	job->JobWrite< float >( 3.3f );
	job->JobWrite< int >( 5 );

// 	float testFloat = job->JobRead< float >();
// 	int testInt = job->JobRead< int >();

	// Initialize net objects to nullptr
	for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
	{
		m_netObjects[ index ] = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
TheGame::~TheGame()
{
	delete g_theMap;
	g_theMap = nullptr;
}



//-----------------------------------------------------------------------------------------------
void TheGame::ShaderTestInitialize()
{
	m_shaderProgram = new ShaderProgram( "../../../Engine/Code/Engine/Shaders/basic.vert",
		"../../../Engine/Code/Engine/Shaders/basic.frag" );

	float positionData[] = {
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f, 0.8f, 0.0f
	};
	float colorData[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f
	};

	// Create and populate the buffer objects
	GLuint vboHandles[ 2 ];
	glGenBuffers( 2, vboHandles );
	GLuint positionBufferHandle = vboHandles[ 0 ];
	GLuint colorBufferHandle = vboHandles[ 1 ];

	// Populate the position buffer
	glBindBuffer( GL_ARRAY_BUFFER, positionBufferHandle );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( float ), positionData, GL_STATIC_DRAW );

	// Populate the color buffer
	glBindBuffer( GL_ARRAY_BUFFER, colorBufferHandle );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof( float ), colorData, GL_STATIC_DRAW );

	// Create and setup the vertex array object
	glGenVertexArrays( 1, &m_vaoHandle );
	glBindVertexArray( m_vaoHandle );

	// Enable the vertex attribute arrays
	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );

	// Map index 0 to the position buffer
	glBindBuffer( GL_ARRAY_BUFFER, positionBufferHandle );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	// Map index 1 to the color buffer
	glBindBuffer( GL_ARRAY_BUFFER, colorBufferHandle );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, NULL );
}


//-----------------------------------------------------------------------------------------------
void TheGame::ShaderTestRender() const
{
	glClear( GL_COLOR_BUFFER_BIT );
	glLoadIdentity();
	glOrtho( 0.0f, 1600.0f, 0.0f, 900.0f, 0.0f, 1.0f );

	// Shader Test
	glUseProgram( m_shaderProgram->m_programID );

	mat44_fl rotationMatrix = mat44_fl::identity;
	rotationMatrix.MakeRotationEuler( EulerAngles( 0.0f, g_angle, 0.0f ), Vector3( 0.0f, 0.0f, 0.0f ) );

	GLuint location = glGetUniformLocation( m_shaderProgram->m_programID, "RotationMatrix" );

	if ( location >= 0 )
	{
		glUniformMatrix4fv( location, 1, GL_FALSE, rotationMatrix.data );
	}

	glBindVertexArray( m_vaoHandle );
	glDrawArrays( GL_TRIANGLES, 0, 3 );

	glUseProgram( NULL );
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{	
	PushProfileSample( "gameupdate" );

	if ( g_updateLoopTimes == true )
	{
		g_updateLoopTimes = false;
		g_loopCounterAge = 0.0f;
	}

	if ( g_loopCounterAge > g_loopCounterDuration ) { g_updateLoopTimes = true; }

	if ( g_updateLoopTimes ) { g_updateLoopStart = ( float ) GetCurrentTimeSeconds() * 1000.0f; }

	m_age += deltaSeconds;
	g_loopCounterAge += deltaSeconds;
	g_angle += 0.05f;

	HandleMouseAndKeyboardInput();

	g_theAudioSystem->Update( deltaSeconds );

	g_deltaSeconds = deltaSeconds;

	UpdateFPSCounter( deltaSeconds );

	UpdateWorld( deltaSeconds );

	if ( g_updateLoopTimes ) { g_updateLoopEnd = ( float ) GetCurrentTimeSeconds() * 1000.0f; }

	// Update net objects
	for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
	{
		if ( m_netObjects[ index ] != nullptr )
		{
			if ( m_netObjects[ index ]->m_ownerConnectionIndex == m_myConnection->m_index )
			{
				m_netObjects[ index ]->Update( deltaSeconds );
			}
		}
	}

	CheckBulletsForCollisions();

	DestroyDeadBullets();

	PopProfileSample();
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
void TheGame::UpdateWorld( float deltaSeconds )
{
	if ( m_state == STATE_PLAYING )
	{
		if ( g_theMap->GetPlayer()->GetIsDead() )
		{
			m_state = STATE_GAME_OVER;
		}
		else
		{
			g_theMap->Update( deltaSeconds ); // update the map
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleMouseAndKeyboardInput()
{
	const float mouseSensitivityYaw = 0.022f;
	const float mouseSensitivityPitch = 0.022f;

	const IntVector2 cursorSnapBackPos( 800, 600 );
	g_theInputSystem->SetMouseCursorSnapBackPosition( cursorSnapBackPos );
	IntVector2 cursorPosition = g_theInputSystem->GetMouseCursorPosition();
	int cursorDeltaX = cursorPosition.x - cursorSnapBackPos.x;
	int cursorDeltaY = cursorPosition.y - cursorSnapBackPos.y;

	cursorDeltaX;
	cursorDeltaY;

	// Does app have focus?
	if ( g_theInputSystem->m_appHasFocus )
	{
		// App has focus
		if ( !g_cursorSetToSnapBackPos )
		{
			g_theInputSystem->SetMouseCursorPosition( cursorSnapBackPos );
			g_cursorSetToSnapBackPos = true;
		}
		// g_theInputSystem->HideMouseCursor();

		if ( !( g_theDeveloperConsole->m_isOpen ) )
		{
			HandleStateSpecificInput();

			if ( g_theInputSystem->IsKeyPressed( 'O' ) )
			{
				m_disableShooting = !m_disableShooting;
			}

			if ( g_theInputSystem->IsKeyPressed( 'P' ) )
			{
				g_debugMode = !g_debugMode;
			}

			HandleTwinStickInput();
		}

		if ( g_theInputSystem->IsKeyPressed( 0xC0 ) ) // ~ key
		{
			g_theDeveloperConsole->m_isOpen = !g_theDeveloperConsole->m_isOpen;
		}
	}
	else // App does not have focus
	{
		g_cursorSetToSnapBackPos = false;
		g_theInputSystem->ShowMouseCursor();
	}

}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleTwinStickInput()
{
	if ( m_myConnection == nullptr )
	{
		return;
	}

	// Check Xbox right stick position to determine if bullet should be spawned
	Vector2 xboxRightStickPosition = g_theInputSystem->GetXboxRightStickPosition( m_myConnection->m_index );

	if ( ( xboxRightStickPosition.Length() > 0 ) && !m_waitToSpawnNewPlayerBullet )
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
void TheGame::HandleStateSpecificInput()
{
	if ( m_state == STATE_MAIN_MENU && g_theInputSystem->IsKeyPressed( 'N' ) )
	{
		SetState( STATE_PLAYING );
	}
	else if ( m_state == STATE_GAME_OVER && g_theInputSystem->IsKeyPressed( 'R' ) )
	{
		g_theMap->Restart();
		SetState( STATE_MAIN_MENU );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	if ( g_updateLoopTimes ) { g_renderLoopStart = ( float ) GetCurrentTimeSeconds() * 1000.0f; }
	
	//ShaderTestRender();

	//SpriteGameRenderer::Instance()->RenderFrame();

	// Clear the screen (color and depths)
 	g_theRenderer->ClearScreen( Rgba::BLACK ); // Affects the Back Buffer (Default FBO)

// 	g_theCamera->SetUp2DView();
// 	g_theMap->Render(); // draw the map
	g_theCamera->SetUp2DView();
	DrawHUD();
	DrawNetDebugInfo();

	// Render net objects
	if ( m_myConnection != nullptr )
	{
		g_theCamera->SetUp2DView();
		for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
		{
			if ( m_netObjects[ index ] != nullptr )
			{
				m_netObjects[ index ]->Render();
			}
		}
	}

	if ( g_updateLoopTimes ) { g_renderLoopEnd = ( float ) GetCurrentTimeSeconds() * 1000.0f; }
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawHUD() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	if ( g_debugMode )
	{
		std::string fpsString = Stringf( "FPS:%.0f", g_fps );

		g_theRenderer->DrawText2D( Vector2( 0.0f, 880.0f ), "Update():" + Stringf( "%.02f", ( g_updateLoopEnd - g_updateLoopStart ) ) + "ms", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 300.0f, 880.0f ), "Render():" + Stringf( "%.02f", ( abs( g_renderLoopEnd - g_renderLoopStart ) ) ) + "ms", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 600.0f, 880.0f ), fpsString, 15.0f, Rgba::GREEN, fixedFont );
	}

	g_theRenderer->DrawText2D( Vector2( 0.0f, 430.0f ), "Red Score: " + std::to_string( m_redScore ), 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 410.0f ), "Green Score: " + std::to_string( m_greenScore), 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 390.0f ), "Blue Score: " + std::to_string( m_blueScore ), 15.0f, Rgba::WHITE, fixedFont );

// 	if ( m_state == STATE_MAIN_MENU )
// 	{
// 		g_theRenderer->DrawText2D( Vector2( 615.0f, 450.0f ), "WELCOME!", 50.0f, Rgba::YELLOW, fixedFont );
// 		g_theRenderer->DrawText2D( Vector2( 675.0f, 400.0f ), "[N]: Play", 25.0f, Rgba::YELLOW, fixedFont );
// 		g_theRenderer->DrawText2D( Vector2( 655.0f, 350.0f ), "[ESC]: Quit", 25.0f, Rgba::YELLOW, fixedFont );
// 	}
// 
// 	if ( m_state == STATE_PLAYING )
// 	{
// 		g_theRenderer->DrawText2D( Vector2( 1450.0, 750.0f ), Stringf( "%i", m_score ), 50.0f, Rgba::WHITE, fixedFont );
// 	}
// 
// 	if ( m_state == STATE_GAME_OVER )
// 	{
// 		g_theRenderer->DrawText2D( Vector2( 775.0f, 550.0f ), Stringf( "%i", m_score ), 50.0f, Rgba::WHITE, fixedFont );
// 		g_theRenderer->DrawText2D( Vector2( 600.0f, 450.0f ), "YOU LOSE", 50.0f, Rgba::YELLOW, fixedFont );
// 		g_theRenderer->DrawText2D( Vector2( 655.0f, 400.0f ), "[R]: Restart", 25.0f, Rgba::YELLOW, fixedFont );
// 		g_theRenderer->DrawText2D( Vector2( 660.0f, 350.0f ), "[ESC]: Quit", 25.0f, Rgba::YELLOW, fixedFont );
// 	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawNetDebugInfo() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	if ( m_mySession != nullptr )
	{
		if ( m_mySession->m_sessionState == SESSION_STATE_UINITIALIZED )
		{
			g_theRenderer->DrawText2D( Vector2( 0.0f, 770.0f ), "SESSION_STATE_UINITIALIZED", 15.0f,
				Rgba::WHITE, fixedFont );
		}
		else if ( m_mySession->m_sessionState == SESSION_STATE_UNCONNECTED )
		{
			g_theRenderer->DrawText2D( Vector2( 0.0f, 770.0f ), "SESSION_STATE_UNCONNECTED", 15.0f,
				Rgba::WHITE, fixedFont );
		}
		else if ( m_mySession->m_sessionState == SESSION_STATE_CONNECTED )
		{
			g_theRenderer->DrawText2D( Vector2( 0.0f, 770.0f ), "SESSION_STATE_CONNECTED", 15.0f,
				Rgba::WHITE, fixedFont );
		}
		g_theRenderer->DrawText2D( Vector2( 0.0f, 750.0f ), "last_sent[" +
			Stringf( "%.02f", m_mySession->m_timeDataLastSent ) + "s]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 730.0f ), "last_recv[" +
			Stringf( "%.02f", m_mySession->m_timeDataLastReceived ) + "s]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 710.0f ), "sim_lag[" +
			Stringf( "%.02f", m_mySession->m_simLagMilliseconds ) + "ms]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 690.0f ), "sim_loss[" +
			Stringf( "%.02f", m_mySession->m_simLossPercent ) + "%]", 15.0f, Rgba::GREEN, fixedFont );
	}

	if ( m_myConnection != nullptr )
	{
		g_theRenderer->DrawText2D( Vector2( 0.0f, 670.0f ), "conn_idx[" +
			std::to_string( m_myConnection->m_index ) + "]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 650.0f ), "guid[" +
			std::string( m_myConnection->m_guid ) + "]", 15.0f, Rgba::GREEN, fixedFont );
		std::string ip = g_theNetworkingSystem->SockAddrToString( ( const sockaddr* ) &m_myConnection->m_address );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 630.0f ), "ip[" +
			ip + "]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 610.0f ), "unsent_r[" + std::to_string( m_myConnection->m_unsentReliables.size() ) 
			+ "]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 590.0f ), "sent_r[" + std::to_string( m_myConnection->m_sentReliables.size() )
			+ "]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 570.0f ), "hr_ack[" + std::to_string( m_myConnection->m_highestReceivedAck )
			+ "]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 550.0f ), "recv_rel[" + std::to_string( m_myConnection->m_receivedReliableIDs.size() )
			+ "]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 530.0f ), "old_unc_rel[" + std::to_string( m_myConnection->m_oldestUnconfirmedReliableID )
			+ "]", 15.0f, Rgba::GREEN, fixedFont );
	}
	else
	{
		g_theRenderer->DrawText2D( Vector2( 0.0f, 670.0f ), "conn_idx[-]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 650.0f ), "guid[-]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 630.0f ), "ip[-]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 610.0f ), "unsent_r[-]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 590.0f ), "sent_r[-]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 570.0f ), "hr_ack[-]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 550.0f ), "recv_rel[-]", 15.0f, Rgba::GREEN, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 530.0f ), "old_unc_rel[-]", 15.0f, Rgba::GREEN, fixedFont );
	}
}


//-----------------------------------------------------------------------------------------------
bool TheGame::SetState( GameState newState )
{
	bool didChange = false;

	if ( GetState() != newState )
	{
		m_state = newState;
		didChange = true;
	}
	else
	{
		ASSERT_OR_DIE( false, "Attempted to set new state to same state!" );
	}

	return didChange;
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnPlayerBullet()
{
	PolarCoordinates xboxRightStickPositionPolar = g_theInputSystem->GetXboxRightStickPositionPolar( m_myConnection->m_index );

	for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
	{
		if ( m_netObjects[ index ] == nullptr )
		{
			uint16_t netID = GetNextNetID();
			m_netObjects[ index ] = new NetBullet( m_myConnection->m_index, netID,
				m_netObjects[ m_myConnection->m_index ]->m_position, xboxRightStickPositionPolar.theta );

			// Call OnSpawnBullet() event
			Message spawnBullet( GAMENETMSG_SPAWNBULLET );
			spawnBullet.Write< uint16_t >( netID );
			spawnBullet.Write< Vector2 >( m_netObjects[ m_myConnection->m_index ]->m_position );
			spawnBullet.Write< float >( xboxRightStickPositionPolar.theta );
			m_mySession->SendMessageToOthers( spawnBullet );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SpawnPlayerBulletIDoNotOwn( uint8_t ownerConnectionIndex, uint16_t netID, const Vector2& position, 
	float xboxRightStickPositionPolarThetaRadians )
{
	for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
	{
		if ( m_netObjects[ index ] == nullptr )
		{
			m_netObjects[ index ] = new NetBullet( ownerConnectionIndex, netID,
				position, xboxRightStickPositionPolarThetaRadians );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DestroyDeadBullets()
{
	for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
	{
		if ( m_netObjects[ index ] != nullptr )
		{
			if ( m_netObjects[ index ]->m_isDead )
			{
				// Store off net ID
				uint16_t netID = m_netObjects[ index ]->m_netID;

				// Delete the object locally
				delete m_netObjects[ index ];
				m_netObjects[ index ] = nullptr;

				// Call OnDestroyBullet() event
				Message destroyBullet( GAMENETMSG_DESTROYBULLET );
				destroyBullet.Write< uint16_t >( netID );
				m_mySession->SendMessageToOthers( destroyBullet );
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::CheckBulletsForCollisions()
{
	for ( int index = 3; index < MAX_NET_OBJECTS; ++index )
	{
		if ( m_netObjects[ index ] != nullptr )
		{
			switch ( m_myConnection->m_index )
			{
			case 0:
				// If this isn't my bullet...
				if ( m_netObjects[ index ]->m_ownerConnectionIndex != 0 )
				{
					NetObject* player = m_netObjects[ 0 ];
					NetObject* bullet = m_netObjects[ index ];
					if ( DoDiscsOverlap( player->m_position, player->m_cosmeticRadius, bullet->m_position,
						bullet->m_cosmeticRadius ) )
					{
						// Store off net ID
						uint16_t netID = m_netObjects[ index ]->m_netID;

						if ( m_netObjects[ index ]->m_ownerConnectionIndex == 1 )
						{
							m_greenScore++;
							Message incrementGreenScore( GAMENETMSG_INCREMENTGREENSCORE );
							m_mySession->SendMessageToOthers( incrementGreenScore );
						}
						else if ( m_netObjects[ index ]->m_ownerConnectionIndex == 2 )
						{
							m_blueScore++;
							Message incrementBlueScore( GAMENETMSG_INCREMENTBLUESCORE );
							m_mySession->SendMessageToOthers( incrementBlueScore );
						}

						// Delete the object locally
						delete m_netObjects[ index ];
						m_netObjects[ index ] = nullptr;

						// Call OnDestroyBullet() event
						Message destroyBullet( GAMENETMSG_DESTROYBULLET );
						destroyBullet.Write< uint16_t >( netID );
						m_mySession->SendMessageToOthers( destroyBullet );
						return;
					}
				}
			case 1:
				// If this isn't my bullet...
				if ( m_netObjects[ index ]->m_ownerConnectionIndex != 1 )
				{
					NetObject* player = m_netObjects[ 1 ];
					NetObject* bullet = m_netObjects[ index ];
					if ( DoDiscsOverlap( player->m_position, player->m_cosmeticRadius, bullet->m_position,
						bullet->m_cosmeticRadius ) )
					{
						// Store off net ID
						uint16_t netID = m_netObjects[ index ]->m_netID;

						if ( m_netObjects[ index ]->m_ownerConnectionIndex == 0 )
						{
							m_redScore++;
							Message incrementRedScore( GAMENETMSG_INCREMENTREDSCORE );
							m_mySession->SendMessageToOthers( incrementRedScore );
						}
						else if ( m_netObjects[ index ]->m_ownerConnectionIndex == 2 )
						{
							m_blueScore++;
							Message incrementBlueScore( GAMENETMSG_INCREMENTBLUESCORE );
							m_mySession->SendMessageToOthers( incrementBlueScore );
						}

						// Delete the object locally
						delete m_netObjects[ index ];
						m_netObjects[ index ] = nullptr;

						// Call OnDestroyBullet() event
						Message destroyBullet( GAMENETMSG_DESTROYBULLET );
						destroyBullet.Write< uint16_t >( netID );
						m_mySession->SendMessageToOthers( destroyBullet );
						return;
					}
				}
			case 2:
				// If this isn't my bullet...
				if ( m_netObjects[ index ]->m_ownerConnectionIndex != 2 )
				{
					NetObject* player = m_netObjects[ 2 ];
					NetObject* bullet = m_netObjects[ index ];
					if ( DoDiscsOverlap( player->m_position, player->m_cosmeticRadius, bullet->m_position,
						bullet->m_cosmeticRadius ) )
					{
						// Store off net ID
						uint16_t netID = m_netObjects[ index ]->m_netID;

						if ( m_netObjects[ index ]->m_ownerConnectionIndex == 0 )
						{
							m_redScore++;
							Message incrementRedScore( GAMENETMSG_INCREMENTREDSCORE );
							m_mySession->SendMessageToOthers( incrementRedScore );
						}
						else if ( m_netObjects[ index ]->m_ownerConnectionIndex == 1 )
						{
							m_greenScore++;
							Message incrementGreenScore( GAMENETMSG_INCREMENTGREENSCORE );
							m_mySession->SendMessageToOthers( incrementGreenScore );
						}

						// Delete the object locally
						delete m_netObjects[ index ];
						m_netObjects[ index ] = nullptr;

						// Call OnDestroyBullet() event
						Message destroyBullet( GAMENETMSG_DESTROYBULLET );
						destroyBullet.Write< uint16_t >( netID );
						m_mySession->SendMessageToOthers( destroyBullet );
						return;
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
uint16_t TheGame::GetNextNetID()
{
	uint16_t currentAvailableNetID = m_nextNetID;
	++m_nextNetID;
	
	while ( FindNetObjectByID( m_myConnection->m_index, currentAvailableNetID ) != nullptr )
	{
		currentAvailableNetID = m_nextNetID;
		++m_nextNetID;
	}

	return currentAvailableNetID;
}


//-----------------------------------------------------------------------------------------------
NetObject* TheGame::FindNetObjectByID( uint8_t connectionID, uint16_t netID )
{
	for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
	{
		if ( m_netObjects[ index ] != nullptr )
		{
			if ( m_netObjects[ index ]->m_ownerConnectionIndex == connectionID &&
				m_netObjects[ index ]->m_netID == netID )
			{
				return m_netObjects[ index ];
			}
		}
	}

	// Didn't find net object with specified connection ID and net ID
	return nullptr;
}


//-----------------------------------------------------------------------------------------------
void TheGame::DeleteNetObjectByID( uint8_t connectionID, uint16_t netID )
{
	for ( int index = 0; index < MAX_NET_OBJECTS; ++index )
	{
		if ( m_netObjects[ index ] != nullptr )
		{
			if ( m_netObjects[ index ]->m_ownerConnectionIndex == connectionID &&
				m_netObjects[ index ]->m_netID == netID )
			{
				delete m_netObjects[ index ];
				m_netObjects[ index ] = nullptr;
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TestJobFunction( Job* job )
{
	UNUSED( job );
}