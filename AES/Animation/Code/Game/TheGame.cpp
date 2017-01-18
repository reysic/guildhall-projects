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
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Tools/fbx.hpp"
#include "Engine/Renderer/GPUProgram.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
int g_renderMode = 0;
float g_updateLoopStart = 0.0f;
float g_updateLoopEnd = 0.0f;
float g_renderLoopStart = 0.0f;
float g_renderLoopEnd = 0.0f;
float g_reductionTimeStart = 0.0f;
float g_reductionTimeEnd = 0.0f;
bool g_drawModel = true;
bool g_drawSkeleton = true;


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( fbx_list )
{
	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Provide filename following fbx_list!", Rgba::RED );
	}
	else if ( args.m_argList.size() == 1 )
	{
		std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
		FbxListScene( filename );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Too many arguments!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( fbx_load )
{
	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Provide filename following fbx_load!", Rgba::RED );
	}
	else if ( args.m_argList.size() == 1 )
	{
		g_theGame->ClearBuilderPointers( g_theGame->m_modelMeshBuilder, g_theGame->m_modelRenderer, g_theGame->m_modelMesh );

		std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
		g_theGame->m_modelScale = 1.0f;
		std::vector< MeshBuilder* > importMeshes;
		std::vector< Skeleton* > importSkeletons;
		std::vector< Motion* > importMotions;
		SceneImport* fbxScene = FbxLoadSceneFromFile( filename, g_engineBasis, true, mat44_fl::identity );
		importMeshes = fbxScene->meshes;
		importSkeletons = fbxScene->skeletons;
		importMotions = fbxScene->motions;
		g_theGame->m_modelMeshBuilder = new MeshBuilder( importMeshes );
		if ( importSkeletons.size() == 1 )
		{
			g_theGame->m_skeleton = importSkeletons[ 0 ];
			g_theGame->PopulateJointMeshRenderer( importSkeletons[ 0 ] );
		}
		if ( importMotions.size() > 0 )
		{
			g_theGame->m_motion = importMotions[ 0 ];
		}
	}
	else if ( args.m_argList.size() == 2 )
	{
		g_theGame->ClearBuilderPointers( g_theGame->m_modelMeshBuilder, g_theGame->m_modelRenderer, g_theGame->m_modelMesh );

		std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
		float scale = stof( args.m_argList[ 1 ] );
		g_theGame->m_modelScale = scale;
		mat44_fl transform = mat44_fl::identity;
		MatrixMakeScale( &transform, g_theGame->m_modelScale );
		std::vector< MeshBuilder* > importMeshes;
		std::vector< Skeleton* > importSkeletons;
		std::vector< Motion* > importMotions;
		SceneImport* fbxScene = FbxLoadSceneFromFile( filename, g_engineBasis, true, transform );
		importMeshes = fbxScene->meshes;
		importSkeletons = fbxScene->skeletons;
		importMotions = fbxScene->motions;
		g_theGame->m_modelMeshBuilder = new MeshBuilder( importMeshes );
		if ( importSkeletons.size() == 1 )
		{
			g_theGame->m_skeleton = importSkeletons[ 0 ];
			g_theGame->PopulateJointMeshRenderer( importSkeletons[ 0 ] );
		}
		if ( importMotions.size() > 0 )
		{
			g_theGame->m_motion = importMotions[ 0 ];
		}
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Too many arguments!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( mesh_save )
{
	if ( g_theGame->m_modelMeshBuilder == nullptr )
	{
		g_theDeveloperConsole->ConsolePrint( "No mesh loaded to save!", Rgba::RED );
		return;
	}

	if ( args.m_argList.size() == 0 ) 
	{
		g_theDeveloperConsole->ConsolePrint( "Provide filename following mesh_save!", Rgba::RED );
	}
	else if ( args.m_argList.size() == 1 )
	{
		std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
		g_theGame->m_modelMeshBuilder->WriteToFile( filename );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Too many arguments!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( mesh_load )
{
	if ( g_theGame->m_modelMeshBuilder == nullptr )
	{
		g_theGame->m_modelMeshBuilder = new MeshBuilder();
	}
	
	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Provide filename following mesh_load!", Rgba::RED );
	}
	else if ( args.m_argList.size() == 1 )
	{
		g_theGame->ClearBuilderPointers( g_theGame->m_modelMeshBuilder, g_theGame->m_modelRenderer, g_theGame->m_modelMesh );
		g_theGame->m_modelMeshBuilder = new MeshBuilder();
		std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
		g_theGame->m_modelMeshBuilder->ReadFromFile( filename );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Too many arguments!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( mesh_reduce )
{
	UNUSED( args );

	if ( g_theGame->m_modelMeshBuilder == nullptr )
	{
		g_theDeveloperConsole->ConsolePrint( "No mesh loaded to reduce!", Rgba::RED );
	}
	else
	{
		g_reductionTimeStart = ( float ) GetCurrentTimeSeconds();
		g_theGame->m_modelMeshBuilder->MeshReduce();
		g_reductionTimeEnd = ( float ) GetCurrentTimeSeconds();
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( skel_save )
{
	if ( g_theGame->m_skeleton == nullptr )
	{
		g_theDeveloperConsole->ConsolePrint( "No skeleton loaded to save!", Rgba::RED );
	}

	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Provide filename following skel_save!", Rgba::RED );
	}
	else if ( args.m_argList.size() == 1 )
	{
		std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
		g_theGame->m_skeleton->WriteToFile( filename );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Too many arguments!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( skel_load )
{
	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Provide filename following skel_load!", Rgba::RED );
	}
	else if ( args.m_argList.size() == 1 )
	{
		if ( g_theGame->m_skeleton )
		{
			delete g_theGame->m_skeleton;
			g_theGame->m_skeleton = nullptr;
		}

		g_theGame->m_skeleton = new Skeleton();
		std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
		g_theGame->m_skeleton->ReadFromFile( filename );
		g_theGame->PopulateJointMeshRenderer( g_theGame->m_skeleton );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Too many arguments!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( anim_save )
{
	if ( g_theGame->m_motion == nullptr )
	{
		g_theDeveloperConsole->ConsolePrint( "No motion loaded to save!", Rgba::RED );
	}

	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Provide filename following anim_save!", Rgba::RED );
	}
	else if ( args.m_argList.size() == 1 )
	{
		std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
		g_theGame->m_motion->WriteToFile( filename );
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Too many arguments!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
CONSOLE_COMMAND( anim_load )
{
	if ( args.m_argList.size() == 0 )
	{
		g_theDeveloperConsole->ConsolePrint( "Provide filename following anim_load!", Rgba::RED );
	}
	else if ( args.m_argList.size() == 1 )
	{
		if ( g_theGame->m_skeleton == nullptr )
		{
			g_theDeveloperConsole->ConsolePrint( "No skeleton loaded to apply animation to!", Rgba::RED );
		}
		else
		{
			if ( g_theGame->m_motion )
			{
				delete g_theGame->m_motion;
				g_theGame->m_motion = nullptr;
			}

			g_theGame->m_motion = new Motion();
			std::string filename = "Data/FBXs/" + args.m_argList[ 0 ];
			g_theGame->m_motion->ReadFromFile( filename );
		}
	}
	else
	{
		g_theDeveloperConsole->ConsolePrint( "Too many arguments!", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;
float g_deltaSeconds = 0.0f;
float g_timePassed = 0.0f;
float g_fps = 0.0f;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_emitter( nullptr )
	, m_age( 0.0f )
	, m_lightNum( 4 )
	, m_maxLights( 4 )
	, m_modelScale( 1.0f )
{
	m_modelMeshBuilder = nullptr;
	m_modelRenderer = nullptr;
	m_modelMesh = nullptr;
	m_skeleton = nullptr;
	m_motion = nullptr;

	MeshBuilder meshBuilder = MeshBuilder( true );
	meshBuilder.AddCustom( Vector3( 0.f, 0.f, 0.f ), Vector3( 1.f, 0.f, 0.f ), Vector3( 0.f, 1.f, 0.f ), -20.f, 20.f, 200, -20.f, 20.f, 200 );

	m_cubeMesh = Mesh::GetMeshShape( MeshShape_CUBE );
	m_sphereMesh = Mesh::GetMeshShape( MeshShape_SPHERE );
	m_planeMesh = new Mesh( &meshBuilder, VertexType_PCUTB );
	m_programDot3 = new GPUProgram( "../../../Engine/Code/Engine/Shaders/skin_debug.vert", "../../../Engine/Code/Engine/Shaders/multi_light_render_modes.frag" );

	m_materialOrange = new Material( m_programDot3 );
	m_materialWhite = new Material( m_programDot3 );

	m_cubeMeshRenderer.push_back( new MeshRenderer( m_planeMesh, m_materialOrange, Transform( Vector3( 0.f, 0.f, 0.0f ), mat44_fl::identity, Vector3::ONE ) ) );

	for ( int lightCount = 0; lightCount < m_maxLights; ++lightCount )
	{
		m_lightMeshRenderer.push_back( new MeshRenderer( m_sphereMesh, m_materialWhite, Transform( Vector3::ZERO, mat44_fl::identity, Vector3( 0.5f, 0.5f, 0.5f ) ) ) );
	}

	m_materialOrange->SetUniform( "gDiffuseTex", "Data/Images/PlainOrangeTexture.png" );
	m_materialOrange->SetUniform( "gNormalTex", "Data/Images/PlainBlueTexture.png" );
	m_materialOrange->SetUniform( "gSpecularTex", "Data/Images/PlainBlackTexture.png" );
	m_materialOrange->SetUniform( "gEmissiveTex", "Data/Images/PlainBlackTexture.png" );

	m_materialOrange->SetUniform( "gSpecularPower", 2.f );
	m_materialOrange->SetUniform( "gSpecularIntensity", 0.4f );
	m_materialOrange->SetUniform( "gAmbientLight", Rgba( 0, 255, 0, 10 ) );

	m_materialOrange->SetUniform( "gFogColor", Vector3( 0.2f, 0.2f, 0.2f ) );
	m_materialOrange->SetUniform( "gFogNear", 50.f );
	m_materialOrange->SetUniform( "gFogFar", 55.f );
	m_materialOrange->SetUniform( "gFogNearFactor", 0.f );
	m_materialOrange->SetUniform( "gFogFarFactor", 1.f );

	m_materialWhite->SetUniform( "gDiffuseTex", "Data/Images/PlainWhiteTexture.png" );
	m_materialWhite->SetUniform( "gNormalTex", "Data/Images/PlainBlueTexture.png" );
	m_materialWhite->SetUniform( "gSpecularTex", "Data/Images/PlainBlackTexture.png" );
	m_materialWhite->SetUniform( "gEmissiveTex", "Data/Images/PlainWhiteTexture.png" );
	//m_materialWhite->SetUniform( "gEmissiveTex", "Data/Images/PlainBlackTexture.png" );

	m_materialWhite->SetUniform( "gSpecularPower", 2.f );
	m_materialWhite->SetUniform( "gSpecularIntensity", 0.4f );
	m_materialWhite->SetUniform( "gAmbientLight", Rgba( 0, 255, 0, 10 ) );

	m_materialWhite->SetUniform( "gFogColor", Vector3( 0.2f, 0.2f, 0.2f ) );
	m_materialWhite->SetUniform( "gFogNear", 50.0f ); // 50.0f
	m_materialWhite->SetUniform( "gFogFar", 55.0f ); // 55.0f
	m_materialWhite->SetUniform( "gFogNearFactor", 0.f );
	m_materialWhite->SetUniform( "gFogFarFactor", 1.f );

	m_lights.push_back( Light( Rgba::WHITE.FloatRepresentation(), Vector3::ZERO, 2.f ) );
	m_lights.push_back( Light( Rgba::WHITE.FloatRepresentation(), Vector3::ZERO, Vector3( 0.f, 0.f, -0.2f ),
		1.f, 6.f, 1.f, 1.f,
		1.f,
		1.f, -1.f, 1.f, 1.f ) );
	m_lights.push_back( Light( Rgba::WHITE.FloatRepresentation(), Vector3::ZERO, 10.f ) );
	m_lights.push_back( Light( Rgba::WHITE.FloatRepresentation(), Vector3::ZERO, 10.f ) );

	// Setting Light Intensity
	float lightIntensity = 1000.0f;
	for ( int lightIndex = 0; lightIndex < m_maxLights; ++lightIndex )
	{
		m_lights[ lightIndex ].m_color.w = lightIntensity;
	}
}


//-----------------------------------------------------------------------------------------------
TheGame::~TheGame()
{
	delete m_planeMesh;
	m_planeMesh = nullptr;
	delete m_programDot3;
	m_programDot3 = nullptr;
	delete m_materialOrange;
	m_materialOrange = nullptr;
	delete m_materialWhite;
	m_materialWhite = nullptr;
	m_cubeMeshRenderer.erase( m_cubeMeshRenderer.begin(), m_cubeMeshRenderer.end() );
	m_cubeMeshRenderer.clear();
	m_lightMeshRenderer.erase( m_lightMeshRenderer.begin(), m_lightMeshRenderer.end() );
	m_lightMeshRenderer.clear();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	g_updateLoopStart = ( float ) GetCurrentTimeSeconds();
	m_age += deltaSeconds;

	HandleMouseAndKeyboardInput();
	UpdateCameraMovement( deltaSeconds );

	g_camera.FixAndClampAngles();

	// Only call emitter Update() if an emitter is active
	if ( m_emitter != nullptr )
	{
		m_emitter->Update( deltaSeconds );
	}

	g_theAudioSystem->Update( deltaSeconds );

	g_deltaSeconds = deltaSeconds;

	UpdateFPSCounter( deltaSeconds );

	if ( m_motion && m_skeleton )
	{
		if ( m_motion->m_isPlaying )
		{
			m_motion->Update( m_skeleton, deltaSeconds );
		}
	}

	// Lights
	for ( int lightCount = 0; lightCount < m_maxLights; ++lightCount )
	{
		float time = m_age * 50.0f;
		float offset = ( 360.f / 16.f ) * ( float ) lightCount;
		float radius = 0.5f * SinDegrees( time * 0.9f ) + 10.5f;
		float xPos = radius * CosDegrees( time + offset );
		float yPos = radius * SinDegrees( time + offset );

		float radius2 = 0.5f * SinDegrees( time * 0.9f ) + 5.5f;
		float xPos2 = radius2 * CosDegrees( time + offset );
		float yPos2 = radius2 * SinDegrees( time + offset );

		if ( lightCount == 0 )
		{
			m_lights[ lightCount ].m_position = g_camera.m_position;
		}
		else if ( lightCount == 1 )
		{
			m_lights[ lightCount ].m_position = Vector3( 0.0f, 0.0f, 5.0f );
		}
		else if ( lightCount == 2 )
		{
			m_lights[ lightCount ].m_position = Vector3( xPos, yPos, 2.0f );
		}
		else if ( lightCount == 3 )
		{
			m_lights[ lightCount ].m_position = Vector3( xPos2, yPos2, 10.0f );
		}
	}

	if ( m_modelMeshBuilder && m_modelRenderer == nullptr )
	{
		m_modelMesh = new Mesh( m_modelMeshBuilder, VertexType_SKINNED_MESH );
		m_modelRenderer = new MeshRenderer( m_modelMesh, m_materialOrange, Transform( Vector3::ZERO, mat44_fl::identity, Vector3( 1.0f, 1.0f, 1.0f ) ) );
	}

	if ( m_skeleton )
	{
		m_boneMatrices = m_skeleton->GetBoneMatrices();
	}

	if ( m_modelMeshBuilder && m_modelRenderer )
	{
		m_modelRenderer->SetUniform( m_lights, m_lightNum );
		m_modelRenderer->SetUniform( "gCameraPosition", g_camera.m_position );
		m_modelRenderer->SetUniform( "gAge", abs( cos( m_age ) ) );
		m_modelRenderer->SetUniform( "gRenderMode", g_renderMode );

		if ( m_skeleton )
		{
			m_modelRenderer->SetUniform( "gBoneMatrices", &m_boneMatrices[ 0 ] );
		}

		m_modelRenderer->Update();
	}

	// Update skeleton joint mesh renderers
	if ( m_skeleton && m_jointMeshRenderer.size() > 0 )
	{
		for ( unsigned int jointIndex = 0; jointIndex < m_jointMeshRenderer.size(); ++jointIndex )
		{
			m_jointMeshRenderer[ jointIndex ]->SetPosition( m_skeleton->GetJointPosition( jointIndex ) );
			m_jointMeshRenderer[ jointIndex ]->SetUniform( "gCameraPosition", g_camera.m_position );
			m_jointMeshRenderer[ jointIndex ]->SetUniform( "gRenderMode", 1 );
			m_jointMeshRenderer[ jointIndex ]->Update();
		}
	}
	g_updateLoopEnd = ( float ) GetCurrentTimeSeconds();
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
			g_camera.m_orientation.m_yawDegreesAboutZ += mouseSensitivityYaw * ( float ) cursorDeltaX;
			g_camera.m_orientation.m_pitchDegreesAboutX -= mouseSensitivityPitch * ( float ) cursorDeltaY;
		}

		if ( !( g_theDeveloperConsole->m_isOpen ) )
		{
			if ( g_theInputSystem->IsKeyDown( 'W' ) )
			{
				g_camera.m_movingForward = true;
				g_camera.m_movingBackward = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'S' ) )
			{
				g_camera.m_movingForward = false;
				g_camera.m_movingBackward = true;
			}

			if ( g_theInputSystem->IsKeyDown( 'A' ) )
			{
				g_camera.m_movingLeft = true;
				g_camera.m_movingRight = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'D' ) )
			{
				g_camera.m_movingLeft = false;
				g_camera.m_movingRight = true;
			}

			if ( g_theInputSystem->IsKeyDown( VK_SPACE) )
			{
				g_camera.m_movingUp = true;
				g_camera.m_movingDown = false;
			}

			if ( g_theInputSystem->IsKeyDown( 'X' ) )
			{
				g_camera.m_movingUp = false;
				g_camera.m_movingDown = true;
			}

			if ( g_theInputSystem->IsKeyPressed( 'I' ) )
			{
				g_camera.ResetPosition();
			}

			if ( g_theInputSystem->IsKeyPressed( 'O' ) )
			{
				g_drawOriginAxes = !g_drawOriginAxes;
			}

			if ( g_theInputSystem->IsKeyPressed( 'P' ) )
			{
				if ( m_motion != nullptr )
				{
					m_motion->m_isPlaying = !m_motion->m_isPlaying;
				}
			}

			if ( g_theInputSystem->IsKeyPressed( 'L' ) )
			{
				if ( g_lightMode < 4 )
				{
					g_lightMode++;
				}
				else
				{
					g_lightMode = 0;
				}
			}

			if ( g_theInputSystem->IsKeyPressed( '0' ) )
			{
				if ( g_renderMode != 0 )
				{
					g_renderMode = 0;
				}
			}

			if ( g_theInputSystem->IsKeyPressed( '1' ) )
			{
				if ( g_renderMode != 1 )
				{
					g_renderMode = 1;
				}
			}

			if ( g_theInputSystem->IsKeyPressed( '2' ) )
			{
				if ( g_renderMode != 2 )
				{
					g_renderMode = 2;
				}
			}

			if ( g_theInputSystem->IsKeyPressed( '3' ) )
			{
				if ( g_renderMode != 3 )
				{
					g_renderMode = 3;
				}
			}

			if ( g_theInputSystem->IsKeyPressed( 'M' ) )
			{
				g_drawModel = !g_drawModel;
			}

			if ( g_theInputSystem->IsKeyPressed( 'K' ) )
			{
				g_drawSkeleton = !g_drawSkeleton;
			}

			if ( g_theInputSystem->IsKeyPressed( 'J' ) )
			{
				if ( m_motion != nullptr )
				{
					if ( m_motion->m_wrapMode == WRAP_MODE_PING_PONG )
					{
						m_motion->m_wrapMode = WRAP_MODE_CLAMP;
					}
					else if ( m_motion->m_wrapMode == WRAP_MODE_CLAMP )
					{
						m_motion->m_wrapMode = WRAP_MODE_LOOP;
					}
					else if ( m_motion->m_wrapMode == WRAP_MODE_LOOP )
					{
						m_motion->m_wrapMode = WRAP_MODE_PING_PONG;
					}
				}
			}

			if ( g_theInputSystem->IsKeyPressed( 'U' ) )
			{
				if ( m_motion != nullptr )
				{
					m_motion->Reset();
				}
			}
		}

		if ( g_theInputSystem->IsKeyPressed( 0xC0 ) )
		{
			g_theDeveloperConsole->m_isOpen = !g_theDeveloperConsole->m_isOpen;
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
	g_camera.m_movingForward = false;
	g_camera.m_movingBackward = false;
	g_camera.m_movingLeft = false;
	g_camera.m_movingRight = false;
	g_camera.m_movingUp = false;
	g_camera.m_movingDown = false;
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	g_renderLoopStart = ( float ) GetCurrentTimeSeconds();

	// Clear the screen (color and depths)
	g_theRenderer->ClearScreen( Rgba::GRAY ); // Affects the Back Buffer (Default FBO)

	SetUp3DView();
	DrawScene();
	DrawDebugStuff3D();

 	SetUp2DView();
 	DrawHUD();
 	DrawDebugStuff2D();

	g_renderLoopEnd = ( float ) GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetUp3DView() const
{
	// Set up a base perspective projection
	float aspect = ( 16.0f / 9.0f ); // VIRTUAL_SCREEN_WIDTH / VIRTUAL_SCREEN_HEIGHT;
	float fovDegreesVertical = 70.0f;
	float zNear = 0.1f;
	float zFar = 1000.0f;
	g_theRenderer->SetPerspective( fovDegreesVertical, aspect, zNear, zFar );

	// Set up our game's preferred world coordinate system
	g_theRenderer->RotateView( -90.0f, Vector3( 1.0f, 0.0f, 0.0f ) ); // Put +Z up
	//g_theRenderer->RotateView( 90.0f, Vector3( 0.0f, 0.0f, 1.0f ) ); // Put +X forward

	// Orient the view per the camera's orientation
	g_theRenderer->RotateView( -g_camera.m_orientation.m_pitchDegreesAboutX, Vector3( 1.0f, 0.0f, 0.0f ) );
	//g_theRenderer->RotateView( -g_camera.m_orientation.m_pitchDegreesAboutY, Vector3( 0.0f, 1.0f, 0.0f ) );
	g_theRenderer->RotateView( g_camera.m_orientation.m_yawDegreesAboutZ, Vector3( 0.0f, 0.0f, 1.0f ) );

	// Position the view per the camera's position
	g_theRenderer->TranslateView( Vector3( -g_camera.m_position.x, -g_camera.m_position.y, -g_camera.m_position.z ) );
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetUp2DView() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawScene() const
{
	g_theRenderer->BackFaceCullingEnabled( true );
	g_theRenderer->DepthTestingEnabled( true );

	// Draw Model
	if ( m_modelRenderer && g_drawModel )
	{
		g_theRenderer->MeshRender( m_modelRenderer, &g_camera );
	}

	// Draw Skeleton
	if ( m_skeleton && g_drawSkeleton )
	{
		for ( unsigned int jointIndex = 0; jointIndex < m_jointMeshRenderer.size(); ++jointIndex )
		{
			// Draw joint
			g_theRenderer->MeshRender( m_jointMeshRenderer[ jointIndex ], &g_camera );

			// Draw bone
			int parentJointIndex = m_skeleton->GetIndexOfParent( jointIndex );
			if ( parentJointIndex < 0 )
			{
				continue;
			}

			Vector3 parentJointPos = m_skeleton->GetJointPosition( parentJointIndex );
			Vector3 thisJointPos = m_skeleton->GetJointPosition( jointIndex );

			g_theRenderer->DrawLine3D( parentJointPos, thisJointPos, Rgba::GREEN, 1.0f );
		}
	}
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

	std::string camPosString = Stringf( "camPos=(%.02f, %.02f, %.02f)", g_camera.m_position.x, g_camera.m_position.y, g_camera.m_position.z );
	std::string camOrientString = Stringf( "pitch=%.0f  roll=%.0f  yaw=%.0f", g_camera.m_orientation.m_pitchDegreesAboutX, g_camera.m_orientation.m_rollDegreesAboutY, g_camera.m_orientation.m_yawDegreesAboutZ );
	std::string camFwdString = Stringf( "fwd=(%.02f, %.02f, %.02f)", g_camera.GetForwardXYZ().x, g_camera.GetForwardXYZ().y, g_camera.GetForwardXYZ().z );
	std::string fpsString = Stringf( "FPS:%.0f", g_fps );
	
	g_theRenderer->DrawText2D( Vector2( 0.0f, 775.0f ), "UPDATE LOOP TIME: " + Stringf( "%.02f", ( g_updateLoopEnd - g_updateLoopStart ) ) + "sec", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 750.0f ), "RENDER LOOP TIME: " + Stringf( "%.02f", ( g_renderLoopEnd - g_renderLoopStart ) ) + "sec", 15.0f, Rgba::GREEN, fixedFont );

	if ( m_modelMeshBuilder != nullptr )
	{
		g_theRenderer->DrawText2D( Vector2( 0.0f, 500.0f ), "VERTICES: " + Stringf( "%i", m_modelMeshBuilder->GetVertexData().size()), 15.0f, Rgba::GREEN, fixedFont );
		if ( g_reductionTimeStart != 0.0f )
		{
			g_theRenderer->DrawText2D( Vector2( 0.0f, 475.0f ), "TIME TO REDUCE: " + Stringf( "%.02f", ( g_reductionTimeEnd - g_reductionTimeStart ) ) + "sec", 15.0f, Rgba::GREEN, fixedFont );
		}
	}

 	g_theRenderer->DrawText2D( Vector2( 0.0f, 700.0f ), "(i) camera reset", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 675.0f ), "(m) draw model", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 650.0f ), "(k) draw skeleton", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 625.0f ), "(j) wrap mode", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 600.0f ), "(u) anim reset", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 575.0f ), "(p) anim pause", 15.0f, Rgba::GREEN, fixedFont );

 	g_theRenderer->DrawText2D( Vector2( 0.0f, 825.0f ), "Camera Movement: W, A, S, D, X, Spacebar, Mouse", 15.0f, Rgba::GREEN, fixedFont );
 	g_theRenderer->DrawText2D( Vector2( 0.0f, 875.0f ), camPosString, 15.0f, Rgba::GREEN, fixedFont );
 	g_theRenderer->DrawText2D( Vector2( 500.0f, 875.0f ), camOrientString, 15.0f, Rgba::GREEN, fixedFont );
 	g_theRenderer->DrawText2D( Vector2( 1000.0f, 875.0f ), camFwdString, 15.0f, Rgba::GREEN, fixedFont );
 	g_theRenderer->DrawText2D( Vector2( 1500.0f, 875.0f ), fpsString, 15.0f, Rgba::GREEN, fixedFont );

	if ( m_motion != nullptr )
	{
		std::string motionTimeString = Stringf( "Motion Time: %.02f", m_motion->m_currentTime );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 425.0f ), motionTimeString, 15.0f, Rgba::GREEN, fixedFont );
		
		std::string wrapModeString;
		if ( m_motion->m_wrapMode == WRAP_MODE_CLAMP )
		{
			wrapModeString = "Wrap Mode: Clamp";
		}
		else if ( m_motion->m_wrapMode == WRAP_MODE_LOOP )
		{
			wrapModeString = "Wrap Mode: Loop";
		}
		else if ( m_motion->m_wrapMode == WRAP_MODE_PING_PONG )
		{
			wrapModeString = "Wrap Mode: Ping Pong";
		}
		g_theRenderer->DrawText2D( Vector2( 0.0f, 400.0f ), wrapModeString, 15.0f, Rgba::GREEN, fixedFont );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateCameraMovement( float deltaSeconds )
{
	float speedMultiplier;

	if ( g_theInputSystem->IsKeyDown( VK_SHIFT ) )
	{
		speedMultiplier = 3.0f;
	}
	else
	{
		speedMultiplier = 1.0f;
	}

	Vector3 acceleration = Vector3( 0.0f, 0.0f, 0.0f );

	if ( g_camera.m_movingForward )
	{
		g_camera.m_position += g_camera.GetForwardXY() * CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;

	}
	else if ( g_camera.m_movingBackward )
	{
		g_camera.m_position -= g_camera.GetForwardXY() * CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}

	if ( g_camera.m_movingLeft )
	{
		g_camera.m_position += g_camera.GetLeftXY() * CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}
	else if ( g_camera.m_movingRight )
	{
		g_camera.m_position -= g_camera.GetLeftXY() * CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}

	if ( g_camera.m_movingUp )
	{
		g_camera.m_position.z += CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}
	else if ( g_camera.m_movingDown )
	{
		g_camera.m_position.z -= CAMERA_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::KillVelocityInDirection( const Vector3& normalVector )
{
	if ( normalVector.x != 0.0f )
	{
		g_camera.m_velocity.x = 0.0f;
	}

	if ( normalVector.y != 0.0f )
	{
		g_camera.m_velocity.y = 0.0f;
	}

	if ( normalVector.z != 0.0f )
	{
		g_camera.m_velocity.z = 0.0f;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::ClearBuilderPointers( MeshBuilder*& meshBuilder, MeshRenderer*& modelRenderer, Mesh*& mesh )
{
	delete meshBuilder;
	meshBuilder = nullptr;

	delete modelRenderer;
	modelRenderer = nullptr;

	delete mesh;
	mesh = nullptr;
}


//-----------------------------------------------------------------------------------------------
void TheGame::PopulateJointMeshRenderer( Skeleton* skeleton )
{
	if ( m_jointMeshRenderer.size() > 0 )
	{
		m_jointMeshRenderer.clear();
	}

	for ( unsigned int jointIndex = 0; jointIndex < skeleton->m_jointNames.size(); ++jointIndex )
	{
		if ( jointIndex == 0 )
		{
			m_jointMeshRenderer.push_back( new MeshRenderer( m_sphereMesh, m_materialWhite,
				Transform( skeleton->GetJointPosition( jointIndex ), mat44_fl::identity,
					Vector3( 0.5f, 0.5f, 0.5f ) ), RenderState::BASIC_2D ) );
		}
		else
		{
			m_jointMeshRenderer.push_back( new MeshRenderer( m_sphereMesh, m_materialWhite,
				Transform( skeleton->GetJointPosition( jointIndex ), mat44_fl::identity,
					Vector3( 0.1f, 0.1f, 0.1f ) ), RenderState::BASIC_2D ) );
		}
	}
}