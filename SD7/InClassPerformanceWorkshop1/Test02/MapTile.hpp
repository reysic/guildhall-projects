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
enum TileType : unsigned char
{
	TILE_TYPE_UNKNOWN = 0,
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
	TileType					m_type;
};