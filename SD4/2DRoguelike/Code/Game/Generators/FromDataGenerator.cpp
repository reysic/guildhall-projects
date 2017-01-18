#include <iostream>
#include <sstream>

#include "Engine/Tools/Parsers/xmlParser.h"
#include "Game/Generators/FromDataGenerator.hpp"



//-----------------------------------------------------------------------------------------------
GeneratorRegistration FromDataGenerator::s_FDGRegistration( "FromData", &FromDataGenerator::CreateGenerator, &Generator::CreateEnvironmentGenerationProcess );


//-----------------------------------------------------------------------------------------------
Map* FromDataGenerator::CreateInitializedMap( const IntVector2& size, const std::string& name )
{
	UNUSED( size );

	// Read in XML
	// Data/XML/Map.xml

	XMLNode node = XMLNode::openFileHelper( "Data/XML/Map.xml" );
	XMLNode rootNode = node.getChildNode( 0 );

	if ( !rootNode.IsContentEmpty() )
	{
		XMLNode cellDataNode = rootNode.getChildNode( "CellData" );
		std::string cellNodeStr = cellDataNode.getText();
		std::vector< std::string > cellNodeStrLines;

		std::stringstream fullCellNodeStr( cellNodeStr );

		std::string token;
		while ( std::getline( fullCellNodeStr, token, '\n' ) ) 
		{
			unsigned int returnIndex = token.find( '\r' );

			if ( returnIndex != std::string::npos )
			{
				token.erase( returnIndex );
			}

			cellNodeStrLines.push_back( token );
		}

		// Generate map from input

		// Find longest row length
		unsigned int longestRowLength = 0;
		for ( unsigned int i = 0; i < cellNodeStrLines.size(); i++ )
		{
			if ( cellNodeStrLines[ i ].size() > longestRowLength )
			{
				longestRowLength = cellNodeStrLines[ i ].size();
			}
		}

		// Pad any short rows
		for ( unsigned int i = 0; i < cellNodeStrLines.size(); i++ )
		{
			if ( cellNodeStrLines[ i ].size() < longestRowLength )
			{
				while ( cellNodeStrLines[ i ].size() < longestRowLength )
				{
					cellNodeStrLines[ i ].append( std::string( "X" ) );
				}
			}
		}

		int mapWidth = longestRowLength;
		int mapHeight = cellNodeStrLines.size(); // Assumes columns of same length

		Map* map = new Map( IntVector2( mapWidth, mapHeight ), name );

		int numCells = mapWidth * mapHeight;
		map->m_cells.resize( numCells );

		// Build the map out of cells
		int cellIndex = 0;
		int lineIndex = mapHeight - 1; // Starting line in CellNodeStrLines
		int charIndex = 0; // Starting index in a line from CellNodeStrLines

		for ( int cellY = 0; cellY < mapHeight; cellY++ )
		{
			// Process a line
			for ( int cellX = 0; cellX < mapWidth; cellX++ )
			{

				// Set cell position
				map->m_cells[ cellIndex ].m_position = IntVector2( cellX, cellY );

				// Set cell type
				if ( cellNodeStrLines[ lineIndex ].at( charIndex ) == '.' )
				{
					map->m_cells[ cellIndex ].m_currentType = CELL_TYPE_AIR;
				}
				else if ( cellNodeStrLines[ lineIndex ].at( charIndex ) == '~' )
				{
					map->m_cells[ cellIndex ].m_currentType = CELL_TYPE_WATER;
				}
				else
				{
					map->m_cells[ cellIndex ].m_currentType = CELL_TYPE_STONE;
				}

				cellIndex++;
				charIndex++;
			}

			// Set charIndex back to 0 before grabbing next line
			charIndex = 0;
			
			// Decrement lineIndex before grabbing next line
			lineIndex--;
		}

		// Generation complete
		map->SetSize( size );

		return map;

	}
	else
	{
		ASSERT_OR_DIE( false, "XML file empty!" );
	}
}


//-----------------------------------------------------------------------------------------------
bool FromDataGenerator::GenerateStep( Map* map, int currentStepNumber )
{
	UNUSED( map );
	UNUSED( currentStepNumber );
	return false;
}