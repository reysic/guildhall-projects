#pragma once
#include "Engine/EngineBase.hpp"
#include <string>
#include <vector>


/////////////////////////////////////////////////////////////////////////////////////////////////
const unsigned char ASCII_WEAK_SHADED_BLOCK		= 0xB0;
const unsigned char ASCII_MEDIUM_SHADED_BLOCK	= 0xB1;
const unsigned char ASCII_STRONG_SHADED_BLOCK	= 0xB2;
const unsigned char ASCII_SOLID_SHADED_BLOCK	= 0xDB;


/////////////////////////////////////////////////////////////////////////////////////////////////
enum TileType
{
	TILE_TYPE_UNKNOWN = -1,
	TILE_TYPE_WALL,
	TILE_TYPE_MUD,
	TILE_TYPE_GRASS,
	TILE_TYPE_FLAT,
	NUM_TILE_TYPES
};


/////////////////////////////////////////////////////////////////////////////////////////////////
class MapFeature
{
public:
	std::string					m_name;
};


/////////////////////////////////////////////////////////////////////////////////////////////////
class MapTile
{
public:
	MapTile( TileType tileType );
	~MapTile();

public:
	static std::string		GetNameForTileType( TileType tileType );
	static unsigned char	GetGlyphForTileType( TileType tileType );
	static float			GetGCostForTileType( TileType tileType );
	static bool				IsTileTypeSolid( TileType tileType );

public:
	TileType					m_type;
	std::string					m_typeName;
	unsigned char				m_glyph;
	float						m_gCost;
	bool						m_isSolid;
	std::vector< MapFeature* >	m_featuresHere;
};


