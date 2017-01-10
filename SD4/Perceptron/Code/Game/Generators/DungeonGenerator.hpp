#pragma once

#include <vector>

#include "Game/Generators/Generator.hpp"


//-----------------------------------------------------------------------------------------------
class IntAABB2;
class IntVector2;
class Map;
class Cell;


//-----------------------------------------------------------------------------------------------
enum CellDirection
{
	CELL_DIRECTION_NORTH = 0,
	CELL_DIRECTION_SOUTH,
	CELL_DIRECTION_EAST,
	CELL_DIRECTION_WEST,
	CELL_DIRECTION_COUNT
};


//-----------------------------------------------------------------------------------------------
struct AdjacentCells
{
	CellDirection m_direction;
	IntVector2 m_position;
};


//-----------------------------------------------------------------------------------------------
class DungeonGenerator : public Generator
{
public:
	DungeonGenerator( const std::string& name ) : Generator( name ) {};
	static Generator* CreateGenerator( const std::string& name ) { return new DungeonGenerator( name ); };
	virtual Map* CreateInitializedMap( const IntVector2& size, const std::string& name );
	virtual bool GenerateStep( Map* map, int currentStepNumber );
	bool AttemptToMakeRoom( Map* map, IntVector2 startPos );
	IntAABB2 CreateRoom( Map* map, IntVector2 startPos );
	bool AttemptToMakeHallway( Map* map, IntVector2* startPos );
	IntAABB2 const MakeHallway( IntVector2 * const out_buildOrigin );
	std::vector< AdjacentCells > GetAirCellsNextToWall( Map* map );

private:
	static GeneratorRegistration s_DGRegistration;
	std::vector< Cell > m_borderCells;
	IntVector2 m_startPosition;
	CellDirection m_direction;
	int m_roomWidth;
	int m_roomHeight;
	int m_hallwayLengthMin;
	int m_hallwayLengthMax;
};