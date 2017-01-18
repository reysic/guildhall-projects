//-----------------------------------------------------------------------------------------------
// Tile.hpp
// Represents a tile in the map

#pragma once


#include "Engine/Math/IntVector2.hpp"


//-----------------------------------------------------------------------------------------------
typedef IntVector2 TileCoords;


enum TileType
{
	TILE_TYPE_GRASS, // 0
	TILE_TYPE_STONE, // 1
	NUM_TILE_TYPES
};


//-----------------------------------------------------------------------------------------------
class Tile
{
public:
	TileType m_type;
	TileCoords m_tileCoords;
	bool m_containsEntity;
};