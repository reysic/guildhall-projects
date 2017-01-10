#pragma once

#include <vector>
#include <map>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Renderer/Vertex.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Block.hpp"


//-----------------------------------------------------------------------------------------------
struct TreeBlock
{
	TreeBlock( BlockType type, Vector3 offsetFromTreeBase )
		: m_type( type )
		, m_offsetFromTreeBase( offsetFromTreeBase )
	{}

	BlockType         m_type;
	Vector3        m_offsetFromTreeBase;
};


//-----------------------------------------------------------------------------------------------
const TreeBlock OAK_TREE_BLOCK_OFFSETS[] =
{
	TreeBlock( BLOCK_TYPE_OAK_WOOD, Vector3( 0.0f, 0.0f, 1.0f ) ),
	TreeBlock( BLOCK_TYPE_OAK_WOOD, Vector3( 0.0f, 0.0f, 2.0f ) ),
	TreeBlock( BLOCK_TYPE_OAK_WOOD, Vector3( 0.0f, 0.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, 1.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, -1.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 1.0f, 0.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( -1.0f, 0.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, 0.0f, 4.0f ) ),
};

const int OAK_TREE_NUM_BLOCKS = sizeof( OAK_TREE_BLOCK_OFFSETS ) / sizeof( OAK_TREE_BLOCK_OFFSETS[ 0 ] );


//-----------------------------------------------------------------------------------------------
const TreeBlock BIRCH_TREE_BLOCK_OFFSETS[] =
{
	TreeBlock( BLOCK_TYPE_BIRCH_WOOD, Vector3( 0.0f, 0.0f, 1.0f ) ),
	TreeBlock( BLOCK_TYPE_BIRCH_WOOD, Vector3( 0.0f, 0.0f, 2.0f ) ),
	TreeBlock( BLOCK_TYPE_BIRCH_WOOD, Vector3( 0.0f, 0.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_BIRCH_WOOD, Vector3( 0.0f, 0.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_BIRCH_WOOD, Vector3( 0.0f, 0.0f, 5.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, 1.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, -1.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 1.0f, 0.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( -1.0f, 0.0f, 3.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, 1.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, -1.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 1.0f, 0.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( -1.0f, 0.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 1.0f, 1.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( -1.0f, 1.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 1.0f, -1.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( -1.0f, -1.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, 2.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, -2.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 2.0f, 0.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( -2.0f, 0.0f, 4.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, 1.0f, 5.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, -1.0f, 5.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 1.0f, 0.0f, 5.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( -1.0f, 0.0f, 5.0f ) ),
	TreeBlock( BLOCK_TYPE_LEAVES, Vector3( 0.0f, 0.0f, 6.0f ) ),
};

const int BIRCH_TREE_NUM_BLOCKS = sizeof( BIRCH_TREE_BLOCK_OFFSETS ) / sizeof( BIRCH_TREE_BLOCK_OFFSETS[ 0 ] );

//-----------------------------------------------------------------------------------------------
class Chunk
{
public:
	Chunk( IntVector2 chunkCoords );
	Chunk( IntVector2 chunkCoords, Block loadedBlocks[ NUM_BLOCKS_PER_CHUNK ] );
	~Chunk();
	void ComputePerlinNoiseForChunkColumns();
	void ComputePerlinNoiseForTrees();
	void Update();
	void GenerateVertexArrayAndVBO();
	void CheckFrustumCulling();
	IntVector3 GetLocalCoordsForBlockIndex( int blockIndex ) const;
	int GetBlockIndexForLocalCoords( const IntVector3& lc ) const;
	void GenerateBlocks();
	void PopulateVertexArray( std::vector< Vertex_PCT >& out_vertexArray );
	void AddBlockVertexes( int blockIndex, std::vector< Vertex_PCT >& out_vertexArray );
	void Render() const;
	void LocateTrees();
	bool IsTreeAtArrayIndex( int arrayIndex );
	void MakeTree( int columnHeight, const Vector2& globalCoords2D );

public:
	IntVector2 m_chunkCoords;
	Block m_blocks[ NUM_BLOCKS_PER_CHUNK ];
	bool m_vboIsOutdated;
	Vector3 m_worldMins;
	Vector2 m_chunkCenterPos;
	float m_chunkColumnHeights[ BLOCKS_WIDE_X * BLOCKS_WIDE_Y ];
	float m_perlinNoiseForTrees[ ( BLOCKS_WIDE_X + 4 ) * ( BLOCKS_WIDE_Y + 4 ) ];
	Chunk* m_northNeighbor;
	Chunk* m_southNeighbor;
	Chunk* m_eastNeighbor;
	Chunk* m_westNeighbor;
	bool m_isRendered;
	unsigned int m_vboID;
	int m_numVerts;
};


//-----------------------------------------------------------------------------------------------
inline IntVector3 Chunk::GetLocalCoordsForBlockIndex( int blockIndex ) const
{
	IntVector3 localCoords;
	localCoords.x = blockIndex & LOCAL_X_MASK;
	localCoords.y = ( blockIndex & LOCAL_Y_MASK ) >> CHUNK_BITS_X;
	localCoords.z = ( blockIndex & LOCAL_Z_MASK ) >> ( CHUNK_BITS_X + CHUNK_BITS_Y );
	ASSERT_OR_DIE( localCoords.x >= 0 && localCoords.x < BLOCKS_WIDE_X, "Error!" );
	return localCoords;
}


//-----------------------------------------------------------------------------------------------
inline int Chunk::GetBlockIndexForLocalCoords( const IntVector3& localCoords ) const
{
	return localCoords.x | ( localCoords.y << CHUNK_BITS_X ) | ( localCoords.z << CHUNK_BITS_XY );
}