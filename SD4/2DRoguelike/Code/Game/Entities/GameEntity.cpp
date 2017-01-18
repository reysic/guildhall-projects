#include "Game/Entities/GameEntity.hpp"


//-----------------------------------------------------------------------------------------------
int GameEntity::s_nextID = 1;


//-----------------------------------------------------------------------------------------------
GameEntity::GameEntity( GameEntityType type )
	: m_position( IntVector2( 0, 0 ) )
	, m_glyph( "?" )
	, m_color( Rgba::WHITE )
	, m_name( "" )
	, m_id( s_nextID++ )
	, m_type( type )
	, m_map( nullptr )
	, m_health( 1 )
{
}


//-----------------------------------------------------------------------------------------------
GameEntity::GameEntity( const GameEntity& copyFrom )
	: m_position( copyFrom.m_position )
	, m_glyph( copyFrom.m_glyph )
	, m_color( copyFrom.m_color )
	, m_name( copyFrom.m_name )
	, m_id( s_nextID++ )
	, m_type( copyFrom.m_type )
	, m_map( copyFrom.m_map )
	, m_health( copyFrom.m_health )
{
}


//-----------------------------------------------------------------------------------------------
float GameEntity::Update()
{
	return 1.0f;
}


//-----------------------------------------------------------------------------------------------
void GameEntity::Render() const
{
	// Don't render an entity that is not visible, is not the player, and when the map is not in debug render
	if ( !m_map->GetCellAtPosition( m_position ).m_isVisible && !m_map->m_debugRender && !IsPlayer() )
		return;

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	// Calculate offset based on map size
	Vector2 offset = m_map->m_renderingOffset;

	Vector2 entityPosition( ( float ) m_position.x * 20.0f, ( float ) m_position.y * 20.0f );
	g_theRenderer->DrawText2D( offset + entityPosition, GetGlyph(), 15.0f, m_color, fixedFont );
}