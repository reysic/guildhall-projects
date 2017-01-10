#pragma once

#include "Game/BlockDefinition.hpp"


//-----------------------------------------------------------------------------------------------
class Block
{
public:
	// Block();
	inline void SetBlockType( BlockType type );
	inline bool IsSolid();

public:
	unsigned char m_blockType;
	unsigned char m_lightValueAndFlags;
};


//-----------------------------------------------------------------------------------------------
inline void Block::SetBlockType( BlockType type )
{
	m_blockType = ( unsigned char ) type;
}


//-----------------------------------------------------------------------------------------------
inline bool Block::IsSolid()
{
	return g_blockDefinitions[ m_blockType ].m_isSolid;
}