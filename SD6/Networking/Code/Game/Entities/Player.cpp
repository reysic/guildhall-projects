#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/Core/TheApp.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Environment/Map.hpp"
#include "Game/Entities/Entity.hpp"
#include "Game/Entities/Player.hpp"


//-----------------------------------------------------------------------------------------------
// Constants
const float PLAYER_SPEED = 10.0f;
const float PLAYER_DEGREES_TO_TURN_PER_SECOND = 90.0f;
const float FOCUS_SPEED_SCALING_FACTOR = 0.5f;
const int PLAYER_STARTING_HEALTH = 1;
const float PLAYER_FIRING_RATE_SHOTS_PER_SECOND = 8.0f;


//-----------------------------------------------------------------------------------------------
Player::Player( const WorldCoords& playerPosition )
	: m_waitToSpawnNewPlayerBullet( false )
	, m_timeBetweenPlayerBullets( 1.0f / PLAYER_FIRING_RATE_SHOTS_PER_SECOND )
	, m_timeLastPlayerBulletFired( 0.0f )
{
	SetPosition( playerPosition );
	SetVelocity( Vector2( 0.0f, 0.0f ) );

	SetOrientationDegrees( 90.0f );
	SetAngularVelocityDegreesPerSecond( 0.0f );
	SetPhysicsRadius( 0.4f );
	SetCosmeticRadius( 0.6f );

	SetBoundingBoxMins( Vector2( -0.75f, -0.75f ) );
	SetBoundingBoxMaxs( Vector2( 0.75f, 0.75f ) );

	SetTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" ) );

	SetTint( Rgba::BLUE );

	SetHealth( PLAYER_STARTING_HEALTH );

	m_type = ENTITY_TYPE_PLAYER;
}



//-----------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	if ( GetIsDead() )
		return;

	if ( GetHealth() <= 0 )
	{
		SetIsDead( true );
	}

	// Check for 'VK_SPACE' which spawns another player bullet
	if ( g_theInputSystem->IsKeyDown( VK_SPACE ) && !m_waitToSpawnNewPlayerBullet )
	{
		if ( !IsFocused() )
		{
			g_theMap->SpawnPlayerBullet( m_position, m_orientationDegrees, m_cosmeticRadius );
		}
		else
		{
			g_theMap->SpawnPlayerBullet( m_position, GetOrientationDegrees(), m_cosmeticRadius );
			g_theMap->SpawnPlayerBullet( m_position, GetOrientationDegrees() - 10.0f, 
				m_cosmeticRadius );
			g_theMap->SpawnPlayerBullet( m_position, GetOrientationDegrees() + 10.0f,
				m_cosmeticRadius );
		}
		m_waitToSpawnNewPlayerBullet = true;
		m_timeLastPlayerBulletFired = ( float ) GetCurrentTimeSeconds();
	}

	float currentTime = ( float ) GetCurrentTimeSeconds();
	float timeNextBulletCanBeFired = m_timeLastPlayerBulletFired + m_timeBetweenPlayerBullets;
	if ( m_waitToSpawnNewPlayerBullet && ( currentTime > timeNextBulletCanBeFired ) )
	{
		m_waitToSpawnNewPlayerBullet = false;
	}

	if ( g_theInputSystem->IsKeyDown( VK_SHIFT ) && !GetIsDead() )
	{
		SetIsFocused( true );
	}
	else
	{
		SetIsFocused( false );
	}

	if ( g_theInputSystem->IsKeyDown( 'W' ) && !GetIsDead() )
	{
		MoveUp();
	}
	else if ( g_theInputSystem->IsKeyDown( 'S' ) && !GetIsDead() )
	{
		MoveDown();
	}
	else if ( g_theInputSystem->IsKeyDown( 'A' ) && !GetIsDead() )
	{
		MoveLeft();
	}
	else if ( g_theInputSystem->IsKeyDown( 'D' ) && !GetIsDead() )
	{
		MoveRight();
	}
	else
	{
		SetVelocity( Vector2( 0.0f, 0.0f ) );
	}

	Entity::Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Player::MoveUp()
{
	if ( !IsFocused() )
	{
		SetVelocity( Vector2( 0.0f, 1.0f ) * PLAYER_SPEED );
	}
	else
	{
		SetVelocity( Vector2( 0.0f, 1.0f ) * PLAYER_SPEED * FOCUS_SPEED_SCALING_FACTOR );
	}
}


//-----------------------------------------------------------------------------------------------
void Player::MoveDown()
{
	if ( !IsFocused() )
	{
		SetVelocity( Vector2( 0.0f, -1.0f ) * PLAYER_SPEED );
	}
	else
	{
		SetVelocity( Vector2( 0.0f, -1.0f ) * PLAYER_SPEED * FOCUS_SPEED_SCALING_FACTOR );
	}
}


//-----------------------------------------------------------------------------------------------
void Player::MoveLeft()
{
	if ( !IsFocused() )
	{
		SetVelocity( Vector2( -1.0f, 0.0f ) * PLAYER_SPEED );
	}
	else
	{
		SetVelocity( Vector2( -1.0f, 0.0f ) * PLAYER_SPEED * FOCUS_SPEED_SCALING_FACTOR );
	}
}


//-----------------------------------------------------------------------------------------------
void Player::MoveRight()
{
	if ( !IsFocused() )
	{
		SetVelocity( Vector2( 1.0f, 0.0f ) * PLAYER_SPEED );
	}
	else
	{
		SetVelocity( Vector2( 1.0f, 0.0f ) * PLAYER_SPEED * FOCUS_SPEED_SCALING_FACTOR );
	}
}


//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
	if ( GetIsDead() )
	{
		return;
	}

	// Non-debug rendering
	//g_theRenderer->PushView();
	//Vector3 positionAsVector3( GetPosition().x, GetPosition().y, 0.0f );
	//g_theRenderer->TranslateView( positionAsVector3 );
	//g_theRenderer->RotateView( GetOrientationDegrees(), Vector3( 0.0f, 0.0f, 1.0f ) );
	//g_theRenderer->DrawTexturedAABB2( GetBoundingBox(), *GetTexture(), Vector2( 0.f, 0.f ), Vector2( 1.f, 1.f ), GetTint() );
	//g_theRenderer->PopView();

	g_theRenderer->DrawFilledPolygon( 20, GetPosition().x, GetPosition().y,
		GetPhysicsRadius(), GetTint() );

	g_theRenderer->DrawHollowPolygon( 20, GetPosition().x, GetPosition().y,
		GetPhysicsRadius(), Rgba::WHITE );

	// Draw entity's forward direction vector
	float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
	Vector2 entityForwardDirection( cos( orientationRadians ), sin( orientationRadians ) );
	Vector2 normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	Vector3 entityPositionAsVector3( GetPosition().x, GetPosition().y, 0.0f );
	Vector3 entityForwardDirectionAsVector3( entityForwardDirection.x, entityForwardDirection.y, 0.0f );
	g_theRenderer->DrawLine3D( Vector3( entityPositionAsVector3 ), Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3
			* GetPhysicsRadius()), Rgba::WHITE, 2.0f );
}