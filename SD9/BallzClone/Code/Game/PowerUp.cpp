#include "Engine/Renderer/Renderer.hpp"

#include "Game/PowerUp.hpp"


//-----------------------------------------------------------------------------------------------
PowerUp::PowerUp( const IntVector2& gridPosition )
	: m_gridPosition( gridPosition )
	, m_radius( 15.0f )
{
	m_worldPosition = Vector2( gridPosition.x * 100.0f + 50.0f, gridPosition.y * 100.0f + 50.0f );
}


//-----------------------------------------------------------------------------------------------
void PowerUp::Update( float deltaSeconds )
{

}


//-----------------------------------------------------------------------------------------------
void PowerUp::Render() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	g_theRenderer->DrawFilledPolygon( 20, m_worldPosition.x, m_worldPosition.y, m_radius - 5.0f, Rgba::WHITE );
	g_theRenderer->DrawHollowPolygon( 20, m_worldPosition.x, m_worldPosition.y, m_radius, Rgba::WHITE );
}