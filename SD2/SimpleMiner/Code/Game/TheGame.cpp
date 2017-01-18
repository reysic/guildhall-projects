#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Block.hpp"
#include "Game/BlockDefinition.hpp"
#include "Game/BlockInfo.hpp"
#include "Game/Chunk.hpp"
#include "Game/World.hpp"


//-----------------------------------------------------------------------------------------------
bool g_waitForF5KeyUp = false;
bool g_waitForPKeyUp = false;


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_world( new World() )
	, m_physicsMode( ( unsigned char ) PHYSICS_MODE_WALKING )
	, m_cameraMode( ( unsigned char ) CAMERA_MODE_FROM_BEHIND )
{
	LoadPlayerFromDisk();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	HandleMouseAndKeyboardInput();
	UpdatePlayerMovement( deltaSeconds );

	m_camera.FixAndClampAngles();
	FixAndClampPlayerAngles();

	UpdateCameraPositionAndOrientation();

	m_world->Update( deltaSeconds );

	g_theAudioSystem->Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleMouseAndKeyboardInput()
{
	ClearPlayerMoveStates();

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

		m_player.m_orientation.m_yawDegreesAboutZ -= mouseSensitivityYaw * ( float ) cursorDeltaX;
		m_player.m_orientation.m_pitchDegreesAboutY += mouseSensitivityPitch * ( float ) cursorDeltaY;

		Vector3 playerForwardXY = GetPlayerForwardXY();
		Vector3 playerLeftXY = GetPlayerLeftXY();

		if ( g_theInputSystem->GetKeyDownStatus( 'E' ) )
		{
			m_player.m_movingForward = true;
			m_player.m_movingBackward= false;
		}

		if ( g_theInputSystem->GetKeyDownStatus( 'D' ) )
		{
			m_player.m_movingForward = false;
			m_player.m_movingBackward = true;
		}

		if ( g_theInputSystem->GetKeyDownStatus( 'S' ))
		{
			m_player.m_movingLeft = true;
			m_player.m_movingRight = false;
		}

		if ( g_theInputSystem->GetKeyDownStatus( 'F' ) )
		{
			m_player.m_movingLeft = false;
			m_player.m_movingRight = true;
		}

		if ( g_theInputSystem->GetSpaceBarDownStatus() )
		{
			m_player.m_movingUp = true;
			m_player.m_movingDown = false;
		}

		if ( ( g_theInputSystem->GetKeyDownStatus( 'Z' ) || g_theInputSystem->GetKeyDownStatus( 'X' ) || g_theInputSystem->GetKeyDownStatus( 'C' ) ) )
		{
			m_player.m_movingUp = false;
			m_player.m_movingDown = true;
		}

		if ( g_theInputSystem->GetKeyDownStatus( '1' ) )
		{
			if ( g_hudSelectedBlockType != 1 )
			{
				g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_TYPE_CHANGE_SOUND_FILE ), 1.0f );
				g_hudSelectedBlockType = 1;
			}
		}

		if ( g_theInputSystem->GetKeyDownStatus( '2' ) )
		{
			if ( g_hudSelectedBlockType != 2 )
			{
				g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_TYPE_CHANGE_SOUND_FILE ), 1.0f );
				g_hudSelectedBlockType = 2;
			}
		}

		if ( g_theInputSystem->GetKeyDownStatus( '3' ) )
		{
			if ( g_hudSelectedBlockType != 3 )
			{
				g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_TYPE_CHANGE_SOUND_FILE ), 1.0f );
				g_hudSelectedBlockType = 3;
			}
		}

		if ( g_theInputSystem->GetKeyDownStatus( '4' ) )
		{
			if ( g_hudSelectedBlockType != 4 )
			{
				g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_TYPE_CHANGE_SOUND_FILE ), 1.0f );
				g_hudSelectedBlockType = 4;
			}
		}

		if ( g_theInputSystem->GetKeyDownStatus( '5' ) )
		{
			if ( g_hudSelectedBlockType != 5 )
			{
				g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_TYPE_CHANGE_SOUND_FILE ), 1.0f );
				g_hudSelectedBlockType = 5;
			}
		}

		if ( g_theInputSystem->GetKeyDownStatus( '6' ) )
		{
			if ( g_hudSelectedBlockType != 6 )
			{
				g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_TYPE_CHANGE_SOUND_FILE ), 1.0f );
				g_hudSelectedBlockType = 6;
			}
		}

		if ( g_theInputSystem->GetKeyDownStatus( 0x74 ) && !g_waitForF5KeyUp ) // 0x74 - VK_F5
		{
			if ( m_cameraMode == CAMERA_MODE_FIRST_PERSON )
				m_cameraMode = ( unsigned char ) CAMERA_MODE_FROM_BEHIND;
			else if ( m_cameraMode == CAMERA_MODE_FROM_BEHIND )
				m_cameraMode = ( unsigned char ) CAMERA_MODE_FIXED_ANGLE;
			else
				m_cameraMode = ( unsigned char ) CAMERA_MODE_FIRST_PERSON;

			g_waitForF5KeyUp = true;
		}

		if ( !g_theInputSystem->GetKeyDownStatus( 0x74 ) )
		{
			g_waitForF5KeyUp = false;
		}

		if ( g_theInputSystem->GetKeyDownStatus( 'P' ) && !g_waitForPKeyUp )
		{
			if ( m_physicsMode == PHYSICS_MODE_WALKING )
				m_physicsMode = ( unsigned char ) PHYSICS_MODE_FLYING;
			else if ( m_physicsMode == PHYSICS_MODE_FLYING )
				m_physicsMode = ( unsigned char ) PHYSICS_MODE_NOCLIP;
			else
				m_physicsMode = ( unsigned char ) PHYSICS_MODE_WALKING;

			g_waitForPKeyUp = true;
		}

		if ( !g_theInputSystem->GetKeyDownStatus( 'P' ) )
		{
			g_waitForPKeyUp = false;
		}
	}
	else
	{
		g_theInputSystem->ShowMouseCursor();
	}
}

//-----------------------------------------------------------------------------------------------
void TheGame::ClearPlayerMoveStates()
{
	m_player.m_movingForward = false;
	m_player.m_movingBackward = false;
	m_player.m_movingLeft = false;
	m_player.m_movingRight = false;
	m_player.m_movingUp = false;
	m_player.m_movingDown = false;
}

//-----------------------------------------------------------------------------------------------
void TheGame::UpdateCameraPositionAndOrientation()
{
	Vector3 displacementFromEye( -9.0f, -6.0f, 5.0f );

	// Set the camera's position and orientation based upon camera mode
	switch ( m_cameraMode )
	{
		case 0: // FIRST_PERSON
		{
			m_camera.m_orientation = m_player.m_orientation;
			m_camera.m_position = Vector3( m_player.m_position.x, m_player.m_position.y, m_player.m_position.z + PLAYER_EYE_LEVEL );
			break;
		}
		case 1: // FROM_BEHIND
		{
			m_camera.m_orientation = m_player.m_orientation;
			Vector3 raycastStart = Vector3( m_player.m_position.x, m_player.m_position.y, m_player.m_position.z + PLAYER_EYE_LEVEL );
			m_camera.m_position = Vector3( m_player.m_position.x, m_player.m_position.y, m_player.m_position.z + PLAYER_EYE_LEVEL );
			Vector3 raycastEnd = raycastStart - ( m_camera.GetForwardXYZ() * 4.0f );
			RaycastResult3D raycastResult = m_world->AWRaycast3D( raycastStart, raycastEnd );
			if ( raycastResult.m_didHit )
			{
				m_camera.m_position -= ( m_camera.GetForwardXYZ() * 4.0f * raycastResult.m_fractionComplete );
			}
			else
			{
				m_camera.m_position -= ( m_camera.GetForwardXYZ() * 4.0f );
			}
			break;
		}
		case 2: // FIXED_ANGLE
		{
			m_camera.m_orientation = EulerAngles( 0.0f, 20.0f, 45.0f );
			Vector3 raycastStart = Vector3( m_player.m_position.x, m_player.m_position.y, m_player.m_position.z + PLAYER_EYE_LEVEL );
			m_camera.m_position = Vector3( m_player.m_position.x, m_player.m_position.y, m_player.m_position.z + PLAYER_EYE_LEVEL );
			Vector3 raycastEnd = raycastStart + displacementFromEye;
			RaycastResult3D raycastResult = m_world->AWRaycast3D( raycastStart, raycastEnd );
			if ( raycastResult.m_didHit )
			{
				m_camera.m_position += ( displacementFromEye * raycastResult.m_fractionComplete );
			}
			else
			{
				m_camera.m_position += displacementFromEye;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	// Clear the screen (color and depths)
	g_theRenderer->ClearScreen( Rgba::CYAN );

	SetUp3DView();
	m_world->Render();
	DrawDebugStuff3D();

	SetUp2DView();
	DrawHUD();
	DrawDebugStuff2D();
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
	g_theRenderer->RotateView( 90.0f, Vector3( 0.0f, 0.0f, 1.0f ) ); // Put +X forward

	// Orient the view per the camera's orientation
	g_theRenderer->RotateView( -m_camera.m_orientation.m_rollDegreesAboutX, Vector3( 1.0f, 0.0f, 0.0f ) );
	g_theRenderer->RotateView( -m_camera.m_orientation.m_pitchDegreesAboutY, Vector3( 0.0f, 1.0f, 0.0f ) );
	g_theRenderer->RotateView( -m_camera.m_orientation.m_yawDegreesAboutZ, Vector3( 0.0f, 0.0f, 1.0f ) );

	// Position the view per the camera's position
	g_theRenderer->TranslateView( Vector3( -m_camera.m_position.x, -m_camera.m_position.y, -m_camera.m_position.z ) );
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetUp2DView() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawDebugStuff3D() const
{
	g_theRenderer->DrawDebugAxisLines();

	// Draw player raycast hit position
// 	if ( m_world->m_raycastResult.m_didHit )
// 	{
// 		Vector3 hitPosition = m_world->m_raycastResult.m_hitPosition;
// 		float markerRadius = 0.5f;
// 		Vector3 above = hitPosition + Vector3( 0.0f, 0.0f, markerRadius );
// 		Vector3 below = hitPosition + Vector3( 0.0f, 0.0f, -markerRadius );
// 		Vector3 east = hitPosition + Vector3( markerRadius, 0.0f, 0.0f );
// 		Vector3 west = hitPosition + Vector3( -markerRadius, 0.0f, 0.0f );
// 		Vector3 north = hitPosition + Vector3( 0.0f, markerRadius, 0.0f );
// 		Vector3 south = hitPosition + Vector3( 0.0f, -markerRadius, 0.0f );
// 		Vector3 norm = hitPosition + ( m_world->m_raycastResult.m_faceNormal );
// 
// 		g_theRenderer->DrawLine3D( hitPosition, above, Rgba::WHITE, 5.0f );
// 		g_theRenderer->DrawLine3D( hitPosition, below, Rgba::WHITE, 5.0f );
// 		g_theRenderer->DrawLine3D( hitPosition, east, Rgba::WHITE, 5.0f );
// 		g_theRenderer->DrawLine3D( hitPosition, west, Rgba::WHITE, 5.0f );
// 		g_theRenderer->DrawLine3D( hitPosition, north, Rgba::WHITE, 5.0f );
// 		g_theRenderer->DrawLine3D( hitPosition, south, Rgba::WHITE, 5.0f );
// 		g_theRenderer->DisableDepthTesting();
// 		g_theRenderer->DrawLine3D( hitPosition, norm, Rgba::RED, 5.0f );
// 	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawHUD() const
{
	g_theRenderer->DrawCrosshairs( Vector2( 800.0f, 450.0f ), Rgba::WHITE );

	static SpriteSheet spriteSheet( "Data/Images/SimpleMinerAtlas.png", 16, 16 );

	float minY = 50.0f;
	float maxY = 125.0f;
	float currentX = 800.0f - ( 7.0f * 37.5f );

	for ( int blockDefinitionIndex = 1; blockDefinitionIndex < 7.0f; blockDefinitionIndex++ )
	{
		BlockDefinition& blockDef = g_blockDefinitions[ blockDefinitionIndex ];
		const AABB2& sideTexCoords = blockDef.m_sideTexCoords;

		if ( blockDefinitionIndex == g_hudSelectedBlockType )
			g_theRenderer->DrawQuad( Vector2( currentX - 3.0f, minY - 3.0f ), Vector2( currentX + 78.0f, maxY + 3.0f ), Rgba::WHITE );
		else
			g_theRenderer->DrawQuad( Vector2( currentX - 3.0f, minY - 3.0f ), Vector2( currentX + 78.0f, maxY + 3.0f ), Rgba::BLACK );

		g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( currentX, minY ), Vector2( currentX + 75.0f, maxY ) ), spriteSheet.GetTexture(), sideTexCoords.mins, sideTexCoords.maxs, Rgba::WHITE );

		currentX += 100.0f;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawDebugStuff2D() const
{
	static BitmapFont* font = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	Vector3 camPos = m_camera.m_position;
	EulerAngles camOrient = m_camera.m_orientation;
	Vector3 camForward = m_camera.GetForwardXYZ();
	Vector3 playerPos = m_player.m_position;
	EulerAngles playerOrient = m_player.m_orientation;
	Vector3 playerForward = GetPlayerForwardXYZ();
	int numActiveChunks = m_world->m_activeChunks.size();
	int numRenderedChunks = m_world->m_numRenderedChunks;
	int numSavedChunks = m_world->m_numSavedChunks;
	int numLoadedChunks = m_world->m_numLoadedChunks;
	std::string camPosString = Stringf( "camPos=(%.02f, %.02f, %.02f)", camPos.x, camPos.y, camPos.z );
	std::string camOrientString = Stringf( "roll=%.0f  pitch=%.0f  yaw=%.0f", camOrient.m_rollDegreesAboutX, camOrient.m_pitchDegreesAboutY, camOrient.m_yawDegreesAboutZ );
	std::string camFwdString = Stringf( "fwd=(%.02f, %.02f, %.02f)", camForward.x, camForward.y, camForward.z );
	std::string playerPosString = Stringf( "playerPos=(%.02f, %.02f, %.02f)", playerPos.x, playerPos.y, playerPos.z );
	std::string playerOrientString = Stringf( "roll=%.0f  pitch=%.0f  yaw=%.0f", playerOrient.m_rollDegreesAboutX, playerOrient.m_pitchDegreesAboutY, playerOrient.m_yawDegreesAboutZ );
	std::string playerFwdString = Stringf( "fwd=(%.02f, %.02f, %.02f)", playerForward.x, playerForward.y, playerForward.z );
	std::string numActiveChunksString = Stringf( "numActiveChunks=%i", numActiveChunks );
	std::string numRenderedChunksString = Stringf( "numRenderedChunks=%i", numRenderedChunks );
	std::string numSavedChunksString = Stringf( "numSavedChunks=%i", numSavedChunks );
	std::string numLoadedChunksString = Stringf( "numLoadedChunks=%i", numLoadedChunks );

	g_theRenderer->DrawText2D( Vector2( 0.0f, 0.0f ), camPosString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 500.0f, 0.0f ), camOrientString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 1000.0f, 0.0f ), camFwdString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 25.0f ), playerPosString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 500.0f, 25.0f ), playerOrientString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 1000.0f, 25.0f ), playerFwdString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 885.0f ), numActiveChunksString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 350.0f, 885.0f ), numRenderedChunksString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 750.0f, 885.0f ), numSavedChunksString, 15.0f, Rgba::WHITE, font );
	g_theRenderer->DrawText2D( Vector2( 1100.0, 885.0f ), numLoadedChunksString, 15.0f, Rgba::WHITE, font );

	switch ( m_physicsMode )
	{
	case 0:
		g_theRenderer->DrawText2D( Vector2( 1480.0f, 885.0f ), "WALKING", 15.0f, Rgba::WHITE, font );
		break;
	case 1:
		g_theRenderer->DrawText2D( Vector2( 1480.0f, 885.0f ), "FLYING", 15.0f, Rgba::WHITE, font );
		break;
	case 2:
		g_theRenderer->DrawText2D( Vector2( 1480.0f, 885.0f ), "NOCLIP", 15.0f, Rgba::WHITE, font );
		break;
	default:
		break;
	}

	switch ( m_cameraMode )
	{
		case 0:
			g_theRenderer->DrawText2D( Vector2( 0.0f, 450.0f ), "FIRST-PERSON", 15.0f, Rgba::WHITE, font );
			break;
		case 1:
			g_theRenderer->DrawText2D( Vector2( 0.0f, 450.0f ), "FROM-BEHIND", 15.0f, Rgba::WHITE, font );
			break;
		case 2:
			g_theRenderer->DrawText2D( Vector2( 0.0f, 450.0f ), "FIXED-ANGLE", 15.0f, Rgba::WHITE, font );
			break;
		default:
			break;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::LoadPlayerFromDisk()
{
	const std::string filePath = Stringf( "Data/SaveSlot0/PlayerInfo.player" );

	std::vector< float > buffer;
	bool success = LoadBinaryFileToBufferFloat( filePath, buffer );

	if ( success )
	{
		m_player.m_position = Vector3( buffer[ 0 ], buffer[ 1 ], buffer[ 2 ] );
		m_player.m_orientation = EulerAngles( buffer[ 3 ], buffer[ 4 ], buffer[ 5 ] );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::FixAndClampPlayerAngles()
{
	while ( m_player.m_orientation.m_yawDegreesAboutZ < 0.0f )
	{
		m_player.m_orientation.m_yawDegreesAboutZ += 360.0f;
	}

	while ( m_player.m_orientation.m_yawDegreesAboutZ >= 360.0f )
	{
		m_player.m_orientation.m_yawDegreesAboutZ -= 360.0f;
	}

	m_player.m_orientation.m_pitchDegreesAboutY = ClampFloat( m_player.m_orientation.m_pitchDegreesAboutY, -89.9f, 89.9f );
	m_player.m_orientation.m_rollDegreesAboutX = ClampFloat( m_player.m_orientation.m_rollDegreesAboutX, -89.9f, 89.9f );
}


//-----------------------------------------------------------------------------------------------
Vector3 TheGame::GetPlayerForwardXYZ() const
{
	float cosYaw = CosDegrees( m_player.m_orientation.m_yawDegreesAboutZ );
	float sinYaw = SinDegrees( m_player.m_orientation.m_yawDegreesAboutZ );

	float cosPitch = CosDegrees( m_player.m_orientation.m_pitchDegreesAboutY );
	float sinPitch = SinDegrees( m_player.m_orientation.m_pitchDegreesAboutY );

	return Vector3( cosYaw * cosPitch, sinYaw * cosPitch, -sinPitch );
}


//-----------------------------------------------------------------------------------------------
Vector3 TheGame::GetPlayerForwardXY() const
{
	float cosYaw = CosDegrees( m_player.m_orientation.m_yawDegreesAboutZ );
	float sinYaw = SinDegrees( m_player.m_orientation.m_yawDegreesAboutZ );
	return Vector3( cosYaw, sinYaw, 0.0f );
}


//-----------------------------------------------------------------------------------------------
Vector3 TheGame::GetPlayerLeftXY() const
{
	Vector3 forwardXY = GetPlayerForwardXY();
	return Vector3( -forwardXY.y, forwardXY.x, 0.0f );
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdatePlayerMovement( float deltaSeconds )
{
	float speedMultiplier;

	if ( g_theInputSystem->GetShiftKeyDownStatus() )
	{
		speedMultiplier = 1.5f;
	}
	else
	{
		speedMultiplier = 1.0f;
	}

	Vector3 acceleration = Vector3( 0.0f, 0.0f, 0.0f );

	switch ( m_physicsMode )
	{
	case 0: // WALKING, physics applied, gravity applied, horizontal friction when grounded
		if ( m_player.m_movingForward )
		{
			acceleration += GetPlayerForwardXY() * PLAYER_MOVE_SPEED * speedMultiplier;

		}
		else if ( m_player.m_movingBackward )
		{
			acceleration -= GetPlayerForwardXY() * PLAYER_MOVE_SPEED * speedMultiplier;
		}

		if ( m_player.m_movingLeft )
		{
			acceleration += GetPlayerLeftXY() * PLAYER_MOVE_SPEED * speedMultiplier;
		}
		else if ( m_player.m_movingRight )
		{
			acceleration -= GetPlayerLeftXY() * PLAYER_MOVE_SPEED * speedMultiplier;
		}

		m_player.m_velocity += ( acceleration * deltaSeconds );
		m_player.m_velocity.x *= WALKING_SPEED_CONSERVATION;
		m_player.m_velocity.y *= WALKING_SPEED_CONSERVATION;

		// Is the player grounded?
		if ( m_world->PlayerGrounded() )
		{
			m_player.m_velocity.z = 0.0f;
			acceleration.z = 0.0f;
		}

		if ( m_player.m_movingUp && m_world->PlayerGrounded() )
		{
			m_player.m_velocity.z += JUMP_IMPULSE;
			acceleration.z = 0.0f;

		}

		// Apply gravity
		acceleration.z -= GRAVITY;
		m_player.m_velocity.z += ( acceleration.z * deltaSeconds );

		DoPreventativePhysics( deltaSeconds );

		break;
	case 1: // FLYING, physics applied, no gravity applied, horizontal & vertical friction
		if ( m_player.m_movingForward )
		{
			acceleration += GetPlayerForwardXY() * PLAYER_MOVE_SPEED * speedMultiplier;

		}
		else if ( m_player.m_movingBackward )
		{
			acceleration -= GetPlayerForwardXY() * PLAYER_MOVE_SPEED * speedMultiplier;
		}

		if ( m_player.m_movingLeft )
		{
			acceleration += GetPlayerLeftXY() * PLAYER_MOVE_SPEED * speedMultiplier;
		}
		else if ( m_player.m_movingRight )
		{
			acceleration -= GetPlayerLeftXY() * PLAYER_MOVE_SPEED * speedMultiplier;
		}

		if ( m_player.m_movingUp )
		{
			acceleration.z += PLAYER_MOVE_SPEED * speedMultiplier;
		}
		else if ( m_player.m_movingDown )
		{
			acceleration.z -= PLAYER_MOVE_SPEED * speedMultiplier;
		}

		m_player.m_velocity += ( acceleration * deltaSeconds );
		m_player.m_velocity.x *= WALKING_SPEED_CONSERVATION;
		m_player.m_velocity.y *= WALKING_SPEED_CONSERVATION;
		m_player.m_velocity.z *= WALKING_SPEED_CONSERVATION;

		// Is the player grounded?
		if ( m_world->PlayerGrounded() )
		{
			if ( m_player.m_velocity.z < 0.0f )
			{
				m_player.m_velocity.z = 0.0f;
				acceleration.z = 0.0f;
			}
		}

		DoPreventativePhysics( deltaSeconds );
		break;
	case 2: // NO_CLIP, no physics applied, instantaneous stops (no velocity carryover)
		if ( m_player.m_movingForward )
		{
			m_player.m_position += GetPlayerForwardXY() * PLAYER_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;

		}
		else if ( m_player.m_movingBackward )
		{
			m_player.m_position -= GetPlayerForwardXY() * PLAYER_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
		}

		if ( m_player.m_movingLeft )
		{
			m_player.m_position += GetPlayerLeftXY() * PLAYER_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
		}
		else if ( m_player.m_movingRight )
		{
			m_player.m_position -= GetPlayerLeftXY() * PLAYER_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
		}

		if ( m_player.m_movingUp )
		{
			m_player.m_position.z += PLAYER_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
		}
		else if ( m_player.m_movingDown )
		{
			m_player.m_position.z -= PLAYER_MOVE_SPEED * speedMultiplier * deltaSeconds * 0.5f;
		}
		break;
	default:
		break;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateBoundingBoxAndVerts()
{
	Vector3 playerBoundingBoxCenter = m_player.m_position;
	m_player.m_boundingBox = AABB3( Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, 
											 playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, 
		                                     playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS ), 
									Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS,
											 playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS,
											 playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS ) );

	m_world->SetPlayerBoundingBoxDrawPositions( playerBoundingBoxCenter );
}


//-----------------------------------------------------------------------------------------------
void TheGame::KillVelocityInDirection( const Vector3& normalVector )
{
	if ( normalVector.x != 0.0f )
	{
		m_player.m_velocity.x = 0.0f;
	}

	if ( normalVector.y != 0.0f )
	{
		m_player.m_velocity.y = 0.0f;
	}

	if ( normalVector.z != 0.0f )
	{
		m_player.m_velocity.z = 0.0f;
	}
}


//-----------------------------------------------------------------------------------------------
// Based on code by Squirrel Eiserloh and Nick Dorbin
void TheGame::DoPreventativePhysics( float deltaSeconds )
{
	RaycastResult3D raycasts[ 12 ];
	float timeLeft = deltaSeconds;

	int priorityRayIndex = -1;

	while ( timeLeft > 0.0f )
	{
		Vector3 movementDisplacement = m_player.m_velocity * timeLeft;
		priorityRayIndex = -1;
		float shortestHitFraction = 99999998.9f;

		for ( int i = 0; i < 12; i++ )
		{
			raycasts[ i ] = m_world->AWRaycast3D( m_world->m_playerBoundingBoxVertexes[ i ], m_world->m_playerBoundingBoxVertexes[ i ] + movementDisplacement );

			if ( raycasts[ i ].m_didHit )
			{
				if ( raycasts[ i ].m_fractionComplete != -1.0f )
				{
					if ( raycasts[ i ].m_fractionComplete < shortestHitFraction )
					{
						priorityRayIndex = i;
						shortestHitFraction = raycasts[ i ].m_fractionComplete;
					}
				}
			}
		}

		if ( priorityRayIndex == -1 )
		{
			m_player.m_position += movementDisplacement;
			UpdateBoundingBoxAndVerts();
			timeLeft = 0.0f;
			return;
		}
		else
		{
			Vector3 hitPosition = raycasts[ priorityRayIndex ].m_hitPosition;
			Vector3 normalVector = raycasts[ priorityRayIndex ].m_faceNormal;

			Vector3 offset = m_player.m_position - m_world->m_playerBoundingBoxVertexes[ priorityRayIndex ];
			Vector3 movePosition = hitPosition + offset;

			m_player.m_position = movePosition;
			UpdateBoundingBoxAndVerts();
			KillVelocityInDirection( normalVector );

			timeLeft -= timeLeft * shortestHitFraction;

			if ( m_player.m_velocity== Vector3::ZERO )
			{
				timeLeft = 0.0f;
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
BlockInfo TheGame::GetBlockInfoForWorldPosition( const Vector3& worldPosition )
{
	IntVector2 chunkCoordsForCurrentPosition = m_world->GetChunkCoordsForWorldPosition( worldPosition );
	Chunk* chunkForCurrentPosition;

	if ( m_world->m_activeChunks.find( chunkCoordsForCurrentPosition ) == m_world->m_activeChunks.end() )
	{
		// Active chunk does not exist at current position
		chunkForCurrentPosition = nullptr;
	}
	else
	{
		chunkForCurrentPosition = m_world->m_activeChunks.find( chunkCoordsForCurrentPosition )->second;
	}

	IntVector3 blockLocalCoordsForCurrentPosition = m_world->GetBlockLocalCoordsForWorldPosition( worldPosition );
	IntVector3 blockGlobalCoordsForCurrentPosition = m_world->GetBlockGlobalCoordsForWorldPosition( worldPosition );

	int blockIndexWithinChunk = -1;

	if ( blockLocalCoordsForCurrentPosition.x >= 0 &&
		blockLocalCoordsForCurrentPosition.x < BLOCKS_WIDE_X &&
		blockLocalCoordsForCurrentPosition.y >= 0 &&
		blockLocalCoordsForCurrentPosition.y < BLOCKS_WIDE_Y &&
		blockLocalCoordsForCurrentPosition.z >= 0 &&
		blockLocalCoordsForCurrentPosition.z < BLOCKS_TALL_Z &&
		chunkForCurrentPosition != nullptr )
	{
		blockIndexWithinChunk = chunkForCurrentPosition->GetBlockIndexForLocalCoords( blockLocalCoordsForCurrentPosition );
	}

	if ( blockIndexWithinChunk != -1 )
	{
		return BlockInfo( chunkForCurrentPosition, blockIndexWithinChunk );
	}
	else
	{
		return BlockInfo( nullptr, 0 );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::IncrementHUDSelectedBlockType()
{
	if ( g_hudSelectedBlockType == ( 7 - 1 ) )
		g_hudSelectedBlockType = 1;
	else
		g_hudSelectedBlockType++;

	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_TYPE_CHANGE_SOUND_FILE ), 1.0f );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DecrementHUDSelectedBlockType()
{
	if ( g_hudSelectedBlockType == 1)
		g_hudSelectedBlockType = ( 7 - 1 );
	else
		g_hudSelectedBlockType--;

	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_TYPE_CHANGE_SOUND_FILE ), 1.0f );
}
