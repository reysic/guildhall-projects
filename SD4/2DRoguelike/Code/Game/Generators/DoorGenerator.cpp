#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntAABB2.hpp"
#include "Game/Generators/DoorGenerator.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"
#include "Game/Entities/FeatureFactory.hpp"


//-----------------------------------------------------------------------------------------------
GeneratorRegistration DoorGenerator::s_DoorGeneratorRegistration( "Door", &DoorGenerator::CreateGenerator, &Generator::CreateEnvironmentGenerationProcess );


//-----------------------------------------------------------------------------------------------
Map* DoorGenerator::CreateInitializedMap( const IntVector2& size, const std::string& name )
{
	Map* initializedMap = new Map( size, name );

	int numCells = size.x * size.y;
	initializedMap->m_cells.resize( numCells );

	// Build the map out of cells
	int cellIndex = 0;

	for ( int cellY = 0; cellY < size.y; cellY++ )
	{
		for ( int cellX = 0; cellX < size.x; cellX++ )
		{
			// Set cell position
			initializedMap->m_cells[ cellIndex ].m_position = IntVector2( cellX, cellY );

			// Set cell type
			initializedMap->m_cells[ cellIndex ].m_currentType = CELL_TYPE_STONE;

			cellIndex++;
		}
	}

	m_startPosition = IntVector2( RandomInt( size.x - 4 ) + 2, RandomInt( size.y - 4 ) + 2 );
	m_direction = ( CellDirection ) RandomInt( 4 );

	initializedMap->SetSize( size );

	return initializedMap;
}


//-----------------------------------------------------------------------------------------------
bool DoorGenerator::GenerateStep( Map* map, int currentStepNumber )
{
	UNUSED( currentStepNumber );

	// Find all empty air tiles next to a wall
	std::vector< AdjacentCells > freeCells = GetAirCellsNextToWall( map );

	if ( freeCells.size() > 0 )
	{
		int randomIndex = RandomInt( ( int ) freeCells.size() );
		m_direction = freeCells[ randomIndex ].m_direction;
		IntVector2 buildOrigin = freeCells[ randomIndex ].m_position;
		AttemptToMakeDoor( map, buildOrigin );
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool DoorGenerator::AttemptToMakeDoor( Map* map, IntVector2 startPos )
{
	CellType northType = map->GetCellTypeAtPosition( startPos.x, startPos.y + 1 );
	CellType southType = map->GetCellTypeAtPosition( startPos.x, startPos.y - 1 );
	CellType eastType = map->GetCellTypeAtPosition( startPos.x + 1, startPos.y );
	CellType westType = map->GetCellTypeAtPosition( startPos.x - 1, startPos.y );

	// Check if North and South cells are stone
	if ( northType == CELL_TYPE_STONE && southType == CELL_TYPE_STONE )
	{
		// Place a door
		return MakeDoor( map, startPos );
	}
	// Check if East and West cells are stone
	else if ( eastType == CELL_TYPE_STONE && westType == CELL_TYPE_STONE )
	{
		// Place a door
		return MakeDoor( map, startPos );
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool DoorGenerator::MakeDoor( Map* map, IntVector2 startPos )
{
	Cell& doorCell = map->GetCellAtPosition( startPos );

	if ( map->IsPositionValid( doorCell.m_position ) )
	{
		Feature* newDoor = FeatureFactory::MakeItemsOfType( FEATURE_TYPE_DOOR )[ 0 ];

		newDoor->m_position = startPos;
		newDoor->m_map = map;

		doorCell.m_containsFeature = true;
		doorCell.m_containedFeature = newDoor;
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
std::vector< AdjacentCells > DoorGenerator::GetAirCellsNextToWall( Map* map )
{
	AdjacentCells foundTile;
	std::vector< AdjacentCells > result;

	for ( int yIndex = 0; yIndex < map->m_mapSizeInCells.y; ++yIndex )
	{
		for ( int xIndex = 0; xIndex < map->m_mapSizeInCells.x; ++xIndex )
		{
			foundTile.m_position = IntVector2( xIndex, yIndex );
			if ( map->GetCellTypeAtPosition( xIndex, yIndex ) == CELL_TYPE_AIR )
			{
				// Check North
				if ( map->IsPositionValid( IntVector2( xIndex, yIndex + 1 ) ) && map->GetCellTypeAtPosition( xIndex, yIndex + 1 ) == CELL_TYPE_STONE )
				{
					foundTile.m_direction = CELL_DIRECTION_NORTH;
					result.push_back( foundTile );
				}

				// Check South
				if ( map->IsPositionValid( xIndex, yIndex - 1 ) && map->GetCellTypeAtPosition( xIndex, yIndex - 1 ) == CELL_TYPE_STONE )
				{
					foundTile.m_direction = CELL_DIRECTION_SOUTH;
					result.push_back( foundTile );
				}

				// Check East
				if ( map->IsPositionValid( xIndex + 1, yIndex ) && map->GetCellTypeAtPosition( xIndex + 1, yIndex ) == CELL_TYPE_STONE )
				{
					foundTile.m_direction = CELL_DIRECTION_EAST;
					result.push_back( foundTile );
				}

				// Check West
				if ( map->IsPositionValid( xIndex - 1, yIndex ) && map->GetCellTypeAtPosition( xIndex - 1, yIndex ) == CELL_TYPE_STONE )
				{
					foundTile.m_direction = CELL_DIRECTION_WEST;
					result.push_back( foundTile );
				}
			}
		}
	}

	return result;
}