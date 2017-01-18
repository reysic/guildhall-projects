#pragma once

#include "../../Arena/include/CommonInterface.hpp"


//-----------------------------------------------------------------------------------------------
enum CellType : unsigned char
{
	CELL_TYPE_AIR = 0,
	CELL_TYPE_DIRT = 1,
	CELL_TYPE_STONE = 2,
	CELL_TYPE_FOOD = 3,
	CELL_TYPE_UNKNOWN = 4,
	NUM_CELL_TYPES
};


//-----------------------------------------------------------------------------------------------
class MapCell
{
public:
	MapCell( CellType cellType );
	~MapCell();
	bool	IsClosed() const { return m_isClosed; }
	float	GetF() const { return m_gCostCumulative + m_hDistToEndGuess; }

public:
	static float	GetGCostForCellType( EntityType antType, CellType cellType );
	static bool		IsCellTypeSolid( CellType cellType );

public:
	mutable float				m_gCostCumulative;
	mutable float				m_hDistToEndGuess;
	mutable unsigned short		m_indexOfParent;
	CellType					m_type;
	mutable bool				m_isClosed;
};