#include "Map.hpp"


//-----------------------------------------------------------------------------------------------
// Constants
const ShortVector2 STEP_EAST( 1, 0 );
const ShortVector2 STEP_WEST( -1, 0 );
const ShortVector2 STEP_NORTH( 0, -1 );
const ShortVector2 STEP_SOUTH( 0, 1 );

//-----------------------------------------------------------------------------------------------
// Globals
EntityType g_currentAntTypeForComputePath = ENTITY_TYPE_WORKER;


//-----------------------------------------------------------------------------------------------
Map::Map( short width, short height )
	: m_width( width )
	, m_height( height )
{
	unsigned short numCells = m_width * m_height;
	for ( int cellIndex = 0; cellIndex < numCells; ++cellIndex )
	{
		MapCell newCell = MapCell( CELL_TYPE_UNKNOWN );
		m_cells.push_back( newCell );
	}
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
}


//-----------------------------------------------------------------------------------------------
ShortVector2 Map::GetRandomInteriorNonSolidCellCoords() const
{
	// Loop forever until we find an interior (non-edge) location which is not solid
	for ( ;; )
	{
		short cellX = ( short ) GetRandomIntInRange( 1, m_width - 2 );
		short cellY = ( short ) GetRandomIntInRange( 1, m_height - 2 );
		ShortVector2 cellCoords( cellX, cellY );
		if ( !IsCellSolid( cellCoords ) )
			return cellCoords;
	}
}


//-----------------------------------------------------------------------------------------------
CellType Map::GetCellTypeForCoords( short cellX, short cellY ) const
{
	if ( cellX < 0 || cellX >= m_width || cellY < 0 || cellY >= m_height )
		return CELL_TYPE_STONE;

	unsigned short cellIndex = GetCellIndexForCoords( cellX, cellY );
	return m_cells[ cellIndex ].m_type;
}


//-----------------------------------------------------------------------------------------------
CellType Map::GetCellTypeForIndex( unsigned short cellIndex ) const
{
	return m_cells[ cellIndex ].m_type;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsCellSolid( const ShortVector2& cellCoords ) const
{
	CellType cellType = GetCellTypeForCoords( cellCoords.x, cellCoords.y );
	return MapCell::IsCellTypeSolid( cellType );
}


//-----------------------------------------------------------------------------------------------
float Map::GuessEstimatedCost( const ShortVector2& fromHere, const ShortVector2& toHere ) const
{
	int xDist = abs( toHere.x - fromHere.x );
	int yDist = abs( toHere.y - fromHere.y );
	return ( float ) ( xDist + yDist ); // simple Manhattan distance, since we're moving 4-way cardinal on 2D tile grid
}


//-----------------------------------------------------------------------------------------------
unsigned short Map::GetIndexOfCellWithLowestF( CellIndicesList& indicesList ) const
{
	unsigned short indexOfCellWithLowestF = indicesList[ 0 ];
	float lowestF = GetFForCellIndex( indexOfCellWithLowestF );

	for ( unsigned int listPosition = 1; listPosition < indicesList.size(); ++listPosition )
	{
		float fForThisIndex = GetFForCellIndex( indicesList[ listPosition ] );
		if ( fForThisIndex < lowestF )
		{
			indexOfCellWithLowestF = indicesList[ listPosition ];
			lowestF = fForThisIndex;
		}
		else if ( fForThisIndex == lowestF )
		{
			// Break a tie by comparing each cell's m_hDistToEndGuess
			MapCell cellBeingConsidered = m_cells[ indicesList[ listPosition ] ];
			MapCell currentCellWithLowestF = m_cells[ indexOfCellWithLowestF ];
			if ( cellBeingConsidered.m_hDistToEndGuess < currentCellWithLowestF.m_hDistToEndGuess )
			{
				indexOfCellWithLowestF = indicesList[ listPosition ];
				lowestF = fForThisIndex;
			}
		}
	}

	return indexOfCellWithLowestF;
}


//-----------------------------------------------------------------------------------------------
float Map::GetFForCellIndex( int cellIndex ) const
{
	return m_cells[ cellIndex ].GetF();
}


//-----------------------------------------------------------------------------------------------
void Map::BuildBackwardsPath( unsigned short endingCellIndex, CellCoordsList& out_path ) const
{
	unsigned short currentCellIndex = endingCellIndex;
	while ( currentCellIndex != 65535 )
	{
		ShortVector2 cellCoordsForCurrentCellIndex = GetCellCoordsForIndex( currentCellIndex );
		out_path.push_back( cellCoordsForCurrentCellIndex );
		currentCellIndex = m_cells[ currentCellIndex ].m_indexOfParent;
	}
}


//-----------------------------------------------------------------------------------------------
void Map::OpenNeighboringCellsAsAppropriate( unsigned short cellIndex, CellIndicesList& openCells, const ShortVector2& goalCoords ) const
{
	const ShortVector2& myCoords = GetCellCoordsForIndex( cellIndex );

	const ShortVector2 eastCoords = myCoords + STEP_EAST;
	const ShortVector2 northCoords = myCoords + STEP_NORTH;
	const ShortVector2 southCoords = myCoords + STEP_SOUTH;
	const ShortVector2 westCoords = myCoords + STEP_WEST;

	OpenNeighboringCellsAsAppropriate( eastCoords, cellIndex, openCells, goalCoords );
	OpenNeighboringCellsAsAppropriate( northCoords, cellIndex, openCells, goalCoords );
	OpenNeighboringCellsAsAppropriate( southCoords, cellIndex, openCells, goalCoords );
	OpenNeighboringCellsAsAppropriate( westCoords, cellIndex, openCells, goalCoords );
}


//-----------------------------------------------------------------------------------------------
void Map::OpenNeighboringCellsAsAppropriate( const ShortVector2& neighborCoords, unsigned short indexOfParent, CellIndicesList& openCells, const ShortVector2& goalCoords ) const
{
	CellType neighborType = GetCellTypeForCoords( neighborCoords.x, neighborCoords.y );
	if ( MapCell::IsCellTypeSolid( neighborType ) )
	{
		return; // Don't open inaccessible cells
	}

	unsigned short cellIndex = GetCellIndexForCoords( neighborCoords.x, neighborCoords.y );
	if ( m_cells[ cellIndex ].IsClosed() )
	{
		return; // Don't reopen already-closed cells
	}

	float gCostForNeighbor = MapCell::GetGCostForCellType( g_currentAntTypeForComputePath, neighborType );
	float gCostToNeighborCumulative = m_cells[ indexOfParent ].m_gCostCumulative + gCostForNeighbor;

	bool alreadyOpenNeighbor = false;
	for ( unsigned int listPosition = 0; listPosition < openCells.size(); ++listPosition )
	{
		if ( openCells[ listPosition ] == cellIndex )
		{
			alreadyOpenNeighbor = true;
			break;
		}
	}

	if ( alreadyOpenNeighbor )
	{
		// This cell is already open; check if we've discovered a cheaper way to reach it
		if ( gCostToNeighborCumulative < m_cells[ cellIndex ].m_gCostCumulative )
		{
			// We have found a cheaper way!  Update the cheaper cost and set us as parent.
			m_cells[ cellIndex ].m_gCostCumulative = gCostToNeighborCumulative;
			m_cells[ cellIndex ].m_indexOfParent = indexOfParent;
			return;
		}
	}
	else
	{
		// This cell has never been opened before; open it!
		float hDistToEndGuess = GuessEstimatedCost( neighborCoords, goalCoords );
		m_cells[ cellIndex ].m_indexOfParent = indexOfParent;
		m_cells[ cellIndex ].m_gCostCumulative = gCostToNeighborCumulative;
		m_cells[ cellIndex ].m_hDistToEndGuess = hDistToEndGuess;
		openCells.push_back( cellIndex );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::ComputePath( EntityType antType, const ShortVector2& start, const ShortVector2& goal, CellCoordsList& out_path ) const
{
	g_currentAntTypeForComputePath = antType;

	out_path.clear();
	ResetAllCellsToNotClosed();

	CellIndicesList openCellList;

	if ( IsCellSolid( start ) )
		return;

	// Set up starting cell and push to open list
	unsigned short cellIndex = GetCellIndexForCoords( start.x, start.y );
	m_cells[ cellIndex ].m_indexOfParent = 65535;
	m_cells[ cellIndex ].m_gCostCumulative = 0.0f;
	m_cells[ cellIndex ].m_hDistToEndGuess = GuessEstimatedCost( start, goal );
	openCellList.push_back( cellIndex );

	// Iterate until the end is found (or we've exhausted all leads)
	for ( ;; )
	{
		if ( openCellList.empty() )
		{
			openCellList.clear();
			return; // FAILURE - We're out of leads, and we never found the end.  No path possible!
		}

 		// Select the "best" open cell (the one with the lowest f=g+h cost)
		unsigned short indexOfCellWithLowestF = GetIndexOfCellWithLowestF( openCellList );

		// Check if it is the end
		ShortVector2 coordsOfCellWithLowestF = GetCellCoordsForIndex( indexOfCellWithLowestF );
		if ( coordsOfCellWithLowestF == goal )
		{
			BuildBackwardsPath( indexOfCellWithLowestF, out_path );
			openCellList.clear();

			return; // SUCCESS - path found!
		}

		// Move it from the open list to the closed list
		for ( unsigned int listPosition = 0; listPosition < openCellList.size(); ++listPosition )
		{
			if ( openCellList[ listPosition ] == indexOfCellWithLowestF )
			{
				openCellList.erase( openCellList.begin() + listPosition );
				break;
			}
		}
		m_cells[ indexOfCellWithLowestF ].m_isClosed = true;

 		// Add each neighbor to open set, if appropriate
		OpenNeighboringCellsAsAppropriate( indexOfCellWithLowestF, openCellList, goal );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::ResetAllCellsToNotClosed() const
{
	int numCells = m_width * m_height;
	for ( int cellIndex = 0; cellIndex < numCells; ++cellIndex )
	{
		m_cells[ cellIndex ].m_isClosed = false;
	}
}