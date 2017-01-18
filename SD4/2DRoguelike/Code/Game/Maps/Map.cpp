#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Fonts/BitmapFont.hpp"
#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Entities/Item.hpp"
#include "Game/Entities/Feature.hpp"
#include "Game/Entities/Agent.hpp"


//-----------------------------------------------------------------------------------------------
Map::Map( const IntVector2& mapSizeInCells, const std::string& name )
	: m_mapSizeInCells( mapSizeInCells )
	, m_name( name )
	, m_numCells( mapSizeInCells.x * mapSizeInCells.y )
	, m_currentCellIndex( 0 )
	, m_mapWidth( mapSizeInCells.x )
	, m_mapHeight( mapSizeInCells.y )
	, m_debugRender( false )
{
}


//-----------------------------------------------------------------------------------------------
void Map::Render() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	float fontCellHeight = 15.0f;

	// Calculate offset based on map size
	float xOffset = ( 95.0f - m_mapSizeInCells.x ) * 10.0f;
	float yOffset = ( 45.0f - m_mapSizeInCells.y ) * 10.0f;
	Vector2 bottomLeftOffset( xOffset, yOffset );

	// For rendering known but not visible cells
	Rgba overlayColor = Rgba( 0, 0, 0, 127 );

	// Draw the cells
	for ( const Cell& cell : m_cells )
	{
		IntVector2 currentCellPosition = cell.m_position;
		Vector2 cellBottomLeft( ( float ) currentCellPosition.x * 20.0f, ( float ) currentCellPosition.y * 20.0f );
		cellBottomLeft += m_renderingOffset;

		if ( cell.m_isHidden )
		{
			continue;
		}

		if ( m_debugRender )
		{
			if ( cell.m_containsFeature )
			{
				g_theRenderer->DrawText2D( cellBottomLeft, cell.m_containedFeature->m_glyph, fontCellHeight, cell.m_containedFeature->m_color, fixedFont );
				continue;
			}
			else if ( cell.m_currentType == CELL_TYPE_AIR )
			{
				g_theRenderer->DrawText2D( cellBottomLeft, ".", fontCellHeight, Rgba( 70, 70, 70, 255 ), fixedFont );
			}
			else if ( cell.m_currentType == CELL_TYPE_STONE )
			{
				g_theRenderer->DrawText2D( cellBottomLeft, "#", fontCellHeight, Rgba( 216, 151, 38, 255 ), fixedFont );
			}
			else if ( cell.m_currentType == CELL_TYPE_WATER )
			{
				g_theRenderer->DrawText2D( cellBottomLeft, "~", fontCellHeight, Rgba( 23, 89, 155, 255 ), fixedFont );
			}
			else if ( cell.m_currentType == CELL_TYPE_LAVA )
			{
				g_theRenderer->DrawText2D( cellBottomLeft, "~", fontCellHeight, Rgba::REDORANGE, fixedFont );
			}
		}
		else
		{
			// Render visible cells
			if ( cell.m_isVisible )
			{
				if ( cell.m_containsAgent || cell.m_containsItem )
					continue;

				if ( cell.m_containsFeature )
				{
					g_theRenderer->DrawText2D( cellBottomLeft, cell.m_containedFeature->m_glyph, fontCellHeight, cell.m_containedFeature->m_color, fixedFont );
					continue;
				}

				if ( cell.m_currentType == CELL_TYPE_AIR )
				{
					g_theRenderer->DrawText2D( cellBottomLeft, ".", fontCellHeight, Rgba( 70, 70, 70, 255 ), fixedFont );
				}
				else if ( cell.m_currentType == CELL_TYPE_STONE )
				{
					g_theRenderer->DrawText2D( cellBottomLeft, "#", fontCellHeight, Rgba( 216, 151, 38, 255 ), fixedFont );
				}
				else if ( cell.m_currentType == CELL_TYPE_WATER )
				{
					g_theRenderer->DrawText2D( cellBottomLeft, "~", fontCellHeight, Rgba( 23, 89, 155, 255 ), fixedFont );
				}
				else if ( cell.m_currentType == CELL_TYPE_LAVA )
				{
					g_theRenderer->DrawText2D( cellBottomLeft, "~", fontCellHeight, Rgba::REDORANGE, fixedFont );
				}
			}
			else
			{
				// Render known cells
				if ( cell.m_isKnown )
				{
					if ( cell.m_currentType == CELL_TYPE_AIR )
					{
						g_theRenderer->DrawText2D( cellBottomLeft, ".", fontCellHeight, Rgba( 70, 70, 70, 255 ), fixedFont );
						g_theRenderer->DrawQuad( cellBottomLeft, Vector2( cellBottomLeft.x + fontCellHeight, cellBottomLeft.y + fontCellHeight ), overlayColor );
					}
					else if ( cell.m_currentType == CELL_TYPE_STONE )
					{
						g_theRenderer->DrawText2D( cellBottomLeft, "#", fontCellHeight, Rgba( 216, 151, 38, 255 ), fixedFont );
						g_theRenderer->DrawQuad( cellBottomLeft, Vector2( cellBottomLeft.x + fontCellHeight, cellBottomLeft.y + fontCellHeight ), overlayColor );
					}
					else if ( cell.m_currentType == CELL_TYPE_WATER )
					{
						g_theRenderer->DrawText2D( cellBottomLeft, "~", fontCellHeight, Rgba( 23, 89, 155, 255 ), fixedFont );
						g_theRenderer->DrawQuad( cellBottomLeft, Vector2( cellBottomLeft.x + fontCellHeight, cellBottomLeft.y + fontCellHeight ), overlayColor );
					}
					else if ( cell.m_currentType == CELL_TYPE_LAVA )
					{
						g_theRenderer->DrawText2D( cellBottomLeft, "~", fontCellHeight, Rgba::REDORANGE, fixedFont );
						g_theRenderer->DrawQuad( cellBottomLeft, Vector2( cellBottomLeft.x + fontCellHeight, cellBottomLeft.y + fontCellHeight ), overlayColor );
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
int Map::GetVectorIndexForCellLocation( const IntVector2& location )
{
	return ( m_mapSizeInCells.x * location.y ) + location.x;
}


//-----------------------------------------------------------------------------------------------
int Map::GetVectorIndexForCellLocation( int cellX, int cellY )
{
	return ( m_mapSizeInCells.x * cellY ) + cellX;
}


//-----------------------------------------------------------------------------------------------
std::vector< Cell > Map::GetNeighborsInRange( const IntVector2& position, int range )
{
	std::vector< Cell > output;
	for ( int x = position.x - range; x <= position.x + range; x++ )
	{
		for ( int y = position.y - range; y <= position.y + range; y++ )
		{
			Cell thisCell = GetCellAtPosition( IntVector2( x, y ) );
			if ( IsPositionValid( thisCell.m_position ) && thisCell.m_position != position )
			{
				output.push_back( thisCell );
			}
		}
	}
	return output;
}


//-----------------------------------------------------------------------------------------------
std::vector< Cell > Map::GetNeighborsInRangeOfType( const IntVector2& position, int range, CellType type )
{
	std::vector< Cell > output;
	for ( int x = position.x - range; x <= position.x + range; x++ )
	{
		for ( int y = position.y - range; y <= position.y + range; y++ )
		{
			Cell thisCell = GetCellAtPosition( IntVector2( x, y ) );
			if ( IsPositionValid( thisCell.m_position ) && thisCell.m_position != position && thisCell.m_currentType == type )
			{
				output.push_back( thisCell );
			}
			else if ( !IsPositionValid( thisCell.m_position ) && thisCell.m_position != position && type == CELL_TYPE_INVALID )
			{
				output.push_back( thisCell );
			}
		}
	}
	return output;
}


//-----------------------------------------------------------------------------------------------
std::vector< IntVector2 > Map::GetValidAdjacentPositions( const IntVector2& position, int movementProperties )
{
	std::vector< IntVector2 > output;

	std::vector< Cell > neighborCells = GetNeighborsInRange( position, 1 );

	for ( Cell cell : neighborCells )
	{
		if ( cell.m_currentType != CELL_TYPE_STONE )
		{
			if ( cell.m_currentType == CELL_TYPE_WATER )
			{
				if ( !( ( movementProperties & MOVE_PROP_BLOCKED_BY_WATER ) != 0 ) )
				{
					output.push_back( cell.m_position );
				}
			}
			else if ( cell.m_currentType == CELL_TYPE_LAVA )
			{
				if ( !( ( movementProperties & MOVE_PROP_BLOCKED_BY_LAVA ) != 0 ) )
				{
					output.push_back( cell.m_position );
				}
			}
			else
			{
				output.push_back( cell.m_position );
			}
		}
	}

	return output;
}


//-----------------------------------------------------------------------------------------------
Cell& Map::GetCellAtPosition( const IntVector2& position )
{
	if ( IsPositionValid( position ) )
	{
		int index = GetVectorIndexForCellLocation( position.x, position.y );
		return m_cells[ index ];
	}
	else
	{
		Cell* cell = new Cell();
		cell->m_position = position;
		cell->m_currentType = CELL_TYPE_INVALID;
		return *cell;
	}
}


//-----------------------------------------------------------------------------------------------
CellType Map::GetCellTypeAtPosition( int cellX, int cellY )
{
	if ( IsPositionValid( cellX, cellY ) )
	{
		int index = GetVectorIndexForCellLocation( cellX, cellY );
		return m_cells[ index ].m_currentType;
	}
	return CELL_TYPE_INVALID;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsPositionValid( int cellX, int cellY )
{
	IntVector2 pos = IntVector2( cellX, cellY );
	return IsPositionValid( pos );
}


//-----------------------------------------------------------------------------------------------
bool Map::IsPositionValid( const IntVector2& position )
{
	if ( position.x >= m_mapSizeInCells.x || position.x < 0 )
		return false;
	if ( position.y >= m_mapSizeInCells.y || position.y < 0 )
		return false;
	return true;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsAreaValid( const IntAABB2& area )
{
	if ( area.mins.x >= m_mapSizeInCells.x || area.mins.x < 0
		|| area.maxs.x >= m_mapSizeInCells.x || area.maxs.x < 0 )
		return false;
	if ( area.mins.y >= m_mapSizeInCells.x || area.mins.y < 0
		|| area.maxs.y >= m_mapSizeInCells.x || area.maxs.y < 0 )
		return false;
	return true;
}


//-----------------------------------------------------------------------------------------------
bool Map::DoesAreaContainCellType( const IntAABB2& area, CellType type )
{
	bool containsType = false;
	for ( int y = area.mins.y; y <= area.maxs.y; y++ )
	{
		for ( int x = area.mins.x; x <= area.maxs.x; x++ )
		{
			Cell thisCell = GetCellAtPosition( IntVector2( x, y ) );
			// If cell is invalid, continue
			if ( IsCellValid( thisCell ) )
			{
				if ( IsCellOfType( thisCell, type ) )
					containsType = true;
			}
			else
			{
				continue;
			}
		}
	}
	return containsType;
}


//-----------------------------------------------------------------------------------------------
void Map::SetAreaCellType( const IntAABB2& area, CellType type )
{
	for ( int y = area.mins.y; y <= area.maxs.y; y++ )
	{
		for ( int x = area.mins.x; x <= area.maxs.x; x++ )
		{
			Cell thisCell = GetCellAtPosition( IntVector2( x, y ) );

			// If cell is invalid, continue
			if ( IsCellValid( thisCell ) )
			{
				SetCellAsType( thisCell, type );
			}
			else
			{
				continue;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Map::IsCellValid( Cell cell )
{
	return IsPositionValid( cell.m_position );
}


//-----------------------------------------------------------------------------------------------
void Map::SetCellAsType( Cell cell, CellType type )
{
	if ( IsCellValid( cell ) )
	{
		int index = GetVectorIndexForCellLocation( cell.m_position );
		SetCellAsTypeAtIndex( index, type );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::SetCellAsTypeAtIndex( int index, CellType type )
{
	m_cells[ index ].m_currentType = type;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsCellOfType( Cell cell, CellType type )
{
	if ( IsCellValid( cell ) )
	{
		return ( cell.m_currentType == type );
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Map::HasLineOfSight( const Vector2& start, const Vector2& end, bool updateCellVisibility )
{
	float stepCount = 100.0f;

	Vector2 displacementVector = end - start;
	Vector2 displacementVectorChunk = displacementVector * ( 1.0f / stepCount );

	Vector2 currentPosition = start;
	IntVector2 cellPosition;
	unsigned int cellIndex;

	for ( int displacementIndex = 0; displacementIndex < stepCount; displacementIndex++ )
	{
		currentPosition += displacementVectorChunk;

		cellPosition = GetCellPositionForWorldPosition( currentPosition );

		if ( IsPositionValid( cellPosition ) )
		{
			cellIndex = GetVectorIndexForCellLocation( cellPosition );
			Feature* cellFeature = m_cells[ cellIndex ].m_containedFeature;

			if ( m_cells[ cellIndex ].m_currentType == CELL_TYPE_STONE )
			{
				if ( updateCellVisibility )
				{
					m_cells[ cellIndex ].m_isVisible = true;
					m_cells[ cellIndex ].m_isKnown = true;
				}
				return false;
			}
			else if ( cellFeature != nullptr )
			{
				if ( cellFeature->IsBlockingLineOfSight() )
				{
					if ( updateCellVisibility )
					{
						m_cells[ cellIndex ].m_isVisible = true;
						m_cells[ cellIndex ].m_isKnown = true;
					}
					return false;
				}
			}
		}

	}

	return true;
}


//-----------------------------------------------------------------------------------------------
IntVector2 Map::GetCellPositionForWorldPosition( const Vector2& position )
{
	IntVector2 cellPosition;
	cellPosition.x = ( int ) floor( position.x );
	cellPosition.y = ( int ) floor( position.y );
	return cellPosition;
}


//-----------------------------------------------------------------------------------------------
void Map::SetAllCellsAsNotVisible()
{
	for ( Cell& cell : m_cells )
	{
		cell.m_isVisible = false;
	}
}


//-----------------------------------------------------------------------------------------------
void Map::SetSize( const IntVector2& size )
{
	m_mapWidth = size.x;
	m_mapHeight = size.y;

	float xOffset = ( 95.0f - m_mapWidth ) * 10.0f;
	float yOffset = ( 45.0f - m_mapHeight ) * 10.0f;

	Vector2 bottomLeftOffset( xOffset, yOffset );
	m_renderingOffset = bottomLeftOffset;
}


//-----------------------------------------------------------------------------------------------
IntVector2 Map::GetRandomAirCellPosition()
{
	bool foundCell = false;

	int xPosition;
	int yPosition;
	IntVector2 cellPosition;

	while ( !foundCell )
	{
		xPosition = RandomInt( m_mapWidth, 0 );
		yPosition = RandomInt( m_mapHeight, 0 );
		cellPosition = IntVector2( xPosition, yPosition );

		if ( GetCellAtPosition( cellPosition ).m_currentType == CELL_TYPE_AIR )
		{
			foundCell = true;
		}
	}

	return cellPosition;
}


//-----------------------------------------------------------------------------------------------
Direction Map::GetRandomDirection()
{
	int index = RandomInt( 7, 0 );
	return ( Direction ) index;
}


//-----------------------------------------------------------------------------------------------
bool Map::AreCellPositionsAdjacent( const IntVector2& position1, const IntVector2& position2 )
{
	if ( position1.x >= ( position2.x - 1 )
		&& position1.x <= ( position2.x + 1 )
		&& position1.y >= ( position2.y - 1 )
		&& position1.y <= ( position2.y + 1 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
bool Map::DoesCellPositionContainAgent( const IntVector2& position )
{
	Cell& thisCell = GetCellAtPosition( position );

	if ( IsPositionValid( thisCell.m_position) )
	{
		return thisCell.m_containsAgent;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
float Map::GetDistanceBetweenCellPositions( const IntVector2& position1, const IntVector2& position2 )
{
	// Returns distance from center of cell at position1 to center of cell at position2
	Vector2 worldPosition1( position1.x + 0.5f, position1.y + 0.5f );
	Vector2 worldPosition2( position2.x + 0.5f, position2.y + 0.5f );

	return CalculateDistanceBetweenPoints( worldPosition1, worldPosition2 );
}


//-----------------------------------------------------------------------------------------------
void Map::RemoveAgentAtCellPosition( const IntVector2& position )
{
	Cell& thisCell = GetCellAtPosition( position );
	thisCell.m_containsAgent = false;
	thisCell.m_containedAgent = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Map::AddItemToCellPosition( const IntVector2& position, Item* itemToAdd )
{
	Cell& thisCell = GetCellAtPosition( position );
	if ( IsPositionValid( thisCell.m_position ) )
	{
		itemToAdd->m_position = position;
		itemToAdd->m_map = this;
		thisCell.m_containedItems.push_back( itemToAdd );
		thisCell.m_containsItem = true;
	}
}


//-----------------------------------------------------------------------------------------------
void Map::RemoveFirstItemAtCellPosition( const IntVector2& position )
{
	Cell& thisCell = GetCellAtPosition( position );
	if ( IsPositionValid( thisCell.m_position ) && thisCell.m_containsItem )
	{
		if ( thisCell.m_containedItems.size() > 0 )
		{

			thisCell.m_containedItems.erase( thisCell.m_containedItems.begin() );

			if ( thisCell.m_containedItems.empty() )
			{
				thisCell.m_containsItem = false;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
XMLNode Map::SaveToXML()
{
	XMLNode mapDataNode = XMLNode();
	mapDataNode = mapDataNode.createXMLTopNode( "MapData" );

	XMLNode cellDataNode = XMLNode();
	cellDataNode = cellDataNode.createXMLTopNode( "CellData" );

	XMLNode visibilityDataNode = XMLNode();
	visibilityDataNode = visibilityDataNode.createXMLTopNode( "VisibilityData" );

	std::string size = IntVector2( m_mapWidth, m_mapHeight ).ToString();
	mapDataNode.addAttribute( "size", size.c_str() );

	// Build CellData
	std::string cellDataStr = "\n";
	std::string visDataStr = "\n";

	for ( int cellY = m_mapHeight - 1; cellY >= 0; cellY-- )
	{
		// Process a line
		for ( int cellX = 0; cellX < m_mapWidth; cellX++ )
		{
			IntVector2 cellXY = IntVector2( cellX, cellY );
			Cell thisCell = GetCellAtPosition( cellXY );
			std::string xmlGlyph = GetXMLGlyphForCell( thisCell );
			cellDataStr += xmlGlyph;

			if ( thisCell.m_isVisible )
			{
				visDataStr += "*";
			}
			else if ( thisCell.m_isKnown )
			{
				visDataStr += ".";
			}
			else
			{
				visDataStr += "#";
			}
		}
		cellDataStr += "\n";
		visDataStr += "\n";
	}

	cellDataNode.addText( cellDataStr.c_str() );

	visibilityDataNode.addText( visDataStr.c_str() );

	mapDataNode.addChild( cellDataNode );
	mapDataNode.addChild( visibilityDataNode );

	return mapDataNode;
}


//-----------------------------------------------------------------------------------------------
std::string Map::GetXMLGlyphForCell( Cell cell )
{
	CellType type = cell.m_currentType;

	switch ( type )
	{
	case CELL_TYPE_INVALID:
		return "?";
		break;
	case CELL_TYPE_AIR:
		return ".";
		break;
	case CELL_TYPE_STONE:
		return "#";
		break;
	case CELL_TYPE_WATER:
		return "~";
		break;
	case CELL_TYPE_LAVA:
		return "=";
		break;
	case NUM_CELL_TYPES:
		return "?";
		break;
	default:
		return "?";
		break;
	}
}


//-----------------------------------------------------------------------------------------------
Direction Map::GetDirectionToCellPositionFromCellPosition( const IntVector2& toCellPosition, 
	const IntVector2& fromCellPosition )
{
	if ( !IsPositionValid( toCellPosition ) )
	{
		return DIRECTION_INVALID;
	}

	if ( toCellPosition.x == fromCellPosition.x )
	{
		if ( toCellPosition.y > fromCellPosition.y )
		{
			return DIRECTION_NORTH;
		}

		if ( toCellPosition.y < fromCellPosition.y )
		{
			return DIRECTION_SOUTH;
		}

		return DIRECTION_INVALID;
	}

	if ( toCellPosition.y == fromCellPosition.y )
	{
		if ( toCellPosition.x > fromCellPosition.x )
		{
			return DIRECTION_EAST;
		}

		if ( toCellPosition.x < fromCellPosition.x )
		{
			return DIRECTION_WEST;
		}

		return DIRECTION_INVALID;
	}

	if ( toCellPosition.x > fromCellPosition.x )
	{
		if ( toCellPosition.y > fromCellPosition.y )
		{
			return DIRECTION_NORTHEAST;
		}

		if ( toCellPosition.y < fromCellPosition.y )
		{
			return DIRECTION_SOUTHEAST;
		}

		return DIRECTION_INVALID;
	}

	if ( toCellPosition.x < fromCellPosition.x )
	{
		if ( toCellPosition.y > fromCellPosition.y )
		{
			return DIRECTION_NORTHWEST;
		}

		if ( toCellPosition.y < fromCellPosition.y )
		{
			return DIRECTION_SOUTHWEST;
		}

		return DIRECTION_INVALID;
	}

	return DIRECTION_INVALID;
}


//-----------------------------------------------------------------------------------------------
Direction Map::GetOppositeDirection( Direction inDirection )
{
	switch ( inDirection )
	{
	case DIRECTION_NORTH:
		return DIRECTION_SOUTH;
		break;
	case DIRECTION_SOUTH:
		return DIRECTION_NORTH;
		break;
	case DIRECTION_EAST:
		return DIRECTION_WEST;
		break;
	case DIRECTION_WEST:
		return DIRECTION_EAST;
		break;
	case DIRECTION_NORTHEAST:
		return DIRECTION_SOUTHWEST;
		break;
	case DIRECTION_SOUTHWEST:
		return DIRECTION_NORTHEAST;
		break;
	case DIRECTION_NORTHWEST:
		return DIRECTION_SOUTHEAST;
		break;
	case DIRECTION_SOUTHEAST:
		return DIRECTION_NORTHWEST;
		break;
	default:
		return DIRECTION_INVALID;
		break;
	}
}

//-----------------------------------------------------------------------------------------------
Item* Map::GetFirstItemAtCellPosition( const IntVector2& position )
{
	Item* firstItem = nullptr;
	Cell& thisCell = GetCellAtPosition( position );
	if ( IsPositionValid( thisCell.m_position) && thisCell.m_containsItem )
	{
		if ( thisCell.m_containedItems.size() > 0 )
		{
			firstItem = thisCell.m_containedItems[ 0 ];
		}
	}

	return firstItem;
}