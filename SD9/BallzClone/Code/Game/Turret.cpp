//-----------------------------------------------------------------------------------------------
// Turret.cpp
// Turret that turns toward and fires at player tank


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/TheGame.hpp"
#include "Game/Map.hpp"
#include "Game/Entity.hpp"
#include "Game/Turret.hpp"
#include "Game/PlayerTank.hpp"


//-----------------------------------------------------------------------------------------------
Turret::Turret( const WorldCoords& turretPosition )
	: m_waitToSpawnNewTurretBullet( false )
	, m_timeBetweenTurretBullets( 1.0f / TURRET_FIRING_RATE_BULLETS_PER_SECOND )
	, m_timeLastTurretBulletFired( 0.0f )
{
	SetPosition( turretPosition );
	SetVelocity( Vector2( 0.f, 0.f ) );

	SetOrientationDegrees( GetRandomFloatInRange( 0.0f, 360.0f ) );
	SetAngularVelocityDegreesPerSecond( 0.f );
	SetPhysicsRadius( 0.4f );
	SetCosmeticRadius( 0.4f );

	SetBoundingBoxMins( Vector2( -0.4f, -0.4f ) );
	SetBoundingBoxMaxs( Vector2( 0.4f, 0.4f ) );

	SetTexture( Texture::CreateOrGetTexture( TURRET_TEXTURE_FILE ) ); //  // #Eiserloh: Should be a static function; See comments on this function

	SetTint( Rgba( 0.97f, 0.46f, 0.04f, 1.0f ) );

	SetHealth( TURRET_STARTING_HEALTH );

	m_type = ENTITY_TYPE_TURRET;
}


//-----------------------------------------------------------------------------------------------
void Turret::Update( float deltaSeconds )
{
	if ( GetIsDead() )
		return;

	if ( GetHealth() <= 0 )
	{
		SetIsDead( true );
		return;
	}

	if ( !( g_theMap->GetPlayerTank()->GetIsDead() ) ) // ensure player tank is not dead
	{
		WorldCoords turretPosition = GetPosition();
		WorldCoords playerTankPosition = g_theMap->GetPlayerTank()->GetPosition();

		if ( CalculateDistanceBetweenPoints( turretPosition, playerTankPosition ) <= TURRET_DETECTION_RADIUS ) // ensure player tank is within range
		{
			if ( g_theMap->HasLineOfSight( turretPosition, playerTankPosition ) ) // ensure turret can "see" player tank
			{
				Vector2 vectorToPlayerTank = playerTankPosition - turretPosition;

				m_goalOrientationDegrees = ConvertRadiansToDegrees( atan2( vectorToPlayerTank.y, vectorToPlayerTank.x ) );

				float shortestSignedAngularDistance = ComputeShortestSignedAngularDistance( GetOrientationDegrees(), m_goalOrientationDegrees );

				if ( abs( shortestSignedAngularDistance ) < TURRET_HALF_OF_DETECTION_CONE_IN_DEGREES )
					ShootAtPlayerTank();
			}
			else
			{
				m_goalOrientationDegrees = GetOrientationDegrees() + 2.0f;
			}
		}
		else
		{
			m_goalOrientationDegrees = GetOrientationDegrees() + 2.0f;
		}
	}
	else
	{
		m_goalOrientationDegrees = GetOrientationDegrees() + 2.0f;
	}

	Turn( deltaSeconds );

	Entity::Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Turret::Turn( float deltaSeconds )
{
	float degreesToTurnThisFrame = TURRET_DEGREES_TO_TURN_PER_SECOND * deltaSeconds;

	float shortestSignedAngularDistance = ComputeShortestSignedAngularDistance( GetOrientationDegrees(), m_goalOrientationDegrees );
	float turnDirection = 0.0f;

	const float angularDistanceThreshold = 1.0f;
	if ( shortestSignedAngularDistance < -angularDistanceThreshold )
	{
		turnDirection += -1.0f; // #Eiserloh: 1.0 means "turn counter-clockwise" (positive direction), -1.0 means "turn clockwise" (negative direction)
	}
	else if ( shortestSignedAngularDistance > angularDistanceThreshold )
	{
		turnDirection += 1.0f;
	}

	degreesToTurnThisFrame *= turnDirection; // May be zero if not turning
	SetOrientationDegrees( GetOrientationDegrees() + degreesToTurnThisFrame );
}


//-----------------------------------------------------------------------------------------------
void Turret::Render() const
{
	if ( GetIsDead() )
		return;

	g_theRenderer->PushView();
	g_theRenderer->TranslateView( GetPosition() );
	g_theRenderer->RotateView( GetOrientationDegrees());
	g_theRenderer->DrawTexturedAABB( GetBoundingBox(), GetTexture(), Vector2( 0.f, 0.f ), Vector2( 1.f, 1.f ), GetTint() );
	g_theRenderer->PopView();
}


//-----------------------------------------------------------------------------------------------
void Turret::ShootAtPlayerTank()
{
	if ( !m_waitToSpawnNewTurretBullet )
	{
		g_theMap->SpawnTurretBullet( this );
		m_waitToSpawnNewTurretBullet = true;
		m_timeLastTurretBulletFired = ( float ) GetCurrentTimeSeconds();
	}

	if ( m_waitToSpawnNewTurretBullet && ( ( float ) GetCurrentTimeSeconds() ) > ( m_timeLastTurretBulletFired + m_timeBetweenTurretBullets ) )
	{
		m_waitToSpawnNewTurretBullet = false;
	}
}