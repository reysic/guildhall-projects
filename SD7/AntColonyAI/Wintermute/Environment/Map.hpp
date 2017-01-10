#pragma once

#include "MapCell.hpp"
#include "../Math/ShortVector2.hpp"
#include "../Math/MathUtils.hpp"
#include "../../Arena/include/CommonInterface.hpp"

#include <vector>


//-----------------------------------------------------------------------------------------------
typedef std::vector< ShortVector2 >		CellCoordsList;
typedef std::vector< unsigned short >	CellIndicesList; // to be used for the open list


//-----------------------------------------------------------------------------------------------
struct Food
{
	ShortVector2 position = ShortVector2( -1, -1 );
	int idOfAntWithClaim = -1;
};


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	Map( short width, short height );
	~Map();
	void			ChangeCellType( unsigned short cellIndex, CellType newType );
	void			ChangeCellType( short cellX, short cellY, CellType newType );
	unsigned short	GetCellIndexForCoords( short cellX, short cellY ) const;
	ShortVector2	GetCellCoordsForIndex( unsigned short cellIndex ) const;
	ShortVector2	GetRandomInteriorNonSolidCellCoords() const;
	CellType		GetCellTypeForCoords( short cellX, short cellY ) const;
	CellType		GetCellTypeForIndex( unsigned short cellIndex ) const;
	bool			IsCellSolid( const ShortVector2& cellCoords ) const;
	float			GuessEstimatedCost( const ShortVector2& fromHere, const ShortVector2& toHere ) const;
	unsigned short	GetIndexOfCellWithLowestF( CellIndicesList& indicesList ) const;
	float			GetFForCellIndex( int cellIndex ) const;
	void			BuildBackwardsPath( unsigned short endingCellIndex, CellCoordsList& out_path ) const;
	void			OpenNeighboringCellsAsAppropriate( const ShortVector2& neighborCoords, unsigned short indexOfParent, CellIndicesList& openCells, const ShortVector2& goalCoords ) const;
	void			OpenNeighboringCellsAsAppropriate( unsigned short cellIndex, CellIndicesList& openCells, const ShortVector2& goalCoords ) const;
	void			ComputePath( EntityType antType, const ShortVector2& start, const ShortVector2& goal, CellCoordsList& out_path ) const;
	void			ResetAllCellsToNotClosed() const;

public:
	std::vector< MapCell >	m_cells;
	std::vector< Food >		m_foods;
	CellCoordsList			m_cellsChangedToStone;
	short					m_width;
	short					m_height;
};


//-----------------------------------------------------------------------------------------------
inline void Map::ChangeCellType( unsigned short cellIndex, CellType newType )
{
	if ( cellIndex >= 0 && cellIndex < ( m_width * m_height ) )
	{
		m_cells[ cellIndex ] = MapCell( newType );
	}
}


//-----------------------------------------------------------------------------------------------
inline void Map::ChangeCellType( short cellX, short cellY, CellType newType )
{
	unsigned short cellIndex = GetCellIndexForCoords( cellX, cellY );
	ChangeCellType( cellIndex, newType );
}


//-----------------------------------------------------------------------------------------------
inline unsigned short Map::GetCellIndexForCoords( short cellX, short cellY ) const
{
	return cellX + ( cellY * m_width );
}


//-----------------------------------------------------------------------------------------------
inline ShortVector2 Map::GetCellCoordsForIndex( unsigned short cellIndex ) const
{
	short cellX = cellIndex % m_width;
	short cellY = cellIndex / m_width;
	return ShortVector2( cellX, cellY );
}