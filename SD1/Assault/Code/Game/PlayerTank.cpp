//-----------------------------------------------------------------------------------------------
// PlayerTank.cpp
// Tank controlled by player


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Map.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerTank.hpp"


//-----------------------------------------------------------------------------------------------
PlayerTank::PlayerTank( const WorldCoords& tankPosition )
	: m_isCloaked( false )
{
	SetPosition( tankPosition );
	SetVelocity( Vector2( 0.0f, 0.0f ) );

	SetOrientationDegrees( 90.0f );
	SetAngularVelocityDegreesPerSecond( 0.0f );
	SetPhysicsRadius( 0.4f );
	SetCosmeticRadius( 0.4f );

	SetBoundingBoxMins( Vector2( -0.4f, -0.4f ) );
	SetBoundingBoxMaxs( Vector2( 0.4f, 0.4f ) );

	SetTexture( Texture::CreateOrGetTexture( TANK_TEXTURE_FILE ) ); //  // #Eiserloh: Should be a static function; See comments on this function

	SetTint( Rgba( 255, 255, 255, 255 ) );

	SetHealth( PLAYER_TANK_STARTING_HEALTH );

	m_type = ENTITY_TYPE_PLAYER_TANK;
}



//-----------------------------------------------------------------------------------------------
void PlayerTank::Update( float deltaSeconds )
{
	if ( GetIsDead() )
		return;

	if ( GetHealth() <= 0 )
	{
		SetIsDead( true );
		g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( EXPLOSION_SOUND_FILE ), 1.0f );
		g_theMap->SpawnExplosion( GetPosition(), 2.0f, 0.75f );
		g_theGame->StartPauseModeCountdown();
		return;
	}

	if ( m_isCloaked )
	{
		SetTint(Rgba( 255, 255, 255, 127 ) );
	}
	else
	{
		SetTint( Rgba( 255, 255, 255, 255 ) );
	}

	Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();
	Vector2 xboxRightStickPosition = g_theApp->GetXboxRightStickPosition();

	if ( g_theApp->GetKeyDownStatus( 'W' ) || g_theApp->GetKeyDownStatus( VK_UP ) // keyboard input
		 || ( ( xboxLeftStickPosition.Length() > 0 ) && ( xboxRightStickPosition.Length() > 0 ) && ( xboxLeftStickPosition.y > 0 ) && ( xboxRightStickPosition.y > 0 ) ) // Xbox controller input
		 && !GetIsDead() )
	{
		MoveForward();
	}
	else if ( g_theApp->GetKeyDownStatus( 'S' ) || g_theApp->GetKeyDownStatus( VK_DOWN ) // keyboard input
		|| ( ( xboxLeftStickPosition.Length() > 0 ) && ( xboxRightStickPosition.Length() > 0 ) && ( xboxLeftStickPosition.y < 0 ) && ( xboxRightStickPosition.y < 0 ) ) // Xbox controller input
		&& !GetIsDead() )
	{
		MoveBackward();
	}
	else
	{
		SetVelocity( Vector2( 0.f, 0.f ) );
	}

	Turn( deltaSeconds );

	Entity::Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void PlayerTank::MoveForward()
{
	float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
	Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

	Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();
	Vector2 xboxRightStickPosition = g_theApp->GetXboxRightStickPosition();

	if  ( ( xboxLeftStickPosition.Length() > 0 ) && ( xboxRightStickPosition.Length() > 0 ) && ( xboxLeftStickPosition.y > 0 ) && ( xboxRightStickPosition.y > 0 ) )
	{
		Vector2 velocityFromLeftStick = ( forwardDirection * xboxLeftStickPosition.Length() * TANK_SPEED ) * 0.5f;
		Vector2 velocityFromRightStick = ( forwardDirection * xboxRightStickPosition.Length() * TANK_SPEED ) * 0.5f;
		SetVelocity( velocityFromLeftStick + velocityFromRightStick );
	}
	else
	{
		SetVelocity( forwardDirection * TANK_SPEED);
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerTank::MoveBackward()
{
	float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
	Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

	Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();
	Vector2 xboxRightStickPosition = g_theApp->GetXboxRightStickPosition();

	if ( ( xboxLeftStickPosition.Length() > 0 ) && ( xboxRightStickPosition.Length() > 0 ) && ( xboxLeftStickPosition.y < 0 ) && ( xboxRightStickPosition.y < 0 ) )
	{
		Vector2 velocityFromLeftStick = ( -forwardDirection * xboxLeftStickPosition.Length() * TANK_SPEED ) * 0.5f;
		Vector2 velocityFromRightStick = ( -forwardDirection * xboxRightStickPosition.Length() * TANK_SPEED ) * 0.5f;
		SetVelocity( velocityFromLeftStick - velocityFromRightStick );
	}
	else
	{
		SetVelocity( -forwardDirection * TANK_SPEED );
	}
}


//-----------------------------------------------------------------------------------------------
void PlayerTank::Turn( float deltaSeconds )
{
	Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();
	Vector2 xboxRightStickPosition = g_theApp->GetXboxRightStickPosition();

	float degreesToTurnThisFrame = TANK_DEGREES_TO_TURN_PER_SECOND * deltaSeconds;

	float turnDirection = 0.f; // #Eiserloh: 1.0 means "turn counter-clockwise" (positive direction), -1.0 means "turn clockwise" (negative direction)

	if ( g_theApp->GetKeyDownStatus( 'A' ) || g_theApp->GetKeyDownStatus( VK_LEFT ) // keyboard input
		|| ( ( xboxLeftStickPosition.Length() > 0 ) && ( xboxRightStickPosition.Length() > 0 ) && ( xboxLeftStickPosition.y < 0 ) && ( xboxRightStickPosition.y > 0 ) ) // Xbox controller input
		&& !GetIsDead() )
		turnDirection += 1.0f;
	else if ( g_theApp->GetKeyDownStatus( 'D' ) || g_theApp->GetKeyDownStatus( VK_RIGHT ) // keyboard input
		|| ( ( xboxLeftStickPosition.Length() > 0 ) && ( xboxRightStickPosition.Length() > 0 ) && ( xboxLeftStickPosition.y > 0 ) && ( xboxRightStickPosition.y < 0 ) ) // Xbox controller input
		&& !GetIsDead() )
		turnDirection += -1.0f;

	degreesToTurnThisFrame *= turnDirection; // May be zero if not turning
	SetOrientationDegrees( GetOrientationDegrees() + degreesToTurnThisFrame );
}


//-----------------------------------------------------------------------------------------------
void PlayerTank::Render() const
{
	if ( GetIsDead() )
		return;

	g_theRenderer->PushView();
	Vector3 positionAsVector3( GetPosition().x, GetPosition().y, 0.0f );
	g_theRenderer->TranslateView( positionAsVector3 );
	g_theRenderer->RotateView( GetOrientationDegrees(), Vector3( 0.0f, 0.0f, 1.0f ) );
	g_theRenderer->DrawTexturedAABB2( GetBoundingBox(), *GetTexture(), Vector2( 0.f, 0.f ), Vector2( 1.f, 1.f ), GetTint() );
	g_theRenderer->PopView();
}


//-----------------------------------------------------------------------------------------------
bool PlayerTank::UsesCorrectivePhysics() const
{
	return true;
}


//-----------------------------------------------------------------------------------------------
void PlayerTank::Respawn()
{
	SetHealth( PLAYER_TANK_STARTING_HEALTH );
	SetIsDead( false );
}