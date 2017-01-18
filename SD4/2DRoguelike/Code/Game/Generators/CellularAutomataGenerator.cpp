#include "Engine/Math/MathUtils.hpp"
#include "Game/Generators/CellularAutomataGenerator.hpp"


//-----------------------------------------------------------------------------------------------
// GeneratorRegistration CellularAutomataGenerator::s_bigCaverns( "BigCaverns", &CellularAutomataGenerator::CreateGenerator );
// GeneratorRegistration CellularAutomataGenerator::s_smallCaverns( "SmallCaverns", &CellularAutomataGenerator::CreateGenerator );


//-----------------------------------------------------------------------------------------------
GeneratorRegistration CellularAutomataGenerator::s_CAGRegistration( "CellularAutomata", &CellularAutomataGenerator::CreateGenerator, &Generator::CreateEnvironmentGenerationProcess );


//-----------------------------------------------------------------------------------------------
Map* CellularAutomataGenerator::CreateInitializedMap( const IntVector2& size, const std::string& name )
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
			initializedMap->m_cells[ cellIndex ].m_position = IntVector2( cellX, cellY );
			float floatBetweenZeroAndOne = GetRandomFloatZeroToOne();
			if ( floatBetweenZeroAndOne < 0.4f )
			{
				initializedMap->m_cells[ cellIndex ].m_currentType = CELL_TYPE_AIR;
			}
			else
			{
				initializedMap->m_cells[ cellIndex ].m_currentType = CELL_TYPE_STONE;
			}

			cellIndex++;
		}
	}

	initializedMap->SetSize( size );

	return initializedMap;
}


//-----------------------------------------------------------------------------------------------
bool CellularAutomataGenerator::GenerateStep( Map* map, int currentStepNumber )
{
	UNUSED( currentStepNumber );

	// Determine future cell types
	std::vector< Cell> neighborAirCells;

	for ( Cell& cell : map->m_cells )
	{
		neighborAirCells = map->GetNeighborsInRangeOfType( cell.m_position, 1, CELL_TYPE_AIR );

		if ( cell.m_currentType == CELL_TYPE_AIR )
		{
			if ( neighborAirCells.size() < 3 )
			{
				cell.m_futureType = CELL_TYPE_STONE;
			}
			else
			{
				cell.m_futureType = CELL_TYPE_AIR;
			}
		}
		else
		{
			if ( neighborAirCells.size() >= 5 )
			{
				cell.m_futureType = CELL_TYPE_AIR;
			}
			else
			{
				cell.m_futureType = CELL_TYPE_STONE;
			}
		}

		neighborAirCells.clear();
	}

	// Now set cells to future cell type
	for ( Cell& cell : map->m_cells )
	{
		cell.m_currentType = cell.m_futureType;
	}

	return true;
}
