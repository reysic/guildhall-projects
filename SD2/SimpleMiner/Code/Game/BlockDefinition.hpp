#pragma once

#include "Engine/Math/AABB2.hpp"
#include "Game/GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
enum BlockType
{
	BLOCK_TYPE_AIR,			// 0
	BLOCK_TYPE_GRASS,		// 1
	BLOCK_TYPE_STONE,		// 2
	BLOCK_TYPE_DIRT,		// 3
	BLOCK_TYPE_SAND,	    // 4
	BLOCK_TYPE_COBBLESTONE, // 5
	BLOCK_TYPE_GLOWSTONE,   // 6
	BLOCK_TYPE_WATER,		// 7
	BLOCK_TYPE_OAK_WOOD,    // 8
	BLOCK_TYPE_BIRCH_WOOD,  // 9
	BLOCK_TYPE_LEAVES,      // 10
	NUM_BLOCK_TYPES
};

//-----------------------------------------------------------------------------------------------
class BlockDefinition;


//-----------------------------------------------------------------------------------------------
extern BlockDefinition g_blockDefinitions[ NUM_BLOCK_TYPES ];


//-----------------------------------------------------------------------------------------------
class BlockDefinition
{
public:
	static void BlockDefinition::InitializeBlockDefinitions();

public:
	unsigned char m_selfIlluminationValue;
	bool m_isOpaque;
	bool m_isSolid;
	AABB2 m_topTexCoords;
	AABB2 m_sideTexCoords;
	AABB2 m_bottomTexCoords;
	float m_toughness;
};