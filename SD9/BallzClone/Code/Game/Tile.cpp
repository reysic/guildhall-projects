#include "Engine/Renderer/Renderer.hpp"

#include "Game/Tile.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( int numHitsRemaining, const IntVector2& gridPosition )
	: m_numHitsRemaining( numHitsRemaining )
	, m_gridPosition( gridPosition )
{
	m_worldPosition = Vector2( gridPosition.x * 100.0f + 50.0f, gridPosition.y * 100.0f + 50.0f );
}


//-----------------------------------------------------------------------------------------------
void Tile::Update( float deltaSeconds )
{
	deltaSeconds;
}


//-----------------------------------------------------------------------------------------------
void Tile::Render()
{
	Vector2 screenSize( 7.0f, 9.0f );
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), screenSize );

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	float fontCellHeight = 0.25f;

	g_theRenderer->DepthTestingEnabled( false );

	g_theRenderer->DrawQuad( Vector2( ( float ) m_gridPosition.x, ( float ) m_gridPosition.y ), Vector2( ( float ) m_gridPosition.x + 1, ( float ) m_gridPosition.y + 1 ), Rgba::WHITE );
	g_theRenderer->DrawText2D( Vector2( ( float ) m_gridPosition.x, ( float ) m_gridPosition.y ), std::to_string( m_numHitsRemaining ), fontCellHeight, Rgba::BLACK, fixedFont );

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	g_theRenderer->DrawFilledPolygon( 20, m_worldPosition.x, m_worldPosition.y, 10.0f, Rgba::RED );
}