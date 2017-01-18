#include "MapTile.hpp"


//-----------------------------------------------------------------------------------------------
MapTile::MapTile( TileType tileType )
	: m_type( tileType )
	, m_typeName( GetNameForTileType( tileType ) )
	, m_glyph( GetGlyphForTileType( tileType ) )
	, m_gCost( GetGCostForTileType( tileType ) )
	, m_isSolid( IsTileTypeSolid( tileType ) )
{
}


//-----------------------------------------------------------------------------------------------
MapTile::~MapTile()
{
}


//-----------------------------------------------------------------------------------------------
STATIC std::string MapTile::GetNameForTileType( TileType tileType )
{
	switch( tileType )
	{
		case TILE_TYPE_WALL:		return "Wall";
		case TILE_TYPE_MUD:			return "Mud";
		case TILE_TYPE_GRASS:		return "Grass";
		case TILE_TYPE_FLAT:		return "Flat";
		default:					return "UNKNOWN";
	}
}


//-----------------------------------------------------------------------------------------------
STATIC unsigned char MapTile::GetGlyphForTileType( TileType tileType )
{
	switch( tileType )
	{
		case TILE_TYPE_WALL:		return ASCII_MEDIUM_SHADED_BLOCK;
		case TILE_TYPE_MUD:			return 'x';
		case TILE_TYPE_GRASS:		return '.'; 
		case TILE_TYPE_FLAT:		return ' '; 
		default:					return '?'; 
	}
}


//-----------------------------------------------------------------------------------------------
STATIC float MapTile::GetGCostForTileType( TileType tileType )
{
	switch( tileType )
	{
		case TILE_TYPE_WALL:		return 999.0f;
		case TILE_TYPE_MUD:			return 4.0f;
		case TILE_TYPE_GRASS:		return 2.0f;
		case TILE_TYPE_FLAT:		return 1.0f;
		default:					return 999.0f;
	}
}


//-----------------------------------------------------------------------------------------------
STATIC bool MapTile::IsTileTypeSolid( TileType tileType )
{
	switch( tileType )
	{
		case TILE_TYPE_WALL:		return true;
		case TILE_TYPE_MUD:			return false;
		case TILE_TYPE_GRASS:		return false;
		case TILE_TYPE_FLAT:		return false;
		default:					return true;
	}
}

