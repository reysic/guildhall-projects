#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/MakeNormalMap.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
{
	Vector3 lightVector( 1.0f, -1.0f, -1.0f );

	m_litTextureID = MakeNormalMap::GetTextureIDOfNormalMappedTexture( lightVector );

	m_surfaceColorMapTexture = Texture::CreateOrGetTexture( SURFACE_COLOR_MAP_TEXTURE_FILE );
	m_normalDisplacementMapTexture = Texture::CreateOrGetTexture( NORMAL_DISPLACEMENT_MAP_TEXTURE_FILE );
}


//-----------------------------------------------------------------------------------------------
void TheGame::Input()
{

}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	deltaSeconds = 0;
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	Rgba tint( 1.0f, 1.0f, 1.0f, 1.0f );
	
	g_theRenderer->DrawTexturedAABB( AABB2( Vector2( 800.0f - 512.0f, 0.0f ), Vector2( 800.0f, 512.0f ) ), 
										m_surfaceColorMapTexture, Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), tint );

	g_theRenderer->DrawTexturedAABB( AABB2( Vector2( 800.0f, 0.0f ), Vector2( 800.0f + 512.0f, 512.0f ) ), 
										m_normalDisplacementMapTexture, Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), tint );

	g_theRenderer->DrawTexturedAABBByTextureID( AABB2( Vector2( 800.0f - 256.0f, 512.0f ), Vector2( 800.0f + 256.0f, 1024.0f ) ), 
										m_litTextureID, Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), tint );
}