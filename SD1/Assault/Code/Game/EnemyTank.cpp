//-----------------------------------------------------------------------------------------------
// EnemyTank.cpp
// Tank controlled by AI

//-----------------------------------------------------------------------------------------------
// Turret.cpp
// Turret that turns toward and fires at player tank


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/Map.hpp"


//-----------------------------------------------------------------------------------------------
EnemyTank::EnemyTank( const WorldCoords& enemyTankPosition )
	: m_waitToPickNewGoalOrientation( true )
	, m_timeBetweenGoalOrientationChanges( ENEMY_TANK_TIME_BETWEEN_GOAL_ORIENTATION_CHANGES_IN_SECONDS )
	, m_timeLastGoalOrientationChange( (float) GetCurrentTimeSeconds() )
	, m_waitToSpawnNewEnemyTankBullet( false )
	, m_timeBetweenEnemyTankBullets( 1.0f / ENEMY_TANK_FIRING_RATE_BULLETS_PER_SECOND )
	, m_timeLastEnemyTankBulletFired( 0.0f )
{
	SetPosition( enemyTankPosition );
	SetVelocity( Vector2( 0.f, 0.f ) );

	SetOrientationDegrees( GetRandomFloatInRange( 0.0f, 360.0f ) );
	SetAngularVelocityDegreesPerSecond( 0.f );
	SetPhysicsRadius( 0.4f );
	SetCosmeticRadius( 0.4f );

	SetBoundingBoxMins( Vector2( -0.4f, -0.4f ) );
	SetBoundingBoxMaxs( Vector2( 0.4f, 0.4f ) );

	SetTexture( Texture::CreateOrGetTexture( ENEMY_TANK_TEXTURE_FILE ) ); //  // #Eiserloh: Should be a static function; See comments on this function

	SetTint( Rgba( 255, 255, 255, 255 ) );

	SetHealth( ENEMY_TANK_STARTING_HEALTH );

	m_type = ENTITY_TYPE_ENEMY_TANK;
}


//-----------------------------------------------------------------------------------------------
void EnemyTank::Update( float deltaSeconds )
{
	if ( GetIsDead() )
		return;

	if ( GetHealth() <= 0 )
	{
		SetIsDead( true );
		return;
	}

	float fractionCurrentHealthOverStartingHealth = GetHealth() / (float ) ENEMY_TANK_STARTING_HEALTH;

	SetTint( Rgba( 255, ( unsigned char ) ( fractionCurrentHealthOverStartingHealth * 255.0f ), 
		( unsigned char ) ( fractionCurrentHealthOverStartingHealth * 255.0f ), 255 ) );

	if ( !( g_theMap->GetPlayerTank()->GetIsDead() ) && !( g_theMap->GetPlayerTank()->m_isCloaked ) ) // ensure player tank is not dead or cloaked
	{
		WorldCoords enemyTankPosition = GetPosition();
		WorldCoords playerTankPosition = g_theMap->GetPlayerTank()->GetPosition();

		if ( CalculateDistanceBetweenPoints( enemyTankPosition, playerTankPosition ) <= ENEMY_TANK_DETECTION_RADIUS ) // ensure player tank is within range
		{
			if ( g_theMap->HasLineOfSight( enemyTankPosition, playerTankPosition ) ) // ensure enemy tank can "see" player tank
			{
				Vector2 vectorToPlayerTank = playerTankPosition - enemyTankPosition;

				m_goalOrientationDegrees = ConvertRadiansToDegrees( atan2( vectorToPlayerTank.y, vectorToPlayerTank.x ) );

				float shortestSignedAngularDistance = ComputeShortestSignedAngularDistance( GetOrientationDegrees(), m_goalOrientationDegrees );

				if ( ( abs( shortestSignedAngularDistance ) < ENEMY_TANK_HALF_OF_DETECTION_CONE_IN_DEGREES) && ( deltaSeconds != 0.0f) )
					ShootAtPlayerTank();

				if ( abs( shortestSignedAngularDistance ) < ENEMY_TANK_HALF_OF_PURSUIT_CONE_IN_DEGREES )
				{
					float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
					Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

					SetVelocity( forwardDirection * ENEMY_TANK_SPEED );
				}
				else
				{
					SetVelocity( Vector2( 0.f, 0.f ) );
				}
			}
			else
			{
				float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
				Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

				SetVelocity( forwardDirection * ENEMY_TANK_SPEED );

				if ( !m_waitToPickNewGoalOrientation )
				{
					m_goalOrientationDegrees = GetRandomFloatInRange( 0.0f, 360.0f );
					m_timeLastGoalOrientationChange = ( float ) GetCurrentTimeSeconds();
					m_waitToPickNewGoalOrientation = true;
				}

				if ( ( ( float ) GetCurrentTimeSeconds() > ( m_timeLastGoalOrientationChange + m_timeBetweenGoalOrientationChanges ) ) && m_waitToPickNewGoalOrientation )
				{
					m_waitToPickNewGoalOrientation = false;
				}
			}
		}
		else
		{
			float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
			Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

			SetVelocity( forwardDirection * ENEMY_TANK_SPEED );

			if ( !m_waitToPickNewGoalOrientation )
			{
				m_goalOrientationDegrees = GetRandomFloatInRange( 0.0f, 360.0f );
				m_timeLastGoalOrientationChange = ( float ) GetCurrentTimeSeconds();
				m_waitToPickNewGoalOrientation = true;
			}

			if ( ( ( float ) GetCurrentTimeSeconds() > ( m_timeLastGoalOrientationChange + m_timeBetweenGoalOrientationChanges ) ) && m_waitToPickNewGoalOrientation )
			{
				m_waitToPickNewGoalOrientation = false;
			}
		}
	}
	else
	{
		float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
		Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

		SetVelocity( forwardDirection * ENEMY_TANK_SPEED );

		if ( !m_waitToPickNewGoalOrientation )
		{
			m_goalOrientationDegrees = GetRandomFloatInRange( 0.0f, 360.0f );
			m_timeLastGoalOrientationChange = ( float ) GetCurrentTimeSeconds();
			m_waitToPickNewGoalOrientation = true;
		}

		if ( ( ( float ) GetCurrentTimeSeconds() > ( m_timeLastGoalOrientationChange + m_timeBetweenGoalOrientationChanges ) ) && m_waitToPickNewGoalOrientation )
		{
			m_waitToPickNewGoalOrientation = false;
		}
	}

	Turn( deltaSeconds );

	Entity::Update( deltaSeconds );
}



//-----------------------------------------------------------------------------------------------
void EnemyTank::Turn( float deltaSeconds )
{
	float degreesToTurnThisFrame = ENEMY_TANK_DEGREES_TO_TURN_PER_SECOND * deltaSeconds;

	float shortestSignedAngularDistance = ComputeShortestSignedAngularDistance( GetOrientationDegrees(), m_goalOrientationDegrees );
	float turnDirection = 0.0f;

	const float angularDistanceThreshold =	1.0f;
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
void EnemyTank::Render() const
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
bool EnemyTank::UsesCorrectivePhysics() const
{
	return true;
}


//-----------------------------------------------------------------------------------------------
void EnemyTank::ShootAtPlayerTank()
{
	if ( !m_waitToSpawnNewEnemyTankBullet && !g_theGame->m_disableFiring )
	{
		g_theMap->SpawnEnemyTankBullet( this );
		m_waitToSpawnNewEnemyTankBullet = true;
		m_timeLastEnemyTankBulletFired = ( float ) GetCurrentTimeSeconds();
	}

	float currentTime = ( float ) GetCurrentTimeSeconds();
	float timeNextBulletCanBeFired = m_timeLastEnemyTankBulletFired + m_timeBetweenEnemyTankBullets;
	if ( m_waitToSpawnNewEnemyTankBullet && ( currentTime > timeNextBulletCanBeFired ) )
	{
		m_waitToSpawnNewEnemyTankBullet = false;
	}
}