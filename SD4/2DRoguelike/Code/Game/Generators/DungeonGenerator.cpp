#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntAABB2.hpp"
#include "Game/Generators/DungeonGenerator.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
GeneratorRegistration DungeonGenerator::s_DGRegistration( "Dungeon", &DungeonGenerator::CreateGenerator, &Generator::CreateEnvironmentGenerationProcess );


//-----------------------------------------------------------------------------------------------
Map* DungeonGenerator::CreateInitializedMap( const IntVector2& size, const std::string& name )
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

	m_roomWidth = RandomInt( 6, 3 );
	m_roomHeight = RandomInt( 6, 3 );
	m_hallwayLengthMin = 3;
	m_hallwayLengthMax = 10;

	m_startPosition = IntVector2( RandomInt( size.x - 4 ) + 2, RandomInt( size.y - 4 ) + 2 );
	m_direction = ( CellDirection ) RandomInt( 4 );
	AttemptToMakeRoom( initializedMap, m_startPosition );

	initializedMap->SetSize( size );

	return initializedMap;
}


//-----------------------------------------------------------------------------------------------
bool DungeonGenerator::GenerateStep( Map* map, int currentStepNumber )
{
	UNUSED( currentStepNumber );

	// Find all empty air tiles next to a wall
	std::vector< AdjacentCells > freeCells = GetAirCellsNextToWall( map );

	if ( freeCells.size() > 0 )
	{
		int randomIndex = RandomInt( ( int ) freeCells.size() );
		m_direction = freeCells[ randomIndex ].m_direction;
		IntVector2 buildOrigin = freeCells[ randomIndex ].m_position;

		if ( AttemptToMakeHallway( map, &buildOrigin ) )
		{
			AttemptToMakeRoom( map, buildOrigin );
		}
	}

	AttemptToMakeRoom( map, m_startPosition );

	return true;
}


//-----------------------------------------------------------------------------------------------
bool DungeonGenerator::AttemptToMakeRoom( Map* map, IntVector2 startPos )
{
	IntAABB2 room = CreateRoom( map, startPos );

	if ( map->DoesAreaContainCellType( room, CellType::CELL_TYPE_AIR ) )
	{
		return false;
	}
	else
	{
		map->SetAreaCellType( room, CELL_TYPE_AIR );
		return true;
	}
}


//-----------------------------------------------------------------------------------------------
IntAABB2 DungeonGenerator::CreateRoom( Map* map, IntVector2 startPos )
{
	UNUSED( map );

	m_roomWidth = RandomInt( 6, 3 );
	m_roomHeight = RandomInt( 6, 3 );
	IntAABB2 room;

	switch ( m_direction )
	{
		case CellDirection::CELL_DIRECTION_NORTH:
		{
			int minsX = startPos.x - m_roomWidth;
			int minsY = startPos.y;
			int maxX = minsX + m_roomWidth;
			int maxY = startPos.y + m_roomHeight;
			room = IntAABB2( IntVector2( minsX, minsY ), IntVector2( maxX, maxY ) );
			return room;
			break;
		}
		case CellDirection::CELL_DIRECTION_SOUTH:
		{
			int xMin = startPos.x - RandomInt( m_roomWidth );
			int xMax = xMin + m_roomWidth - 1;
			int yMax = startPos.y;
			int yMin = yMax - m_roomHeight + 1;
			return IntAABB2( IntVector2( xMin, yMin ), IntVector2( xMax, yMax ) );
			break;
		}
		case CellDirection::CELL_DIRECTION_EAST:
		{
			int xMin = startPos.x;
			int xMax = xMin + m_roomWidth - 1;
			int yMin = startPos.y - RandomInt( m_roomHeight );
			int yMax = yMin + m_roomHeight - 1;
			return IntAABB2( IntVector2( xMin, yMin ), IntVector2( xMax, yMax ) );
			break;
		}
		case CellDirection::CELL_DIRECTION_WEST:
		{
			int xMax = startPos.x;
			int xMin = xMax - m_roomWidth + 1;
			int yMin = startPos.y - RandomInt( m_roomHeight );
			int yMax = yMin + m_roomHeight - 1;
			return IntAABB2( IntVector2( xMin, yMin ), IntVector2( xMax, yMax ) );
			break;
		}
		default:
		{
			return IntAABB2( IntVector2( -1, -1 ), IntVector2( -1, -1 ) );
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool DungeonGenerator::AttemptToMakeHallway( Map* map, IntVector2* outPos )
{
	IntAABB2 const hallArea = MakeHallway( outPos );

	// Check room area and border
	if ( map->DoesAreaContainCellType( hallArea, CELL_TYPE_STONE ) )
	{
		// Stamp room
		map->SetAreaCellType( hallArea, CELL_TYPE_AIR );

		if ( map->IsPositionValid( outPos->x, outPos->y ) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		// No hall made
		return false;
	}
}

//-------------------------------------------------------------------------------------------------
IntAABB2 const DungeonGenerator::MakeHallway( IntVector2 * const out_buildOrigin )
{
	// #TODO: Maybe do hall size checks
	int length = RandomInt( 11, 3 );

	switch ( m_direction )
	{
		case CELL_DIRECTION_NORTH:
		{
			int xMin = out_buildOrigin->x;
			int xMax = xMin;
			int yMin = out_buildOrigin->y + 1;
			int yMax = yMin + length;
			*out_buildOrigin = IntVector2( xMax, yMax + 1 );
			return IntAABB2( IntVector2( xMin, yMin ), IntVector2( xMax, yMax ) );
			break;
		}
		case CELL_DIRECTION_SOUTH:
		{
			int xMin = out_buildOrigin->x;
			int xMax = xMin;
			int yMax = out_buildOrigin->y - 1;
			int yMin = yMax - length;
			*out_buildOrigin = IntVector2( xMin, yMin - 1 );
			return IntAABB2( IntVector2( xMin, yMin ), IntVector2( xMax, yMax ) );
			break;
		}
		case CELL_DIRECTION_EAST:
		{
			int xMin = out_buildOrigin->x + 1;
			int xMax = xMin + length;
			int yMin = out_buildOrigin->y;
			int yMax = yMin;
			*out_buildOrigin = IntVector2( xMax + 1, yMax );
			return IntAABB2( IntVector2( xMin, yMin ), IntVector2( xMax, yMax ) );
			break;
		}
		case CELL_DIRECTION_WEST:
		{
			int xMax = out_buildOrigin->x - 1;
			int xMin = xMax - length;
			int yMax = out_buildOrigin->y;
			int yMin = yMax;
			*out_buildOrigin = IntVector2( xMin - 1, yMin );
			return IntAABB2( IntVector2( xMin, yMin ), IntVector2( xMax, yMax ) );
			break;
		}
		default:
		{
			return IntAABB2( IntVector2( -1, -1 ), IntVector2( -1, -1 ) );
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
std::vector< AdjacentCells > DungeonGenerator::GetAirCellsNextToWall( Map* map )
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