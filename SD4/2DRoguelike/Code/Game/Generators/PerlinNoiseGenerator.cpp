#include <iostream>
#include <sstream>

#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Math/Noise.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Generators/PerlinNoiseGenerator.hpp"



//-----------------------------------------------------------------------------------------------
GeneratorRegistration PerlinNoiseGenerator::s_LakesRegistration( "Lakes", &PerlinNoiseGenerator::CreateGenerator, &Generator::CreateEnvironmentGenerationProcess );
GeneratorRegistration PerlinNoiseGenerator::s_LavaRegistration( "Lava", &PerlinNoiseGenerator::CreateGenerator, &Generator::CreateEnvironmentGenerationProcess );


//-----------------------------------------------------------------------------------------------
Map* PerlinNoiseGenerator::CreateInitializedMap( const IntVector2& size, const std::string& name )
{
	Map* map = new Map( size, name );

	int numCells = size.x * size.y;
	map->m_cells.resize( numCells );
	m_perlinNoiseForMapCells.resize( numCells );

	m_randX = RandomInt( 1000 );
	m_randY = RandomInt( 1000 );

	// Build the map out of cells, compute Perlin noise
	int cellIndex = 0;

	for ( int cellY = 0; cellY < size.y; cellY++ )
	{
		for ( int cellX = 0; cellX < size.x; cellX++ )
		{
			// Set cell position
			map->m_cells[ cellIndex ].m_position = IntVector2( cellX, cellY );

			// Set cell type
			map->m_cells[ cellIndex ].m_currentType = CELL_TYPE_STONE;

			// Compute Perlin noise for cell position
			m_perlinNoiseForMapCells[ cellIndex ] = Compute2dPerlinNoise( ( float ) ( cellX + m_randX ), ( float ) ( cellY + m_randY ), 20.0f, 2, 0.4f );

			cellIndex++;
		}
	}

	// Determine cell types based upon computed Perlin noise values
	for ( Cell& cell : map->m_cells )
	{
		if ( m_perlinNoiseForMapCells[ map->GetVectorIndexForCellLocation( cell.m_position ) ] <= -0.7f )
		{
			if ( m_name == "Lakes" )
			{
				cell.m_currentType = CELL_TYPE_WATER;
			}
			else if ( m_name == "Lava" )
			{
				cell.m_currentType = CELL_TYPE_LAVA;
			}
		}
	}

	// Generation complete

	map->SetSize( size );

	return map;
}


//-----------------------------------------------------------------------------------------------
bool PerlinNoiseGenerator::GenerateStep( Map* map, int currentStepNumber )
{
	currentStepNumber;
	m_perlinNoiseForMapCells.resize( map->m_cells.size() );

	if ( m_randX == 0 || m_randY == 0 )
	{
		m_randX = RandomInt( 1000 );
		m_randY = RandomInt( 1000 );
	}

	// Build the map out of cells, compute Perlin noise
	int cellIndex = 0;

	for ( int cellY = 0; cellY < map->m_mapSizeInCells.y; cellY++ )
	{
		for ( int cellX = 0; cellX < map->m_mapSizeInCells.x; cellX++ )
		{
			// Compute Perlin noise for cell position
			m_perlinNoiseForMapCells[ cellIndex ] = Compute2dPerlinNoise( ( float ) ( cellX + m_randX ), ( float ) ( cellY + m_randY ), 20.0f, 2, 0.4f );

			cellIndex++;
		}
	}

	// Determine cell types based upon computed Perlin noise values
	float threshold = -0.7f + ( ( currentStepNumber ) * 0.03f );
	threshold = ClampFloat( threshold, -1.0f, 1.0f );

	for ( Cell& cell : map->m_cells )
	{
		if ( m_perlinNoiseForMapCells[ map->GetVectorIndexForCellLocation( cell.m_position ) ] <= threshold )
		{
			if ( m_name == "Lakes" )
			{
				cell.m_currentType = CELL_TYPE_WATER;
			}
			else if ( m_name == "Lava" )
			{
				cell.m_currentType = CELL_TYPE_LAVA;
			}
		}
	}

	return true;
}