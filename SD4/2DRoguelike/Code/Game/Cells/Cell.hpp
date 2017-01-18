#pragma once

#include <vector>

#include "Engine/Math/IntVector2.hpp"


//-----------------------------------------------------------------------------------------------
class Agent;
class Item;
class Feature;


//-----------------------------------------------------------------------------------------------
enum CellType
{
	CELL_TYPE_STONE,
	CELL_TYPE_AIR,
	CELL_TYPE_WATER,
	CELL_TYPE_LAVA,
	CELL_TYPE_INVALID,
	NUM_CELL_TYPES
};


//-----------------------------------------------------------------------------------------------
class Cell
{
public:
	IntVector2 m_position;
	CellType m_currentType;
	CellType m_futureType;
	bool m_isHidden;
	bool m_isVisible = false;
	bool m_isKnown = false;
	bool m_containsAgent = false;
	Agent* m_containedAgent = nullptr;
	bool m_containsItem = false;
	std::vector< Item* > m_containedItems;
	bool m_containsFeature = false;
	bool m_blocksMovement = false;
	bool m_blocksLineOfSight = false;
	Feature* m_containedFeature = nullptr;

	bool DoesCellTypeBlockMovement( CellType type );
	bool DoesCellTypeBlockLineOfSight( CellType type );
};