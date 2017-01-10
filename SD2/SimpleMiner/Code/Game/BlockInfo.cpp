#include "Game/BlockInfo.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
BlockInfo::BlockInfo()
	: m_chunk( nullptr )
	, m_blockIndex( 0 )
{
}


//-----------------------------------------------------------------------------------------------
BlockInfo::BlockInfo( Chunk* chunk, int blockIndex )
	: m_chunk( chunk )
	, m_blockIndex( blockIndex )
{
}


//-----------------------------------------------------------------------------------------------
Block BlockInfo::GetBlock()
{
	return m_chunk->m_blocks[ m_blockIndex ];
}


//-----------------------------------------------------------------------------------------------
BlockInfo BlockInfo::GetNorthNeighborInfo() const
{
	if ( ( m_blockIndex & LOCAL_Y_MASK ) == LOCAL_Y_MASK )
	{
		// Current block is on chunk boundary
		if ( m_chunk->m_northNeighbor )
		{
			// Chunk containing current block has a north neighbor
			return BlockInfo( m_chunk->m_northNeighbor, m_blockIndex & ~LOCAL_Y_MASK );
		}
		else
		{
			// Chunk containing current block does not have a north neighbor
			return BlockInfo( nullptr, 0 );
		}
	}
	else
	{
		return BlockInfo( m_chunk, m_blockIndex + 16 );
	}
}


//-----------------------------------------------------------------------------------------------
BlockInfo BlockInfo::GetSouthNeighborInfo() const
{
	if ( ( m_blockIndex & LOCAL_Y_MASK ) == 0 )
	{
		// Current block is on chunk boundary
		if ( m_chunk->m_southNeighbor )
		{
			// Chunk containing current block has a south neighbor
			return BlockInfo( m_chunk->m_southNeighbor, m_blockIndex & ~LOCAL_Y_MASK | LOCAL_Y_MASK );
		}
		else
		{
			// Chunk containing current block does not have a south neighbor
			return BlockInfo( nullptr, 0 );
		}
	}
	else
	{
		return BlockInfo( m_chunk, m_blockIndex - 16 );
	}
}


//-----------------------------------------------------------------------------------------------
BlockInfo BlockInfo::GetEastNeighborInfo() const
{
	if ( ( m_blockIndex & LOCAL_X_MASK ) == LOCAL_X_MASK )
	{
		// Current block is on chunk boundary
		if ( m_chunk->m_eastNeighbor )
		{
			// Chunk containing current block has an east neighbor
			return BlockInfo( m_chunk->m_eastNeighbor, m_blockIndex & ~LOCAL_X_MASK );
		}
		else
		{
			// Chunk containing current block does not have an east neighbor
			return BlockInfo( nullptr, 0 );
		}
	}
	else
	{
		return BlockInfo( m_chunk, m_blockIndex + 1 );
	}
}


//-----------------------------------------------------------------------------------------------
BlockInfo BlockInfo::GetWestNeighborInfo() const
{
	if ( ( m_blockIndex & LOCAL_X_MASK ) == 0 )
	{
		// Current block is on chunk boundary
		if ( m_chunk->m_westNeighbor )
		{
			// Chunk containing current block has an west neighbor
			return BlockInfo( m_chunk->m_westNeighbor, m_blockIndex & ~LOCAL_X_MASK | LOCAL_X_MASK );
		}
		else
		{
			// Chunk containing current block does not have a west neighbor
			return BlockInfo( nullptr, 0 );
		}
	}
	else
	{
		return BlockInfo( m_chunk, m_blockIndex - 1 );
	}
}


//-----------------------------------------------------------------------------------------------
BlockInfo BlockInfo::GetTopNeighborInfo() const
{
	return BlockInfo( m_chunk, m_blockIndex + 256 );
}


//-----------------------------------------------------------------------------------------------
BlockInfo BlockInfo::GetBottomNeighborInfo() const
{
	return BlockInfo( m_chunk, m_blockIndex - 256 );
}