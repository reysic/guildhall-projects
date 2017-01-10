//-----------------------------------------------------------------------------------------------
// PlayerBullet.cpp
// Bullet fired by player tank


//-----------------------------------------------------------------------------------------------
#include <math.h>
#include <stdlib.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerBullet.hpp"


//-----------------------------------------------------------------------------------------------
PlayerBullet::PlayerBullet( const Vector2& tankPosition, float tankOrientationDegrees, float tankCosmeticRadius )
{
	SetPosition( tankPosition );

	SetOrientationDegrees( tankOrientationDegrees );

	// #Eiserloh: Which way is forward?
	float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
	Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

	// Move bullet to tip of tank
	SetPosition( GetPosition() + ( forwardDirection * tankCosmeticRadius ) );

	SetVelocity( forwardDirection * PLAYER_BULLET_SPEED );

	SetAngularVelocityDegreesPerSecond( 0.f );
	SetPhysicsRadius( 0.1f );
	SetCosmeticRadius( 0.2f );

	SetBoundingBoxMins( Vector2( -0.4f, -0.4f ) );
	SetBoundingBoxMaxs( Vector2( 0.4f, 0.4f ) );

	SetTexture( Texture::CreateOrGetTexture( PLAYER_BULLET_TEXTURE_FILE ) ); //  // #Eiserloh: Should be a static function; See comments on this function

	SetTint( Rgba( 255, 255, 255, 255 ) );

	SetHealth( PLAYER_BULLET_STARTING_HEALTH );

	m_type = ENTITY_TYPE_PLAYER_BULLET;
}


//-----------------------------------------------------------------------------------------------
void PlayerBullet::Update( float deltaSeconds )
{
	if ( GetIsDead() )
		return;

	if ( GetHealth() <= 0 )
	{
		SetIsDead( true );
		return;
	}

	Entity::Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void PlayerBullet::Render() const
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
bool PlayerBullet::IsProjectile() const
{
	return true;
}