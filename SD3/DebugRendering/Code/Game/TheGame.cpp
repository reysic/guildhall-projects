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
#include "Engine/Renderer/Fonts/BitmapFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
bool g_waitForTildeKeyUp = false;
bool g_waitForUpArrowKeyUp = false;
bool g_waitForIKeyUp = false;
bool g_waitForOKeyUp = false;
bool g_waitForPKeyUp = false;
bool g_waitForLKeyUp = false;


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;
float g_deltaSeconds = 0.0f;
float g_timePassed = 0.0f;
float g_fps = 0.0f;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_age( 0.0f )
{
	// Testing of debug draw commands
	//g_theRenderer->DebugDrawPoint( Vector3( 2.0f, 2.0f, 2.0f ), Rgba::PINK );
	//g_theRenderer->DebugDrawLine( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 1.0f, 1.0f, 1.0f ), Rgba::WHITE );
	//g_theRenderer->DebugDrawArrow( Vector3( 5.0f, 5.0f, 5.0f ), Vector3( 6.0f, 6.0f, 6.0f ), Rgba::RED );
	//g_theRenderer->DebugDrawAABB3( Vector3( -3.0f, -3.0f, -3.0f ), Vector3( -2.0f, -2.0f, -2.0f ), Rgba::WHITE, Rgba::CYAN );
	//g_theRenderer->DebugDrawSphere( Vector3( -2.0f, 2.0f, 2.0f ), 1.0f, Rgba::GREEN );
	//g_theRenderer->DebugDrawSphere( Vector3( -6.0f, 2.0f, 2.0f ), 1.0f, Rgba::GREEN, 15.0f );
	//g_theRenderer->DebugDrawSphere( Vector3( -10.0f, 2.0f, 2.0f ), 1.0f, Rgba::GREEN, 10.0f );
	//g_theRenderer->DebugDrawSphere( Vector3( -14.0f, 2.0f, 2.0f ), 1.0f, Rgba::GREEN, 5.0f );


	std::string fvfVertexShaderFile = "../../../Engine/Code/Engine/Shaders/fvf.vert";
	std::string fvfFragmentShaderFile = "../../../Engine/Code/Engine/Shaders/fvf.frag";
	std::string vertexShaderFile = "../../../Engine/Code/Engine/Shaders/multi_light.vert";
	std::string fragmentShaderFile = "../../../Engine/Code/Engine/Shaders/multi_light.frag";

	m_fvfMaterial = new Material( fvfVertexShaderFile, fvfFragmentShaderFile );
	m_cubeMaterial = new Material( vertexShaderFile, fragmentShaderFile );
	m_sphereMaterial = new Material( vertexShaderFile, fragmentShaderFile );

	Vector4 materialColor( 1.0f, 1.0f, 1.0f, 1.0f );

	m_cubeMaterial->SetUniform( "gColor", &materialColor );
	m_sphereMaterial->SetUniform( "gColor", &materialColor );

	MakeAxisLines();
	MakeCubeMesh();
	MakeSphereMesh();

	SetTextures();

	m_cubeMaterial->SetUniform( "gEffectState", &g_effectState );
	m_sphereMaterial->SetUniform( "gEffectState", &g_effectState );
}


//-----------------------------------------------------------------------------------------------
void TheGame::MakeAxisLines()
{
	m_lineX = new Mesh();
	m_lineX->MakeLineMesh( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 2.0f, 0.0f, 0.0f ), Rgba::RED );

// 	m_lineX->BindAttributeToMaterial( m_fvfMaterial, "inPosition", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_position ) );
// 	m_lineX->BindAttributeToMaterial( m_fvfMaterial, "inUV0", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_texCoords ) );
// 	m_lineX->BindAttributeToMaterial( m_fvfMaterial, "inColor", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_color ) );

	g_theRenderer->BindBuffer( 0 );

	m_lineY = new Mesh();
	m_lineY->MakeLineMesh( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 0.0f, 2.0f, 0.0f ), Rgba::GREEN );

// 	m_lineY->BindAttributeToMaterial( m_fvfMaterial, "inPosition", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_position ) );
// 	m_lineY->BindAttributeToMaterial( m_fvfMaterial, "inUV0", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_texCoords ) );
// 	m_lineY->BindAttributeToMaterial( m_fvfMaterial, "inColor", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_color ) );

	g_theRenderer->BindBuffer( 0 );

	m_lineZ = new Mesh();
	m_lineZ->MakeLineMesh( Vector3( 0.0f, 0.0f, 0.0f ), Vector3( 0.0f, 0.0f, 2.0f ), Rgba::BLUE );

// 	m_lineZ->BindAttributeToMaterial( m_fvfMaterial, "inPosition", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_position ) );
// 	m_lineZ->BindAttributeToMaterial( m_fvfMaterial, "inUV0", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_texCoords ) );
// 	m_lineZ->BindAttributeToMaterial( m_fvfMaterial, "inColor", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_color ) );

	g_theRenderer->BindBuffer( 0 );
}


//-----------------------------------------------------------------------------------------------
void TheGame::MakeCubeMesh()
{
	m_cube = new Mesh();
	m_cube->MakeTangentCubeMesh();

	m_cube->BindAttributeToMaterial( m_cubeMaterial, "inPosition", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_position ) );
	m_cube->BindAttributeToMaterial( m_cubeMaterial, "inUV0", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_texCoords ) );
	m_cube->BindAttributeToMaterial( m_cubeMaterial, "inColor", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_color ) );
	m_cube->BindAttributeToMaterial( m_cubeMaterial, "inTangent", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_tangent ) );
	m_cube->BindAttributeToMaterial( m_cubeMaterial, "inBitangent", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_bitangent ) );

	g_theRenderer->BindBuffer( 0 );
}


//-----------------------------------------------------------------------------------------------
void TheGame::MakeSphereMesh()
{
	m_sphere = new Mesh();
	m_sphere->MakeTangentSphereMesh();

	m_sphere->BindAttributeToMaterial( m_sphereMaterial, "inPosition", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_position ) );
	m_sphere->BindAttributeToMaterial( m_sphereMaterial, "inUV0", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_texCoords ) );
	m_sphere->BindAttributeToMaterial( m_sphereMaterial, "inColor", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_color ) );
	m_sphere->BindAttributeToMaterial( m_sphereMaterial, "inTangent", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_tangent ) );
	m_sphere->BindAttributeToMaterial( m_sphereMaterial, "inBitangent", sizeof( Vertex_PCUTB ), ( GLvoid* ) offsetof( Vertex_PCUTB, m_bitangent ) );

	g_theRenderer->BindBuffer( 0 );
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetTextures()
{
	m_diffuseTex = Texture::CreateOrGetTexture( "Data/Images/stone_diffuse.png" )->m_openglTextureID;
	m_normalTex = Texture::CreateOrGetTexture( "Data/Images/stone_normal.png" )->m_openglTextureID;
	m_specularTex = Texture::CreateOrGetTexture( "Data/Images/stone_specular.png" )->m_openglTextureID;
	m_emissiveTex = Texture::CreateOrGetTexture( "Data/Images/horde.png" )->m_openglTextureID;
	m_dissolveTex = Texture::CreateOrGetTexture( "Data/Images/dissolve.jpg" )->m_openglTextureID;
	m_maskTex = Texture::CreateOrGetTexture( "Data/Images/mask.png" )->m_openglTextureID;
}

//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	m_age += deltaSeconds;

	HandleMouseAndKeyboardInput();
	UpdateCameraMovement( deltaSeconds );

	g_theCamera->FixAndClampAngles();

	g_theAudioSystem->Update( deltaSeconds );

	g_deltaSeconds = deltaSeconds;

	UpdateFPSCounter( deltaSeconds );
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
void TheGame::HandleMouseAndKeyboardInput()
{
	ClearCameraMoveStates();

	const float mouseSensitivityYaw = 0.022f;
	const float mouseSensitivityPitch = 0.022f;

	const IntVector2 cursorSnapBackPos( 800, 600 );
	g_theInputSystem->SetMouseCursorSnapBackPosition( cursorSnapBackPos );
	IntVector2 cursorPosition = g_theInputSystem->GetMouseCursorPosition();
	int cursorDeltaX = cursorPosition.x - cursorSnapBackPos.x;
	int cursorDeltaY = cursorPosition.y - cursorSnapBackPos.y;
	if ( g_theInputSystem->m_appHasFocus )
	{
		g_theInputSystem->SetMouseCursorPosition( cursorSnapBackPos );
		g_theInputSystem->HideMouseCursor();

		if ( !( g_theDeveloperConsole->m_isOpen ) )
		{
			g_theCamera->m_orientation.m_yawDegreesAboutZ += mouseSensitivityYaw * ( float ) cursorDeltaX;
			g_theCamera->m_orientation.m_pitchDegreesAboutX -= mouseSensitivityPitch * ( float ) cursorDeltaY;
		}

		if ( !( g_theDeveloperConsole->m_isOpen ) )
		{
			if ( g_theInputSystem->IsKeyDown( 'W' ) )
			{
				g_theCamera->m_movingForward = true;
				g_theCamera->m_movingBackward = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'S' ) )
			{
				g_theCamera->m_movingForward = false;
				g_theCamera->m_movingBackward = true;
			}

			if ( g_theInputSystem->IsKeyDown( 'A' ) )
			{
				g_theCamera->m_movingLeft = true;
				g_theCamera->m_movingRight = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'D' ) )
			{
				g_theCamera->m_movingLeft = false;
				g_theCamera->m_movingRight = true;
			}

			if ( g_theInputSystem->IsKeyDown( VK_SPACE ) )
			{
				g_theCamera->m_movingUp = true;
				g_theCamera->m_movingDown = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'X' ) )
			{
				g_theCamera->m_movingUp = false;
				g_theCamera->m_movingDown = true;
			}

			if ( g_theInputSystem->IsKeyDown( 'I' ) && !g_waitForIKeyUp )
			{
				g_theCamera->ResetPosition();
				g_waitForIKeyUp = true;
			}

			if ( !( g_theInputSystem->IsKeyDown( 'I' ) ) )
			{
				g_waitForIKeyUp = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'O' ) && !g_waitForOKeyUp )
			{
				g_drawOriginAxes = !g_drawOriginAxes;
				g_waitForOKeyUp = true;
			}

			if ( !( g_theInputSystem->IsKeyDown( 'O' ) ) )
			{
				g_waitForOKeyUp = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'P' ) && !g_waitForPKeyUp )
			{
				g_effectState = !g_effectState;
				g_waitForPKeyUp = true;
			}

			if ( !( g_theInputSystem->IsKeyDown( 'P' ) ) )
			{
				g_waitForPKeyUp = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'L' ) && !g_waitForLKeyUp )
			{
				if ( g_lightMode < 4 )
				{
					g_lightMode++;
				}
				else
				{
					g_lightMode = 0;
				}
				g_waitForLKeyUp = true;
			}

			if ( !( g_theInputSystem->IsKeyDown( 'L' ) ) )
			{
				g_waitForLKeyUp = false;
			}

			if ( g_theInputSystem->IsKeyDown( VK_NUMPAD0 ) )
			{
				g_theRenderer->m_fboToDrawWith = g_theRenderer->m_fboMaterial0;
			}

			if ( g_theInputSystem->IsKeyDown( VK_NUMPAD1 ) )
			{
				g_theRenderer->m_fboToDrawWith = g_theRenderer->m_fboMaterial1;
			}

			if ( g_theInputSystem->IsKeyDown( VK_NUMPAD2 ) )
			{
				g_theRenderer->m_fboToDrawWith = g_theRenderer->m_fboMaterial2;
			}

			if ( g_theInputSystem->IsKeyDown( VK_NUMPAD3 ) )
			{
				g_theRenderer->m_fboToDrawWith = g_theRenderer->m_fboMaterial3;
			}

			if ( g_theInputSystem->IsKeyDown( VK_NUMPAD4 ) )
			{
				g_theRenderer->m_fboToDrawWith = g_theRenderer->m_fboMaterial4;
			}
			
			if ( g_theInputSystem->IsKeyDown( VK_NUMPAD5 ) )
			{
				g_theRenderer->m_fboToDrawWith = g_theRenderer->m_fboMaterial5;
			}

			if ( g_theInputSystem->IsKeyDown( VK_NUMPAD6 ) )
			{
				g_theRenderer->m_fboToDrawWith = g_theRenderer->m_fboMaterial6;
			}
		}

		if ( g_theInputSystem->IsKeyDown( 0xC0 ) && !g_waitForTildeKeyUp)
		{
			g_theDeveloperConsole->m_isOpen = !g_theDeveloperConsole->m_isOpen;
			g_waitForTildeKeyUp = true;
		}

		if ( !( g_theInputSystem->IsKeyDown( 0xC0 ) ) )
		{
			g_waitForTildeKeyUp = false;
		}
	}
	else
	{
		g_theInputSystem->ShowMouseCursor();
	}
}

//-----------------------------------------------------------------------------------------------
void TheGame::ClearCameraMoveStates()
{
	g_theCamera->m_movingForward = false;
	g_theCamera->m_movingBackward = false;
	g_theCamera->m_movingLeft = false;
	g_theCamera->m_movingRight = false;
	g_theCamera->m_movingUp = false;
	g_theCamera->m_movingDown = false;
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	// Clear the screen (color and depths)
	g_theRenderer->ClearScreen( Rgba::GRAY ); // Affects the Back Buffer (Default FBO)

	// BEGIN FBOs //
	g_theRenderer->FramebufferBind( g_theRenderer->m_fbo );
	g_theRenderer->ClearScreen( Rgba::GRAY ); // This clears the m_fbo
	// END FBOs //

	SetUp3DView();

	DrawScene();
	DrawDebugStuff3D();

	SetUp2DView();
	DrawHUD();
	DrawDebugStuff2D();

	// BEGIN FBOs //
	g_theRenderer->FramebufferBind( nullptr );
	g_theRenderer->PostProcess( g_theRenderer->m_fbo );
	//g_theRenderer->FramebufferCopyToBack( g_theRenderer->m_fbo );
	// Render full-screen quad instead of copy to back
	// END FBOs //

	// FlipAndPresent();
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetUp3DView() const
{
	// View matrix
	g_theRenderer->m_view.MatrixMakeRotationEuler( -g_theCamera->m_orientation.m_yawDegreesAboutZ, g_theCamera->m_orientation.m_pitchDegreesAboutX, 0.f, Vector3( 0.f, 0.f, 0.f ) );
	g_theRenderer->m_view.TransposeRotation();
	g_theRenderer->m_view.NegateTranslation( g_theCamera->m_position );

	// Projection matrix
	float aspect = 16.f / 9.f;
	g_theRenderer->m_projection.ChangeToPerspectiveProjection( 70.0f, aspect, 0.1f, 100.0f );
	Matrix4x4 COB = Matrix4x4();
	COB.SetBasis( Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 0.f, 1.f ), Vector3( 0.f, 1.f, 0.f ) );
	Matrix4x4 newProj = COB * g_theRenderer->m_projection;
	g_theRenderer->m_projection = newProj;

// 	// Set up a base perspective projection
// 	float aspect = ( 16.0f / 9.0f ); // VIRTUAL_SCREEN_WIDTH / VIRTUAL_SCREEN_HEIGHT;
// 	float fovDegreesVertical = 70.0f;
// 	float zNear = 0.1f;
// 	float zFar = 1000.0f;
// 	g_theRenderer->SetPerspective( fovDegreesVertical, aspect, zNear, zFar );
// 
// 	// Set up our game's preferred world coordinate system
// 	g_theRenderer->RotateView( -90.0f, Vector3( 1.0f, 0.0f, 0.0f ) ); // Put +Z up
// 	//g_theRenderer->RotateView( 90.0f, Vector3( 0.0f, 0.0f, 1.0f ) ); // Put +X forward
// 
// 	// Orient the view per the camera's orientation
// 	g_theRenderer->RotateView( -g_theCamera->m_orientation.m_pitchDegreesAboutX, Vector3( 1.0f, 0.0f, 0.0f ) );
// 	//g_theRenderer->RotateView( -g_theCamera.m_orientation.m_pitchDegreesAboutY, Vector3( 0.0f, 1.0f, 0.0f ) );
// 	g_theRenderer->RotateView( g_theCamera->m_orientation.m_yawDegreesAboutZ, Vector3( 0.0f, 0.0f, 1.0f ) );
// 
// 	// Position the view per the camera's position
// 	g_theRenderer->TranslateView( Vector3( -g_theCamera->m_position.x, -g_theCamera->m_position.y, -g_theCamera->m_position.z ) );
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetUp2DView() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawScene() const
{
	g_theRenderer->DepthTestingEnabled( true );

	Vector4 ambientLight( 1.0f, 1.0f, 1.0f, 0.1f );
	Vector4 lightColor( 1.0f, 1.0f, 1.0f, 1.0f );
	Vector3 cameralightPosition( g_theCamera->m_position );
	Vector3 lightPosition = Vector3::ZERO;
	float specularPower = 100.0f;
	float specularIntensity = 2.0f;
	float minDistanceFromCamera = 5.0f;
	float maxDistanceFromCamera = 10.0f;
	Vector4 fogColor( 0.5f, 0.5f, 0.5f, 1.0f );

	int lightCount = 16;
	std::vector< Light > cubeLights;

	if ( g_lightMode == 0 ) // Global point light
	{
		Vector3 globalLightPosition = Vector3( 0.0f, 0.0f, 2.0f );
		lightColor = Vector4( 1.0f, 1.0f, 0.0f, 1.0f );
		Light globalPointLight = Light( lightColor, globalLightPosition, Vector3::ZERO, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f );
		g_theRenderer->DrawSphere( globalLightPosition, 0.2f, Rgba::YELLOW );
		cubeLights.push_back( globalPointLight );
		lightCount = 1;
	}
	else if ( g_lightMode == 1 ) // Local point lights
	{
		for ( int i = 0; i < lightCount; i++ )
		{
			lightPosition = Vector3( cos( m_age + ( 2.0f * pi / 16.0f * i ) ), sin( m_age + ( 2.0f * pi / 16.0f * i ) ) - 0.1f, 0.0f );
			if ( i == 0 )
			{
				lightColor = Vector4( 1.0f, 0.0f, 0.0f, 1.0f );
				g_theRenderer->DrawSphere( lightPosition, 0.2f, Rgba::RED );
			}
			else if ( i == 4 )
			{
				lightColor = Vector4( 0.0f, 1.0f, 0.0f, 1.0f );
				g_theRenderer->DrawSphere( lightPosition, 0.2f, Rgba::GREEN );
			}
			else if ( i == 8 )
			{
				lightColor = Vector4( 0.0f, 0.0f, 1.0f, 1.0f );
				g_theRenderer->DrawSphere( lightPosition, 0.2f, Rgba::BLUE );
			}
			else if ( i == 12 )
			{
				lightColor = Vector4( 1.0f, 1.0f, 0.0f, 1.0f );
				g_theRenderer->DrawSphere( lightPosition, 0.2f, Rgba::YELLOW );
			}
			else
			{
				lightColor = Vector4( 0.f, 0.f, 0.f, 1.f );
				//g_theRenderer->DrawSphere( lightPosition, 0.2f, Rgba::BLACK );
			}
			cubeLights.push_back( Light( lightColor, lightPosition, 1.0f ) );
		}
	}
	else if ( g_lightMode == 2 ) // Local spotlight
	{
		lightColor = Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
		Vector3 cameraDirection = g_theCamera->GetForwardXYZ().Normalize();
		Light globalPointLight = Light( lightColor, g_theCamera->m_position, cameraDirection,
			1.0f, 6.0f, 1.0f, 0.0f,
			0.0f,
			CosDegrees( 5.0f ), CosDegrees( 15.0f ), 1.0f, 0.0f );

		cubeLights.push_back( globalPointLight );
		lightCount = 1;
	}
	else if ( g_lightMode == 3 ) // Global spotlight
	{
		lightColor = Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
		Vector3 globalSpotLightPos = Vector3( 0.0f, 0.0f, 2.0f );
		Vector3 globalSpotLightPosDir = Vector3( 0.0f, 0.0f, -1.0f );
		Light globalPointLight = Light( lightColor, globalSpotLightPos, globalSpotLightPosDir,
			1.0f, 6.0f, 1.0f, 1.0f,
			0.0f,
			CosDegrees( 5.0f ), CosDegrees( 15.0f ), 1.0f, 0.0f );

		cubeLights.push_back( globalPointLight );
		g_theRenderer->DrawSphere( globalSpotLightPos, 0.2f, Rgba::WHITE );
		lightCount = 1;
	}
	else if ( g_lightMode == 4 ) // Global directional light
	{
		lightColor = Vector4( 1.0f, 1.0f, 0.0f, 1.0f );
		Vector3 globalSpotLightPos = Vector3( 5.0f, 0.0f, 6.0f );
		Vector3 globalSpotLightPosDir = Vector3( -1.0f, 0.0f, -1.0f );
		Light globalPointLight = Light( lightColor, Vector3::ZERO, globalSpotLightPosDir,
			1.0f, 6.0f, 1.0f, 1.0f,
			1.0f,
			1.0f, -1.0f, 1.0f, 1.0f );

		cubeLights.push_back( globalPointLight );

		lightCount = 1;
	}

	///////////////////////////////////////////////////////////// FVF /////////////////////////////////////////////////////////////////////
	Matrix4x4 fvfModelMatrix = Matrix4x4();
	fvfModelMatrix.ChangeToTranslationMatrix( Vector3( 0.0f, 0.0f, 0.0f ) );

	m_fvfMaterial->SetUniform( "gModel", &fvfModelMatrix );
	m_fvfMaterial->SetUniform( "gView", &g_theRenderer->m_view );
	m_fvfMaterial->SetUniform( "gProj", &g_theRenderer->m_projection );

	m_fvfMaterial->BindTexture( "gDiffuseTex", ( void * ) &Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" )[ 0 ], 0 );

	m_lineX->BindAttributeToMaterial( m_fvfMaterial, "inPosition", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_position ) );
	m_lineX->BindAttributeToMaterial( m_fvfMaterial, "inUV0", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_texCoords ) );
	m_lineX->BindAttributeToMaterial( m_fvfMaterial, "inColor", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_color ) );

	m_lineX->DrawWithMaterial( m_fvfMaterial, PRIMITIVE_LINES );

	m_lineY->BindAttributeToMaterial( m_fvfMaterial, "inPosition", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_position ) );
	m_lineY->BindAttributeToMaterial( m_fvfMaterial, "inUV0", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_texCoords ) );
	m_lineY->BindAttributeToMaterial( m_fvfMaterial, "inColor", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_color ) );

	m_lineY->DrawWithMaterial( m_fvfMaterial, PRIMITIVE_LINES );

	m_lineZ->BindAttributeToMaterial( m_fvfMaterial, "inPosition", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_position ) );
	m_lineZ->BindAttributeToMaterial( m_fvfMaterial, "inUV0", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_texCoords ) );
	m_lineZ->BindAttributeToMaterial( m_fvfMaterial, "inColor", sizeof( Vertex_PCT ), ( GLvoid* ) offsetof( Vertex_PCT, m_color ) );

	m_lineZ->DrawWithMaterial( m_fvfMaterial, PRIMITIVE_LINES );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////// CUBE ////////////////////////////////////////////////////////////////////
	Matrix4x4 cubeModelMatrix = Matrix4x4();
	cubeModelMatrix.ChangeToTranslationMatrix( Vector3( 0.0f, 0.0f, 0.0f ) );

	m_cubeMaterial->SetUniform( "gModel", &cubeModelMatrix );
	m_cubeMaterial->SetUniform( "gView", &g_theRenderer->m_view );
	m_cubeMaterial->SetUniform( "gProj", &g_theRenderer->m_projection );

	m_cubeMaterial->BindTexture( "gDiffuseTex", ( void * ) &m_diffuseTex, 0 );
	m_cubeMaterial->BindTexture( "gNormalTex", ( void * ) &m_normalTex, 1 );
	m_cubeMaterial->BindTexture( "gSpecularTex", ( void * ) &m_specularTex, 2 );
	m_cubeMaterial->BindTexture( "gEmissiveTex", ( void * ) &m_emissiveTex, 3 );
	m_cubeMaterial->BindTexture( "gDissolveTex", ( void * ) &m_dissolveTex, 4 );

	m_cubeMaterial->SetLights( cubeLights );
	m_cubeMaterial->SetUniform( "gLightCount", &lightCount );
	m_cubeMaterial->SetUniform( "gAmbientLight", &ambientLight );
	m_cubeMaterial->SetUniform( "gSpecularPower", &specularPower );
	m_cubeMaterial->SetUniform( "gSpecularIntensity", &specularIntensity );
	m_cubeMaterial->SetUniform( "gCameraPosition", &( g_theCamera->m_position ) );
	m_cubeMaterial->SetUniform( "gMinDistanceFromCamera", &minDistanceFromCamera );
	m_cubeMaterial->SetUniform( "gMaxDistanceFromCamera", &maxDistanceFromCamera );
	m_cubeMaterial->SetUniform( "gFogColor", &fogColor );

	float periodicAge = abs( cos( m_age ) );
	m_cubeMaterial->SetUniform( "gAge", &periodicAge );

	m_cubeMaterial->SetUniform( "gEffectState", &g_effectState );

	m_cube->DrawWithMaterial( m_cubeMaterial, PRIMITIVE_TRIANGLES );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////// SPHERE ///////////////////////////////////////////////////////////////////
	Matrix4x4 sphereModelMatrix = Matrix4x4();
	sphereModelMatrix.ChangeToTranslationMatrix( Vector3( 5.0f, 0.0f, 0.0f ) );

	m_sphereMaterial->SetUniform( "gModel", &sphereModelMatrix );
	m_sphereMaterial->SetUniform( "gView", &g_theRenderer->m_view );
	m_sphereMaterial->SetUniform( "gProj", &g_theRenderer->m_projection );

	m_sphereMaterial->BindTexture( "gDiffuseTex", ( void * ) &m_diffuseTex, 0 );
	m_sphereMaterial->BindTexture( "gNormalTex", ( void * ) &m_normalTex, 1 );
	m_sphereMaterial->BindTexture( "gSpecularTex", ( void * ) &m_specularTex, 2 );
	m_sphereMaterial->BindTexture( "gEmissiveTex", ( void * ) &m_emissiveTex, 3 );
	m_sphereMaterial->BindTexture( "gDissolveTex", ( void * ) &m_dissolveTex, 4 );

	m_sphereMaterial->SetUniform( "gAmbientLight", &ambientLight );
	m_sphereMaterial->SetUniform( "gSpecularPower", &specularPower );
	m_sphereMaterial->SetUniform( "gSpecularIntensity", &specularIntensity );
	m_sphereMaterial->SetUniform( "gCameraPosition", &g_theCamera->m_position );
	m_sphereMaterial->SetUniform( "gMinDistanceFromCamera", &minDistanceFromCamera );
	m_sphereMaterial->SetUniform( "gMaxDistanceFromCamera", &maxDistanceFromCamera );
	m_sphereMaterial->SetUniform( "gFogColor", &fogColor );

	m_sphereMaterial->SetUniform( "gAge", &periodicAge );

	m_sphereMaterial->SetUniform( "gEffectState", &g_effectState );

	m_sphere->DrawWithMaterial( m_sphereMaterial, PRIMITIVE_TRIANGLES );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	g_theRenderer->SetActiveTextureTo0();

	g_theRenderer->m_fboToDrawWith->SetUniform( "gTime", ( void * ) &m_age );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawDebugStuff3D() const
{
	if ( g_drawOriginAxes )
	{
		g_theRenderer->DrawDebugAxisLines();
	}
	//g_theRenderer->DrawGrid();
	g_theRenderer->DrawDebugRenderCommands( g_deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawHUD() const
{
	g_theRenderer->DrawCrosshairs( Vector2( 800.0f, 450.0f ), Rgba::WHITE );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawDebugStuff2D() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	static BitmapFontProportional* proportionalFont = BitmapFontProportional::CreateOrGetFont( "Data/Fonts/Arial.fnt" );

	std::string camPosString = Stringf( "camPos=(%.02f, %.02f, %.02f)", g_theCamera->m_position.x, g_theCamera->m_position.y, g_theCamera->m_position.z );
	std::string camOrientString = Stringf( "pitch=%.0f  roll=%.0f  yaw=%.0f", g_theCamera->m_orientation.m_pitchDegreesAboutX, g_theCamera->m_orientation.m_rollDegreesAboutY, g_theCamera->m_orientation.m_yawDegreesAboutZ );
	std::string camFwdString = Stringf( "fwd=(%.02f, %.02f, %.02f)", g_theCamera->GetForwardXYZ().x, g_theCamera->GetForwardXYZ().y, g_theCamera->GetForwardXYZ().z );
	std::string fpsString = Stringf( "FPS:%.0f", g_fps );
	std::string lightModeString;

	if ( g_lightMode == 0 )
	{
		lightModeString = Stringf( "Global point light" );
	}
	else if ( g_lightMode == 1 )
	{
		lightModeString = Stringf( "Local point lights" );
	}
	else if ( g_lightMode == 2 )
	{
		lightModeString = Stringf( "Local spotlight (on camera)" );
	}
	else if ( g_lightMode == 3 )
	{
		lightModeString = Stringf( "Global spotlight" );
	}
	else if ( g_lightMode == 4 )
	{
		lightModeString = Stringf( "Global directional light" );
	}

	g_theRenderer->DrawText2D( Vector2( 0.0f, 875.0f ), "SD3 A7 - Jeremy Hicks", 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 850.0f ), "I - resets camera to starting position", 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 825.0f ), "O - toggles drawing of origin axes", 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 800.0f ), "L - changes light type", 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 725.0f ), "Custom Effect: Pulsing Emissive", 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 700.0f ), "Light Type: " + lightModeString, 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 50.0f ), "Camera Movement: W, A, S, D, X, Spacebar, Mouse", 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 0.0f ), camPosString, 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 500.0f, 0.0f ), camOrientString, 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 1000.0f, 0.0f ), camFwdString, 15.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 1500.0f, 0.0f ), fpsString, 15.0f, Rgba::WHITE, fixedFont );

	g_theRenderer->DrawTextProportional2D( Vector3( 0.0f, 450.0f, 0.0f ), "Proportional font", 0.5f, Rgba::WHITE, proportionalFont );
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateCameraMovement( float deltaSeconds )
{
	float speedMultiplier;

	if ( g_theInputSystem->IsKeyDown( VK_SHIFT ) )
	{
		speedMultiplier = 1.5f;
	}
	else
	{
		speedMultiplier = 1.0f;
	}

	Vector3 acceleration = Vector3( 0.0f, 0.0f, 0.0f );

	if ( g_theCamera->m_movingForward )
	{
		g_theCamera->m_position += g_theCamera->GetForwardXY() * CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;

	}
	else if ( g_theCamera->m_movingBackward )
	{
		g_theCamera->m_position -= g_theCamera->GetForwardXY() * CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}

	if ( g_theCamera->m_movingLeft )
	{
		g_theCamera->m_position += g_theCamera->GetLeftXY() * CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}
	else if ( g_theCamera->m_movingRight )
	{
		g_theCamera->m_position -= g_theCamera->GetLeftXY() * CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}

	if ( g_theCamera->m_movingUp )
	{
		g_theCamera->m_position.z += CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}
	else if ( g_theCamera->m_movingDown )
	{
		g_theCamera->m_position.z -= CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::KillVelocityInDirection( const Vector3& normalVector )
{
	if ( normalVector.x != 0.0f )
	{
		g_theCamera->m_velocity.x = 0.0f;
	}

	if ( normalVector.y != 0.0f )
	{
		g_theCamera->m_velocity.y = 0.0f;
	}

	if ( normalVector.z != 0.0f )
	{
		g_theCamera->m_velocity.z = 0.0f;
	}
}