#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Entities/EnemyBullet.hpp"


//-----------------------------------------------------------------------------------------------
// Constants
const float ENEMY_BULLET_SPEED = 5.0f;
const char* ENEMY_BULLET_TEXTURE_FILE = "Data/Images/shot.png";
const int ENEMY_BULLET_STARTING_HEALTH = 1;


//-----------------------------------------------------------------------------------------------
EnemyBullet::EnemyBullet( const WorldCoords& spawnPosition, float spawnOrientationDegrees,
	float spawnCosmeticRadius )
{
	SetPosition( spawnPosition );

	SetOrientationDegrees( spawnOrientationDegrees );

	// Which way is forward?
	float orientationRadians = ConvertDegreesToRadians( GetOrientationDegrees() );
	Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

	// Move bullet to tip of cosmetic radius
	SetPosition( GetPosition() + ( forwardDirection * spawnCosmeticRadius ) );

	SetVelocity( forwardDirection * ENEMY_BULLET_SPEED );

	SetAngularVelocityDegreesPerSecond( 0.f );
	SetPhysicsRadius( 0.1f );
	SetCosmeticRadius( 0.2f );

	SetBoundingBoxMins( Vector2( -0.4f, -0.4f ) );
	SetBoundingBoxMaxs( Vector2( 0.4f, 0.4f ) );

	SetTexture( Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" ) );

	SetTint( Rgba::RED );

	SetHealth( ENEMY_BULLET_STARTING_HEALTH );

	m_type = ENTITY_TYPE_ENEMY_BULLET;
}


//-----------------------------------------------------------------------------------------------
EnemyBullet::~EnemyBullet()
{
}


//-----------------------------------------------------------------------------------------------
void EnemyBullet::Update( float deltaSeconds )
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

	Entity::Update( deltaSeconds );

	if ( m_ageInSeconds >= 5.0f )
	{
		SetIsDead( true );
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void EnemyBullet::Render() const
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
	g_theRenderer->DrawLine3D( Vector3( entityPositionAsVector3 ), Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3
		* GetPhysicsRadius() ), Rgba::WHITE, 2.0f );
}