#pragma once

#include "Game/GameCommon.hpp"
#include "Game/Chunk.hpp"
#include "Game/Block.hpp"


//-----------------------------------------------------------------------------------------------
class BlockInfo
{
public:
	BlockInfo();
	BlockInfo( Chunk* chunk, int blockIndex );
	Block GetBlock();
	BlockInfo GetNorthNeighborInfo() const;
	BlockInfo GetSouthNeighborInfo() const;
	BlockInfo GetEastNeighborInfo() const;
	BlockInfo GetWestNeighborInfo() const;
	BlockInfo GetTopNeighborInfo() const;
	BlockInfo GetBottomNeighborInfo() const;
	BlockInfo GetBlockInfoForWorldPosition( const Vector3& worldPosition ) const;
	inline bool IsValid();
	inline bool IsSolid();
	inline BlockType GetBlockType();

public:
	Chunk* m_chunk;
	int m_blockIndex;
};


//-----------------------------------------------------------------------------------------------
inline bool BlockInfo::IsValid()
{
	return ( m_chunk != nullptr );
}


//-----------------------------------------------------------------------------------------------
inline bool BlockInfo::IsSolid()
{
	Block* b = &m_chunk->m_blocks[ m_blockIndex ];
	return b->IsSolid();
}


//-----------------------------------------------------------------------------------------------
inline BlockType BlockInfo::GetBlockType()
{
	if ( m_chunk )
	{
		return ( ( BlockType ) m_chunk->m_blocks[ m_blockIndex ].m_blockType );
	}
	return BLOCK_TYPE_AIR;
}