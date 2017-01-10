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
	, m_disableShooting( true )
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

	float testFloat = job->JobRead< float >();
	int testInt = job->JobRead< int >();
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
		}

		if ( g_theInputSystem->IsKeyPressed( 0xC0 ) ) // ~ key
		{
			g_theDeveloperConsole->m_isOpen = !g_theDeveloperConsole->m_isOpen;
		}

		if ( g_theInputSystem->IsKeyPressed( 'O' ) )
		{
			m_disableShooting = !m_disableShooting;
		}

		if ( g_theInputSystem->IsKeyPressed( 'P' ) )
		{
			g_debugMode = !g_debugMode;
		}
	}
	else // App does not have focus
	{
		g_cursorSetToSnapBackPos = false;
		g_theInputSystem->ShowMouseCursor();
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

//	SpriteGameRenderer::Instance()->RenderFrame();

	// Clear the screen (color and depths)
	g_theRenderer->ClearScreen( Rgba::BLACK ); // Affects the Back Buffer (Default FBO)

	g_theCamera->SetUp2DView();
	g_theMap->Render(); // draw the map
 	g_theCamera->SetUp2DView();
  	DrawHUD();

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

	if ( m_state == STATE_MAIN_MENU )
	{
		g_theRenderer->DrawText2D( Vector2( 615.0f, 450.0f ), "WELCOME!", 50.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 675.0f, 400.0f ), "[N]: Play", 25.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 655.0f, 350.0f ), "[ESC]: Quit", 25.0f, Rgba::YELLOW, fixedFont );
	}

	if ( m_state == STATE_PLAYING )
	{
		g_theRenderer->DrawText2D( Vector2( 1450.0, 750.0f ), Stringf( "%i", m_score ), 50.0f, Rgba::WHITE, fixedFont );
	}

	if ( m_state == STATE_GAME_OVER )
	{
		g_theRenderer->DrawText2D( Vector2( 775.0f, 550.0f ), Stringf( "%i", m_score ), 50.0f, Rgba::WHITE, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 600.0f, 450.0f ), "YOU LOSE", 50.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 655.0f, 400.0f ), "[R]: Restart", 25.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 660.0f, 350.0f ), "[ESC]: Quit", 25.0f, Rgba::YELLOW, fixedFont );
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
void TestJobFunction( Job* job )
{

}