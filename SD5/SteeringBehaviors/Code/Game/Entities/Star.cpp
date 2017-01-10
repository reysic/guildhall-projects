#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Entities/Star.hpp"


//-----------------------------------------------------------------------------------------------
// Constants
const float STAR_MAX_DOWNWARD_SPEED = 5.0f;
const char* STAR_TEXTURE_FILE = "Data/Images/star.png";
const int STAR_STARTING_HEALTH = 1;


//-----------------------------------------------------------------------------------------------
Star::Star( const WorldCoords& spawnPosition )
{
	SetPosition( spawnPosition );
	SetVelocity( Vector2( 0.0f, -1.0f ) * GetRandomFloatInRange( 1.0f, STAR_MAX_DOWNWARD_SPEED ) );

	SetOrientationDegrees( 0.0f );
	SetAngularVelocityDegreesPerSecond( GetRandomFloatInRange( 0.0f, 90.0f ) );
	SetPhysicsRadius( 0.1f );
	SetCosmeticRadius( 0.2f );

	SetBoundingBoxMins( Vector2( -0.05f, -0.05f ) );
	SetBoundingBoxMaxs( Vector2( 0.05f, 0.05f ) );

	SetTexture( Texture::CreateOrGetTexture( STAR_TEXTURE_FILE ) );

	SetTint( Rgba::WHITE );

	SetHealth( STAR_STARTING_HEALTH );

	m_type = ENTITY_TYPE_STAR;
}


//-----------------------------------------------------------------------------------------------
Star::~Star()
{
}


//-----------------------------------------------------------------------------------------------
void Star::Update( float deltaSeconds )
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

	if ( m_position.y < -1.0f )
	{
		SetIsDead( true );
		return;
	}
}


//-----------------------------------------------------------------------------------------------
void Star::Render() const
{
	if ( GetIsDead() )
	{
		return;
	}

	// Non-debug rendering
	g_theRenderer->PushView();
	Vector3 positionAsVector3( GetPosition().x, GetPosition().y, 0.0f );
	g_theRenderer->TranslateView( positionAsVector3 );
	g_theRenderer->RotateView( GetOrientationDegrees(), Vector3( 0.0f, 0.0f, 1.0f ) );
	g_theRenderer->DrawTexturedAABB2( GetBoundingBox(), *GetTexture(), Vector2( 0.f, 0.f ), Vector2( 1.f, 1.f ), GetTint() );
	g_theRenderer->PopView();
}