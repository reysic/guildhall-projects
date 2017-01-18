#pragma once

#include <vector>

#include "Game/Generators/Generator.hpp"
#include "Game/Cells/Cell.hpp"


//-----------------------------------------------------------------------------------------------
class IntAABB2;
class IntVector2;
class Map;


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
class DoorGenerator : public Generator
{
public:

	DoorGenerator( std::string name ) :Generator( name ) {};

	static Generator* CreateGenerator( const std::string& name ) { return new DoorGenerator( name ); };
	virtual Map* CreateInitializedMap( const IntVector2& size, const std::string& name );
	virtual bool GenerateStep( Map* map, int currentStepNumber );

	static GeneratorRegistration s_DoorGeneratorRegistration;

private:
	std::vector<Cell> m_borderCells;
	IntVector2 m_startPosition;
	CellDirection m_direction;

	bool AttemptToMakeDoor( Map* map, IntVector2 startPos );
	bool MakeDoor( Map* map, IntVector2 startPos );
	std::vector < AdjacentCells > GetAirCellsNextToWall( Map* map );
};