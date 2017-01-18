#include "Engine/Renderer/SpriteSheet.hpp"
#include "Game/BlockDefinition.hpp"


//-----------------------------------------------------------------------------------------------
BlockDefinition g_blockDefinitions[ NUM_BLOCK_TYPES ];


//-----------------------------------------------------------------------------------------------
void BlockDefinition::InitializeBlockDefinitions()
{
	static SpriteSheet spriteSheet( "Data/Images/SimpleMinerAtlas.png", 16, 16 );

	g_blockDefinitions[ BLOCK_TYPE_AIR ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_AIR ].m_isOpaque = false;
	g_blockDefinitions[ BLOCK_TYPE_AIR ].m_isSolid = false;

	g_blockDefinitions[ BLOCK_TYPE_GRASS ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_GRASS ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_GRASS ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_GRASS ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 9, 8 );
	g_blockDefinitions[ BLOCK_TYPE_GRASS ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 8, 8 );
	g_blockDefinitions[ BLOCK_TYPE_GRASS ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 7, 8 );
	g_blockDefinitions[ BLOCK_TYPE_GRASS ].m_toughness = 0.5f;

	g_blockDefinitions[ BLOCK_TYPE_STONE ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_STONE ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_STONE ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_STONE ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 3, 10 );
	g_blockDefinitions[ BLOCK_TYPE_STONE ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 3, 10 );
	g_blockDefinitions[ BLOCK_TYPE_STONE ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 3, 10 );
	g_blockDefinitions[ BLOCK_TYPE_STONE ].m_toughness = 2.0f;

	g_blockDefinitions[ BLOCK_TYPE_DIRT ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_DIRT ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_DIRT ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_DIRT ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 7, 8 );
	g_blockDefinitions[ BLOCK_TYPE_DIRT ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 7, 8 );
	g_blockDefinitions[ BLOCK_TYPE_DIRT ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 7, 8 );
	g_blockDefinitions[ BLOCK_TYPE_DIRT ].m_toughness = 0.5f;

	g_blockDefinitions[ BLOCK_TYPE_SAND ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_SAND ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_SAND ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_SAND ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 1, 8 );
	g_blockDefinitions[ BLOCK_TYPE_SAND ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 1, 8 );
	g_blockDefinitions[ BLOCK_TYPE_SAND ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 1, 8 );
	g_blockDefinitions[ BLOCK_TYPE_SAND ].m_toughness = 0.5f;

	g_blockDefinitions[ BLOCK_TYPE_COBBLESTONE ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_COBBLESTONE ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_COBBLESTONE ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_COBBLESTONE ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 5, 10 );
	g_blockDefinitions[ BLOCK_TYPE_COBBLESTONE ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 5, 10 );
	g_blockDefinitions[ BLOCK_TYPE_COBBLESTONE ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 5, 10 );
	g_blockDefinitions[ BLOCK_TYPE_COBBLESTONE ].m_toughness = 2.0f;

	g_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_selfIlluminationValue = 12;
	g_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 4, 11 );
	g_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 4, 11 );
	g_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 4, 11 );
	g_blockDefinitions[ BLOCK_TYPE_GLOWSTONE ].m_toughness = 1.0f;

	g_blockDefinitions[ BLOCK_TYPE_WATER ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_WATER ].m_isOpaque = false;
	g_blockDefinitions[ BLOCK_TYPE_WATER ].m_isSolid = false;
	g_blockDefinitions[ BLOCK_TYPE_WATER ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 15, 11 );
	g_blockDefinitions[ BLOCK_TYPE_WATER ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 15, 11 );
	g_blockDefinitions[ BLOCK_TYPE_WATER ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 15, 11 );
	g_blockDefinitions[ BLOCK_TYPE_WATER ].m_toughness = 1.0f;

	g_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 15, 8 );
	g_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 14, 8 );
	g_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 15, 8 );
	g_blockDefinitions[ BLOCK_TYPE_OAK_WOOD ].m_toughness = 1.0f;

	g_blockDefinitions[ BLOCK_TYPE_BIRCH_WOOD ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_BIRCH_WOOD ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_BIRCH_WOOD ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_BIRCH_WOOD ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 15, 9 );
	g_blockDefinitions[ BLOCK_TYPE_BIRCH_WOOD ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 14, 9 );
	g_blockDefinitions[ BLOCK_TYPE_BIRCH_WOOD ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 15, 9 );
	g_blockDefinitions[ BLOCK_TYPE_BIRCH_WOOD ].m_toughness = 1.0f;

	g_blockDefinitions[ BLOCK_TYPE_LEAVES ].m_selfIlluminationValue = 0;
	g_blockDefinitions[ BLOCK_TYPE_LEAVES ].m_isOpaque = true;
	g_blockDefinitions[ BLOCK_TYPE_LEAVES ].m_isSolid = true;
	g_blockDefinitions[ BLOCK_TYPE_LEAVES ].m_topTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 12, 8 );
	g_blockDefinitions[ BLOCK_TYPE_LEAVES ].m_sideTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 12, 8 );
	g_blockDefinitions[ BLOCK_TYPE_LEAVES ].m_bottomTexCoords = spriteSheet.GetTexCoordsForSpriteCoords( 12, 8 );
	g_blockDefinitions[ BLOCK_TYPE_LEAVES ].m_toughness = 1.0f;
}