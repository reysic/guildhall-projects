#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Entities/Entity.hpp"
#include "Game/Entities/Enemy.hpp"
#include "Game/Environment/Map.hpp"
#include "Game/AI/SteeringManager.hpp"


//-----------------------------------------------------------------------------------------------
// Constants
const float ENEMY_MAX_SPEED = 2.0f;
const float PURSUE_ENEMY_MAX_SPEED = 1.0f;
const float ENEMY_MAX_ACCELERATION = 0.1f;
const float PURSUE_ENEMY_MAX_ACCELERATION = 0.05f;
const float ENEMY_DEGREES_TO_TURN_PER_SECOND = 360.0f;
const float ENEMY_FIRING_RATE_SHOTS_PER_SECOND = 1.0f;
const int ENEMY_STARTING_HEALTH = 2;
const int PATH_ENEMY_STARTING_HEALTH = 6;


//-----------------------------------------------------------------------------------------------
// Globals
float g_wanderOrientation = 0.0f; // Holds the current orientation of the wander target
WorldCoords g_wanderCircleCenter = Vector2( 0.0f, 0.0f );
WorldCoords g_wanderTargetPosition = Vector2( 0.0f, 0.0f );
unsigned int g_currentPathNodeIndex = 0;
WorldCoords g_pursueTargetPosition = Vector2( 0.0f, 0.0f );


//-----------------------------------------------------------------------------------------------
Enemy::Enemy( const WorldCoords& enemyCarPosition, EntityType type )
	: m_satisfactionRadius( 2.0f )
	, m_slowRadius( 5.0f )
	, m_waitToShoot( false )
	, m_timeBetweenShots( 1.0f / ENEMY_FIRING_RATE_SHOTS_PER_SECOND )
	, m_timeLastShotFired( 0.0f )
	, m_steeringManager( new SteeringManager( this ) )
{
	SetPosition( enemyCarPosition );
	SetVelocity( Vector2( 0.f, 0.f ) );

	SetOrientationDegrees( GetRandomFloatInRange( 0.0f, 360.0f ) );
	SetAngularVelocityDegreesPerSecond( 0.f );
	SetCosmeticRadius( 0.6f );

	if ( type == ENTITY_TYPE_SEEK_ENEMY )
	{
		SetPhysicsRadius( 0.4f );
		SetBoundingBoxMins( Vector2( -0.75f, -0.75f ) );
		SetBoundingBoxMaxs( Vector2( 0.75f, 0.75f ) );
		SetTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" ) );
		SetHealth( ENEMY_STARTING_HEALTH );
		SetTint( Rgba::ORANGE );
		m_type = ENTITY_TYPE_SEEK_ENEMY;
		m_steeringBehavior = STEERING_BEHAVIOR_SEEK;
	}
	else if ( type == ENTITY_TYPE_PATH_ENEMY )
	{
		SetPhysicsRadius( 0.4f );
		SetBoundingBoxMins( Vector2( -0.75f, -0.75f ) );
		SetBoundingBoxMaxs( Vector2( 0.75f, 0.75f ) );
		SetTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" ) );
		SetHealth( PATH_ENEMY_STARTING_HEALTH );
		SetTint( Rgba::LIGHTOLIVEGREEN );
		m_type = ENTITY_TYPE_PATH_ENEMY;
		m_steeringBehavior = STEERING_BEHAVIOR_PATH_FOLLOW;
	}
	else if ( type == ENTITY_TYPE_WANDER_ENEMY )
	{
		SetPhysicsRadius( 0.4f );
		SetBoundingBoxMins( Vector2( -0.75f, -0.75f ) );
		SetBoundingBoxMaxs( Vector2( 0.75f, 0.75f ) );
		SetTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" ) );
		SetHealth( ENEMY_STARTING_HEALTH );
		SetTint( Rgba::PINK );
		m_type = ENTITY_TYPE_WANDER_ENEMY;
		m_steeringBehavior = STEERING_BEHAVIOR_WANDER;
	}
	else if ( type == ENTITY_TYPE_PURSUE_ENEMY )
	{
		SetPhysicsRadius( 0.4f );
		SetBoundingBoxMins( Vector2( -0.75f, -0.75f ) );
		SetBoundingBoxMaxs( Vector2( 0.75f, 0.75f ) );
		SetTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" ) );
		SetHealth( ENEMY_STARTING_HEALTH );
		SetTint( Rgba::RED );
		m_type = ENTITY_TYPE_PURSUE_ENEMY;
		m_steeringBehavior = STEERING_BEHAVIOR_PURSUE;
	}
	else
	{
		ASSERT_OR_DIE( false, "Unknown enemy type!" );
	}
}


//-----------------------------------------------------------------------------------------------
Enemy::~Enemy()
{
	delete m_steeringManager;
	m_steeringManager = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Enemy::Update( float deltaSeconds )
{
	if ( GetIsDead() )
	{
		return;
	}

	if ( GetHealth() <= 0 )
	{
		SetIsDead( true );
		return;
	}

	//UpdateTintBasedOnRemainingHealth();

	// Don't execute further logic if the player is dead
	if ( g_theMap->GetPlayer()->GetIsDead() )
	{
		return;
	}

	UpdateSteering( deltaSeconds );

	////////////////////////////// Toggle Additional Behaviors Here ///////////////////////////////
	ApplySeparation( deltaSeconds );

	//ApplyCollisionAvoidance( deltaSeconds );

	ApplyObstacleAvoidance( deltaSeconds );
	////////////////////////////// Toggle Additional Behaviors Here ///////////////////////////////

	// Clamp to maximum speed if exceeding
	Vector2 myVelocity = GetVelocity();
	if ( m_type == ENTITY_TYPE_PURSUE_ENEMY )
	{
		if ( myVelocity.Length() > PURSUE_ENEMY_MAX_SPEED )
		{
			myVelocity.Normalize();
			SetVelocity( myVelocity * PURSUE_ENEMY_MAX_SPEED );
		}
	}
	else if ( myVelocity.Length() > ENEMY_MAX_SPEED )
	{
		myVelocity.Normalize();
		SetVelocity( myVelocity * ENEMY_MAX_SPEED );
	}

	Shoot();

	// Call parent class Update()
	Entity::Update( deltaSeconds );
}



//-----------------------------------------------------------------------------------------------
void Enemy::UpdateTintBasedOnRemainingHealth()
{
	float fractionCurrentHealthOverStartingHealth;

	if ( m_type == ENTITY_TYPE_SEEK_ENEMY )
	{
		fractionCurrentHealthOverStartingHealth = GetHealth() / ( float ) ENEMY_STARTING_HEALTH;
	}
	else if ( m_type == ENTITY_TYPE_PATH_ENEMY )
	{
		fractionCurrentHealthOverStartingHealth = GetHealth() / ( float ) PATH_ENEMY_STARTING_HEALTH;
	}
	else
	{
		ASSERT_OR_DIE( false, "Unknown enemy type!" );
	}

	SetTint( Rgba( 255, ( unsigned char ) ( fractionCurrentHealthOverStartingHealth * 255.0f ),
		( unsigned char ) ( fractionCurrentHealthOverStartingHealth * 255.0f ), 255 ) );
}


//-----------------------------------------------------------------------------------------------
void Enemy::UpdateSteering( float deltaSeconds )
{
	// Behave according to active steering behavior
	switch ( m_steeringBehavior )
	{
		case STEERING_BEHAVIOR_NONE:
		{
			break;
		}
		case STEERING_BEHAVIOR_WANDER:
		{
			Wander( deltaSeconds );
			break;
		}
		case STEERING_BEHAVIOR_SEEK:
		{
			Seek( deltaSeconds );
			break;
		}
		case STEERING_BEHAVIOR_PURSUE:
		{
			Pursue( deltaSeconds );
			break;
		}
		case STEERING_BEHAVIOR_PATH_FOLLOW:
		{
			PathFollow( deltaSeconds );
			break;
		}
		default:
		{
			ASSERT_OR_DIE( false, "Unknown steering behavior" );
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Enemy::Render() const
{
	if ( GetIsDead() )
	{
		return;
	}

	// Non-debug rendering
// 	g_theRenderer->PushView();
// 	Vector3 positionAsVector3( GetPosition().x, GetPosition().y, 0.0f );
// 	g_theRenderer->TranslateView( positionAsVector3 );
// 	g_theRenderer->RotateView( GetOrientationDegrees(), Vector3( 0.0f, 0.0f, 1.0f ) );
// 	g_theRenderer->DrawTexturedAABB2( GetBoundingBox(), *GetTexture(), Vector2( 0.f, 0.f ), Vector2( 1.f, 1.f ), GetTint() );
// 	g_theRenderer->PopView();

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
	g_theRenderer->DrawLine3D( entityPositionAsVector3, Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3
		* GetPhysicsRadius() ), Rgba::WHITE, 2.0f );


	if ( m_type == ENTITY_TYPE_SEEK_ENEMY )
	{
		// Draw collision detection ray(s)

		// Primary detection ray
		WorldCoords startOfRay( GetPosition().x, GetPosition().y );
		orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
		entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
		normalizedEntityForwardDirection = entityForwardDirection.Normalize();
		WorldCoords endOfRay = startOfRay + entityForwardDirection * GetCosmeticRadius() * 2.0f;
		Vector3 startOfRayAsVector3( startOfRay.x, startOfRay.y, 0.0f );
		Vector3 endOfRayAsVector3( endOfRay.x, endOfRay.y, 0.0f );
		g_theRenderer->DrawLine3D( startOfRayAsVector3, endOfRayAsVector3, Rgba::RED, 2.0f );
		g_theRenderer->DrawFilledPolygon( 20, endOfRay.x, endOfRay.y,
			0.1f, Rgba::WHITE );

		// Secondary detection ray
		startOfRay = WorldCoords( GetPosition().x, GetPosition().y );
		orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() + 30.0f );
		entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
		normalizedEntityForwardDirection = entityForwardDirection.Normalize();
		endOfRay = startOfRay + entityForwardDirection * GetCosmeticRadius() * 1.25f;
		startOfRayAsVector3 = Vector3( startOfRay.x, startOfRay.y, 0.0f );
		endOfRayAsVector3 = Vector3( endOfRay.x, endOfRay.y, 0.0f );
		g_theRenderer->DrawLine3D( startOfRayAsVector3, endOfRayAsVector3, Rgba::RED, 2.0f );
		g_theRenderer->DrawFilledPolygon( 20, endOfRay.x, endOfRay.y,
			0.1f, Rgba::WHITE );

		startOfRay = WorldCoords( GetPosition().x, GetPosition().y );
		orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() - 30.0f );
		entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
		normalizedEntityForwardDirection = entityForwardDirection.Normalize();
		endOfRay = startOfRay + entityForwardDirection * GetCosmeticRadius() * 1.25f;
		startOfRayAsVector3 = Vector3( startOfRay.x, startOfRay.y, 0.0f );
		endOfRayAsVector3 = Vector3( endOfRay.x, endOfRay.y, 0.0f );
		g_theRenderer->DrawLine3D( startOfRayAsVector3, endOfRayAsVector3, Rgba::RED, 2.0f );
		g_theRenderer->DrawFilledPolygon( 20, endOfRay.x, endOfRay.y,
			0.1f, Rgba::WHITE );
	}

	if ( m_type == ENTITY_TYPE_WANDER_ENEMY )
	{
		g_theRenderer->DrawHollowPolygon( 20, g_wanderCircleCenter.x, g_wanderCircleCenter.y,
			0.5f, Rgba::WHITE );
		g_theRenderer->DrawFilledPolygon( 20, g_wanderTargetPosition.x, g_wanderTargetPosition.y,
			0.1f, Rgba::RED );
	}

	if ( m_type == ENTITY_TYPE_PURSUE_ENEMY )
	{
		g_theRenderer->DrawFilledPolygon( 20, g_pursueTargetPosition.x, g_pursueTargetPosition.y,
			0.1f, Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
void Enemy::Wander( float deltaSeconds )
{
	// Holds the radius and forward offset of the wander circle
	const float WANDER_OFFSET = 2.0f;
	const float WANDER_RADIUS = 0.5f;

	// Holds the maximum rate at which wander orientation can change
	const float WANDER_RATE = 12.0f;

// 	// Holds the current orientation of the wander target
// 	float wanderOrientation = 0.0f;

	// 1. Calculate the target to delegate to face
	// Update wander orientation
	g_wanderOrientation += GetRandomBinomial() * WANDER_RATE;
	// Calculate the combined target orientation
	float targetOrientation = g_wanderOrientation + GetOrientationDegrees();
	// Calculate the center of the wander circle
	float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
	Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );
	forwardDirection.Normalize();
	WorldCoords targetPosition = GetPosition() + ( forwardDirection * WANDER_OFFSET );
	g_wanderCircleCenter = targetPosition;
	// Calculate the target location
	float targetOrientationRadians = ConvertDegreesToRadians( targetOrientation );
	Vector2 targetDirection( cos( targetOrientationRadians ), sin( targetOrientationRadians ) );
	targetPosition += targetDirection * WANDER_RADIUS;
	g_wanderTargetPosition = targetPosition;

	// 2. Delegate to face
	// Goal orientation is facing in direction of target
	WorldCoords myPosition = GetPosition();
	Vector2 directionToTarget = targetPosition - myPosition;
	directionToTarget.Normalize();
	m_goalOrientationDegrees = GetVectorAngleDegrees( directionToTarget );

	// How many degrees to turn?
	float degreesToTurn = ComputeShortestSignedAngularDistance( GetOrientationDegrees(), m_goalOrientationDegrees );

	// Which direction to turn?
	float turnDirection = 0.0f;
	const float angularDistanceThreshold = 1.0f;
	if ( degreesToTurn < -angularDistanceThreshold )
	{
		turnDirection += -1.0f;
	}
	else if ( degreesToTurn > angularDistanceThreshold )
	{
		turnDirection += 1.0f;
	}

	// Turn
	float degreesToTurnThisFrame;
	if ( abs( degreesToTurn ) > 90.0f )
	{
		// Rotate at max speed
		degreesToTurnThisFrame = ENEMY_DEGREES_TO_TURN_PER_SECOND * deltaSeconds;
		degreesToTurnThisFrame *= turnDirection;
	}
	else if ( abs( degreesToTurn ) > 1.0f )
	{
		// Calculate a scaled rotation
		float targetRotation = ENEMY_DEGREES_TO_TURN_PER_SECOND * abs( degreesToTurn ) / 90.0f;
		degreesToTurnThisFrame = targetRotation * deltaSeconds;
		degreesToTurnThisFrame *= turnDirection;
	}
	else
	{
		// Don't turn
		degreesToTurnThisFrame = 0.0f;
	}
	SetOrientationDegrees( GetOrientationDegrees() + degreesToTurnThisFrame );

	// 3. Now set the linear acceleration to be at full acceleration in the direction of
	// the orientation
	float curOrientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
	Vector2 curForwardDirection( cos( curOrientationRadians ), sin( curOrientationRadians ) );
	SetVelocity( curForwardDirection * ENEMY_MAX_SPEED );
}


//-----------------------------------------------------------------------------------------------
void Enemy::Seek( float deltaSeconds )
{
	Vector2 playerPosition = g_theMap->GetPlayer()->GetPosition();
	m_steeringManager->Seek( playerPosition, ENEMY_MAX_ACCELERATION );
	FaceTarget( deltaSeconds, playerPosition );
}


//-----------------------------------------------------------------------------------------------
void Enemy::Arrive( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	WorldCoords enemyCarPosition = GetPosition();
	WorldCoords playerCarPosition = g_theMap->GetPlayer()->GetPosition();
	Vector2 vectorToPlayerCar = playerCarPosition - enemyCarPosition;
	if ( vectorToPlayerCar.Length() < m_satisfactionRadius )
	{
		// We're already within the arrival radius, so stop moving
		SetVelocity( Vector2( 0.0f, 0.0f ) );
	}
	else if ( vectorToPlayerCar.Length() > m_slowRadius )
	{
		// Outside of slow radius, so go max speed
		vectorToPlayerCar.Normalize();
		SetVelocity( vectorToPlayerCar * ENEMY_MAX_SPEED );
	}
	else
	{
		// Calculate a scaled speed
		float targetSpeed = ENEMY_MAX_SPEED * vectorToPlayerCar.Length() / m_slowRadius;
		Vector2 targetVelocity = vectorToPlayerCar.Normalize() * targetSpeed;

		SetVelocity( targetVelocity );
	}
	if ( GetVelocity().Length() > 0.0f )
	{
		// We have a velocity, so let's face in that direction
		SetOrientationDegrees( ConvertRadiansToDegrees( atan2( vectorToPlayerCar.y, vectorToPlayerCar.x ) ) );
	}
}


//-----------------------------------------------------------------------------------------------
void Enemy::ApplySeparation( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	const float SEPARATION_THRESHOLD = GetCosmeticRadius() * 1.5f;

	for ( Entity* otherEntity : g_theMap->m_entities )
	{
		if ( otherEntity )
		{
			if ( otherEntity->m_type == m_type ) // Is entity the same type as me?
			{
				if ( otherEntity != this ) // Is entity not me?
				{
					// Get direction and distance to other entity
					WorldCoords myPosition = GetPosition();
					WorldCoords otherPosition = otherEntity->GetPosition();
					Vector2 directionToOther = otherPosition - myPosition;
					float distanceToOther = directionToOther.Length();

					// Is other entity within separation threshold?
					if ( distanceToOther < SEPARATION_THRESHOLD )
					{
						// Calculate the strength of repulsion
						float strength = ENEMY_MAX_SPEED * ( SEPARATION_THRESHOLD - distanceToOther )
							/ SEPARATION_THRESHOLD;

						directionToOther.Normalize();
						directionToOther.Negate();

						Vector2 myCurrentVelocity = GetVelocity();
						Vector2 myNewVelocity = myCurrentVelocity + ( directionToOther * strength );

						SetVelocity( myNewVelocity );
					}
				}
			}
		}
	}
}



//-----------------------------------------------------------------------------------------------
void Enemy::ApplyCollisionAvoidance( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	const float COLLISION_RADIUS = 2.0f; // Try this being equal to the physics/cosmetic radii

	// Store the first collision time
	float shortestTimeToCollision = 1000000.0f; // Set to a really large value initially

	Vector2 relativePosition;
	Vector2 relativeVelocity;

	// Store the target that collides then, and other data
	// that we will need and can avoid recalculating
	Entity* firstTarget = nullptr;
	float firstMinimumSeparation = 0.0f;
	float firstDistance = 0.0f;
	Vector2 firstRelativePosition;
	Vector2 firstRelativeVelocity;

	// Loop through other entities and calculate the time to collision
	for ( Entity* otherEntity : g_theMap->m_entities )
	{
		if ( otherEntity )
		{
			if ( otherEntity->m_type == m_type ) // Is entity the same type as me?
			{
				if ( otherEntity != this ) // Is entity not me?
				{
					WorldCoords myPosition = GetPosition();
					WorldCoords otherPosition = otherEntity->GetPosition();
					Vector2 myVelocity = GetVelocity();
					Vector2 otherVelocity = otherEntity->GetVelocity();

					// Calculate relative position, velocity, and speed
					relativePosition = otherPosition - myPosition;
					relativeVelocity = otherVelocity - myVelocity;
					float relativeSpeed = relativeVelocity.Length();

					// Calculate time to collision
					float timeToCollision = DotProductVector2( relativePosition, relativeVelocity ) /
						( relativeSpeed * relativeSpeed );

					// Check if there is going to be a collision at all
					float distance = relativePosition.Length();
					float minimumSeparation = distance - relativeSpeed * timeToCollision;
					if ( minimumSeparation > 2.0f * COLLISION_RADIUS )
					{
						continue;
					}

					// Check if it is the shortest
					if ( timeToCollision > 0.0f && timeToCollision < shortestTimeToCollision )
					{
						shortestTimeToCollision = timeToCollision;
						firstTarget = otherEntity;
						firstMinimumSeparation = minimumSeparation;
						firstDistance = distance;
						firstRelativePosition = relativePosition;
						firstRelativeVelocity = relativeVelocity;
					}
				}
			}
		}
	}

	// Calculate the steering

	// If we have no target, return
	if ( firstTarget == nullptr )
	{
		return;
	}

	// If we're going to hit exactly, or if we're already
	// colliding, then do the steering based on current
	// position.
 	if ( firstMinimumSeparation <= 0.0f || firstDistance < ( 2.0f * COLLISION_RADIUS ) )
	{
		relativePosition = firstTarget->GetPosition() - GetPosition();
	}
	// Otherwise calculate future relative position
	else
	{
		relativePosition = firstRelativePosition + firstRelativeVelocity * shortestTimeToCollision;
	}

	// Avoid the target
	relativePosition.Normalize();
	SetVelocity( relativePosition * ENEMY_MAX_ACCELERATION );
}


//-----------------------------------------------------------------------------------------------
void Enemy::ApplyObstacleAvoidance( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	const float AVOID_SCALE_FACTOR = 0.25f;

	// Calculate start and end of ray
	WorldCoords startOfRay( GetPosition().x, GetPosition().y );
	float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
	Vector2 entityForwardDirection( cos( orientationRadians ), sin( orientationRadians ) );
	Vector2 normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	WorldCoords endOfRay = startOfRay + entityForwardDirection * GetCosmeticRadius() * 2.0f;

	// Get TileCoords for start and end of ray
	TileCoords startOfRayTileCoords = g_theMap->GetTileCoordsForWorldCoords( startOfRay );
	TileCoords endOfRayTileCoords = g_theMap->GetTileCoordsForWorldCoords( endOfRay );

	// Get type of tile at end of ray
	TileIndex tileIndex = g_theMap->GetIndexForTileCoords( endOfRayTileCoords );
	if ( g_theMap->m_tiles[ tileIndex ].m_type == TILE_TYPE_STONE )
	{
		WorldCoords stoneTileCenter( g_theMap->m_tiles[ tileIndex ].m_tileCoords.x + 0.5f,
									 g_theMap->m_tiles[ tileIndex ].m_tileCoords.y + 0.5f );

		Vector2 vectorFromStoneTile = endOfRay - stoneTileCenter;
		vectorFromStoneTile.Normalize();

		Vector2 myCurrentVelocity = GetVelocity();
		Vector2 myNewVelocity = myCurrentVelocity + ( vectorFromStoneTile * AVOID_SCALE_FACTOR );

		SetVelocity( myNewVelocity );
		return;
	}

	// Secondary collision rays
	startOfRay = WorldCoords( GetPosition().x, GetPosition().y );
	orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() + 30.0f );
	entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
	normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	endOfRay = startOfRay + entityForwardDirection * GetCosmeticRadius() * 1.25f;

	// Get TileCoords for start and end of ray
	startOfRayTileCoords = g_theMap->GetTileCoordsForWorldCoords( startOfRay );
	endOfRayTileCoords = g_theMap->GetTileCoordsForWorldCoords( endOfRay );

	// Get type of tile at end of ray
	tileIndex = g_theMap->GetIndexForTileCoords( endOfRayTileCoords );
	if ( g_theMap->m_tiles[ tileIndex ].m_type == TILE_TYPE_STONE )
	{
		WorldCoords stoneTileCenter( g_theMap->m_tiles[ tileIndex ].m_tileCoords.x + 0.5f,
			g_theMap->m_tiles[ tileIndex ].m_tileCoords.y + 0.5f );

		Vector2 vectorFromStoneTile = endOfRay - stoneTileCenter;
		vectorFromStoneTile.Normalize();

		Vector2 myCurrentVelocity = GetVelocity();
		Vector2 myNewVelocity = myCurrentVelocity + ( vectorFromStoneTile * AVOID_SCALE_FACTOR );

		SetVelocity( myNewVelocity );
		return;
	}

	startOfRay = WorldCoords( GetPosition().x, GetPosition().y );
	orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() - 30.0f );
	entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
	normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	endOfRay = startOfRay + entityForwardDirection * GetCosmeticRadius() * 1.25f;

	// Get TileCoords for start and end of ray
	startOfRayTileCoords = g_theMap->GetTileCoordsForWorldCoords( startOfRay );
	endOfRayTileCoords = g_theMap->GetTileCoordsForWorldCoords( endOfRay );

	// Get type of tile at end of ray
	tileIndex = g_theMap->GetIndexForTileCoords( endOfRayTileCoords );
	if ( g_theMap->m_tiles[ tileIndex ].m_type == TILE_TYPE_STONE )
	{
		WorldCoords stoneTileCenter( g_theMap->m_tiles[ tileIndex ].m_tileCoords.x + 0.5f,
			g_theMap->m_tiles[ tileIndex ].m_tileCoords.y + 0.5f );

		Vector2 vectorFromStoneTile = endOfRay - stoneTileCenter;
		vectorFromStoneTile.Normalize();

		Vector2 myCurrentVelocity = GetVelocity();
		Vector2 myNewVelocity = myCurrentVelocity + ( vectorFromStoneTile * AVOID_SCALE_FACTOR );

		SetVelocity( myNewVelocity );
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void Enemy::Pursue( float deltaSeconds )
{
	const float MAX_PREDICTION_TIME_SEC = 0.25f;

	// Get direction and distance to target
	WorldCoords myPosition = GetPosition();
	WorldCoords playerPosition = g_theMap->GetPlayer()->GetPosition();
	Vector2 directionToPlayer = playerPosition - myPosition;
	float distanceToPlayer = directionToPlayer.Length();

	// Determine our current speed
	float myCurrentSpeed = GetVelocity().Length();

	float predictionTime;
	// Check if current speed is too small to give a reasonable prediction time
	if ( myCurrentSpeed <= ( distanceToPlayer / MAX_PREDICTION_TIME_SEC ) )
	{
		predictionTime = MAX_PREDICTION_TIME_SEC;
	}
	// Otherwise calculate the prediction time
	else
	{
		predictionTime = distanceToPlayer / myCurrentSpeed;
	}

	// Get target's position following predicted movement
	Vector2 currentPlayerVelocity = g_theMap->GetPlayer()->GetVelocity();
	WorldCoords playerPositionPredicted = playerPosition + 
		( currentPlayerVelocity * predictionTime );

	g_pursueTargetPosition = playerPositionPredicted;

	// Delegate to Seek()
	m_steeringManager->Seek( playerPositionPredicted, PURSUE_ENEMY_MAX_ACCELERATION );
	FaceTarget( deltaSeconds, playerPositionPredicted );
}


//-----------------------------------------------------------------------------------------------
void Enemy::FaceTarget( float deltaSeconds, const Vector2& targetPosition )
{
	// Goal orientation is facing in direction of target
	WorldCoords myPosition = GetPosition();
	WorldCoords playerPosition = g_theMap->GetPlayer()->GetPosition();
	Vector2 directionToTarget = targetPosition - myPosition;
	directionToTarget.Normalize();
	m_goalOrientationDegrees = GetVectorAngleDegrees( directionToTarget );

	// How many degrees to turn?
	float degreesToTurn = ComputeShortestSignedAngularDistance( GetOrientationDegrees(), m_goalOrientationDegrees );

	// Which direction to turn?
	float turnDirection = 0.0f;
	const float angularDistanceThreshold = 1.0f;
	if ( degreesToTurn < -angularDistanceThreshold )
	{
		turnDirection += -1.0f;
	}
	else if ( degreesToTurn > angularDistanceThreshold )
	{
		turnDirection += 1.0f;
	}

	// Turn
	float degreesToTurnThisFrame;
	if ( abs( degreesToTurn ) > 90.0f )
	{
		// Rotate at max speed
		degreesToTurnThisFrame = ENEMY_DEGREES_TO_TURN_PER_SECOND * deltaSeconds;
		degreesToTurnThisFrame *= turnDirection;
	}
	else if ( abs( degreesToTurn ) > 1.0f )
	{
		// Calculate a scaled rotation
		float targetRotation = ENEMY_DEGREES_TO_TURN_PER_SECOND * abs( degreesToTurn ) / 90.0f;
		degreesToTurnThisFrame = targetRotation * deltaSeconds;
		degreesToTurnThisFrame *= turnDirection;
	}
	else
	{
		// Don't turn
		degreesToTurnThisFrame = 0.0f;
	}
	SetOrientationDegrees( GetOrientationDegrees() + degreesToTurnThisFrame );
}


//-----------------------------------------------------------------------------------------------
void Enemy::PathFollow( float deltaSeconds )
{
	const float NODE_REACHED_THRESHOLD = 0.5f;

	// Does another path node exist?
	if ( g_currentPathNodeIndex >= g_theMap->m_path->m_nodes.size() )
	{
		g_currentPathNodeIndex = 0;
	}

	// Get distance to current path node
	WorldCoords myPosition = GetPosition();
	WorldCoords nodePosition = g_theMap->m_path->m_nodes[ g_currentPathNodeIndex ];
	Vector2 vectorToNode = nodePosition - myPosition;
	float distanceToNode = vectorToNode.Length();

	// Has the current node been reached?
	if ( distanceToNode < NODE_REACHED_THRESHOLD )
	{
		++g_currentPathNodeIndex;
		return;
	}

	// Delegate to Seek()
	m_steeringManager->Seek( nodePosition, ENEMY_MAX_ACCELERATION );
	FaceTarget( deltaSeconds, nodePosition );
}


//-----------------------------------------------------------------------------------------------
void Enemy::Shoot()
{
	if ( !m_waitToShoot )
	{
		if ( m_type == ENTITY_TYPE_SEEK_ENEMY )
		{
			g_theMap->SpawnEnemyBullet( m_position, m_orientationDegrees, m_cosmeticRadius );
		}
		else if ( m_type == ENTITY_TYPE_PATH_ENEMY )
		{
			g_theMap->SpawnEnemyBullet( m_position, 0.0f, m_cosmeticRadius );
			g_theMap->SpawnEnemyBullet( m_position, 45.0f, m_cosmeticRadius );
			g_theMap->SpawnEnemyBullet( m_position, 90.0f, m_cosmeticRadius );
			g_theMap->SpawnEnemyBullet( m_position, 135.0f, m_cosmeticRadius );
			g_theMap->SpawnEnemyBullet( m_position, 180.0f, m_cosmeticRadius );
			g_theMap->SpawnEnemyBullet( m_position, 225.0f, m_cosmeticRadius );
			g_theMap->SpawnEnemyBullet( m_position, 270.0f, m_cosmeticRadius );
			g_theMap->SpawnEnemyBullet( m_position, 315.0f, m_cosmeticRadius );
		}
		else if ( m_type == ENTITY_TYPE_WANDER_ENEMY )
		{
			g_theMap->SpawnEnemyBullet( m_position, m_orientationDegrees, m_cosmeticRadius );
		}
		else if ( m_type == ENTITY_TYPE_PURSUE_ENEMY )
		{
			g_theMap->SpawnEnemyBullet( m_position, m_orientationDegrees, m_cosmeticRadius );
		}
		else
		{
			ASSERT_OR_DIE( false, "Unknown enemy type!" );
		}
		m_waitToShoot = true;
		m_timeLastShotFired = ( float ) GetCurrentTimeSeconds();
	}

	float currentTime = ( float ) GetCurrentTimeSeconds();
	float timeNextBulletCanBeFired = m_timeLastShotFired + m_timeBetweenShots;
	if ( m_waitToShoot && ( currentTime > timeNextBulletCanBeFired ) )
	{
		m_waitToShoot = false;
	}
}