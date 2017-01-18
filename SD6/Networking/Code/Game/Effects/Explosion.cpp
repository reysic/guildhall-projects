#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Sprites/SpriteSheet.hpp"
#include "Engine/Renderer/Sprites/SpriteAnimation.hpp"
#include "Engine/Core/Time.hpp"
#include "Game/Effects/Explosion.hpp"


//-----------------------------------------------------------------------------------------------
const char* EXPLOSION_SPRITE_SHEET_FILE = "Data/Images/Explosion_5x5.png";


//-----------------------------------------------------------------------------------------------
Explosion::Explosion( Vector2 explosionLocation, float sizeScalingFactor, float durationInSeconds )
{
	m_position = explosionLocation;

	m_sizeScalingFactor = sizeScalingFactor;

	m_duration = durationInSeconds;

	m_ageInSeconds = 0.0f;

	Vector2 boundingBoxMins( -0.5f, -0.5f );
	Vector2 boundingBoxMaxs( 0.5f, 0.5f );

	boundingBoxMins *= m_sizeScalingFactor;
	boundingBoxMaxs *= m_sizeScalingFactor;

	m_bounds = AABB2( boundingBoxMins, boundingBoxMaxs );

	m_explosionSpriteSheet = new SpriteSheet( EXPLOSION_SPRITE_SHEET_FILE, 5, 5 );

	m_explosionSpriteAnimation = new SpriteAnimation( m_explosionSpriteSheet, m_duration, SPRITE_ANIM_MODE_PLAY_TO_END, 0, 23 );

	m_isFinished = false;
}


//-----------------------------------------------------------------------------------------------
Explosion::~Explosion()
{
	delete m_explosionSpriteAnimation;
	m_explosionSpriteAnimation = nullptr;
	delete m_explosionSpriteSheet;
	m_explosionSpriteSheet = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Explosion::Update( float deltaSeconds )
{
	if ( m_isFinished )
		return;

	if ( m_ageInSeconds > m_duration )
	{
		m_isFinished = true;
		return;
	}

	m_ageInSeconds += deltaSeconds;

	m_explosionSpriteAnimation->Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Explosion::Render() const
{
	if ( m_isFinished )
		return;

	g_theRenderer->PushView();

	Vector3 positionAsVector3( m_position.x, m_position.y, 0.0f );
	g_theRenderer->TranslateView( positionAsVector3 );

	Vector2 texCoordsMins = m_explosionSpriteAnimation->GetCurrentTexCoords().mins;
	Vector2 texCoordsMaxs = m_explosionSpriteAnimation->GetCurrentTexCoords().maxs;
	Rgba tint( 255, 255, 255, 255 );

	g_theRenderer->EnableGLAdditiveBlending();
	g_theRenderer->DrawTexturedAABB2( m_bounds, *Texture::CreateOrGetTexture( EXPLOSION_SPRITE_SHEET_FILE ), texCoordsMins, texCoordsMaxs, tint );
	g_theRenderer->EnableGLAlphaBlending();

	g_theRenderer->PopView();
}