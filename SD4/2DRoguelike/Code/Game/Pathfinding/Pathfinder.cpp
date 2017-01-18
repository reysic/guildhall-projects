#include "Engine/Renderer/Renderer.hpp"
#include "Game/Pathfinding/Pathfinder.hpp"
#include "Game/Entities/Agent.hpp"


//-----------------------------------------------------------------------------------------------
Pathfinder::Pathfinder( const IntVector2& startPosition, const IntVector2& goalPosition, Map* map,
	Agent* owningAgent )
	: m_startPosition( startPosition )
	, m_goalPosition( goalPosition )
	, m_map( map )
	, m_isFinished( false )
	, m_owningAgent( owningAgent )
{
	PathNode* startNode = new PathNode( startPosition, nullptr, 0.0f, 0.0f );

	// AddNodeToOpenList( startNode );
	m_openList.push_back( startNode );
}


//-----------------------------------------------------------------------------------------------
bool Pathfinder::TakeOneStep()
{
	if ( m_openList.size() == 0 )
	{
		m_isFinished = true;
		return false;
	}

	// PathNode* activeNode = FindLowestFNodeOnOpenListAndRemoveIt();
	float lowestF = 1000.0f;
	unsigned int indexOfNodeToRemove = 0;
	for ( unsigned int nodeIndex = 0; nodeIndex < m_openList.size(); ++nodeIndex )
	{
		if ( m_openList[ nodeIndex ]->m_f < lowestF )
		{
			lowestF = m_openList[ nodeIndex ]->m_f;
			indexOfNodeToRemove = nodeIndex;
		}
	}

	/*PathNode* activeNode = m_openList[ indexOfNodeToRemove ];*/

	// Above line might need to be the below stuff?
	PathNode* activeNode = new PathNode( m_openList[ indexOfNodeToRemove ]->m_position,
		m_openList[ indexOfNodeToRemove ]->m_parent,
		m_openList[ indexOfNodeToRemove ]->m_g,
		m_openList[ indexOfNodeToRemove ]->m_h );

	m_openList.erase( m_openList.begin() + indexOfNodeToRemove );

	// AddToClosedList( activeNode )
	m_closedList.push_back( activeNode );

	// Are we at goal node?
	if ( activeNode->m_position == m_goalPosition )
	{
		// RecursivelyBuildPathToStartFromNode( active Node ); // Store in an out value
		PathNode* currentNode = activeNode;
		while ( currentNode != nullptr )
		{
			m_path.push_back( currentNode );
			currentNode = currentNode->m_parent;
		}
		m_isFinished = true;
		return true;
	}

	std::vector< IntVector2 > validPositions = m_map->GetValidAdjacentPositions( activeNode->m_position, m_owningAgent->GetMovementProperties() );

	for ( IntVector2& position : validPositions )
	{
		bool isPositionOnClosedList = false;
		for ( PathNode* node : m_closedList )
		{
			if ( node->m_position == position )
			{
				isPositionOnClosedList = true;
				break;
			}
		}

		if ( isPositionOnClosedList )
		{
			continue;
		}

		float localG = ComputeLocalG( activeNode->m_position, position );
		float parentG = activeNode->m_g;
		float h = ComputeEstimatedCostFromPosToPos( position, m_goalPosition );

		PathNode* existingNode = nullptr;
		for ( PathNode* node : m_openList )
		{
			if ( node->m_position == position )
			{
				existingNode = node;
				break;
			}
		}

		float totalG = localG + parentG;
		if ( existingNode != nullptr )
		{
			if ( ( totalG ) < existingNode->m_g )
			{
				existingNode->m_g = totalG;
				existingNode->m_f = totalG + existingNode->m_h;
				existingNode->m_parent = activeNode;
			}
			continue;
		}

		PathNode* newNode = new PathNode( position, activeNode, totalG, h );
		m_openList.push_back( newNode );
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
float Pathfinder::ComputeLocalG( const IntVector2& currentPosition, const IntVector2& nextPosition )
{
	float outLocalG = 1.0f;

	if ( !( currentPosition.x == nextPosition.x || currentPosition.y == nextPosition.y ) )
	{
		outLocalG += 0.4f;
		
	}

	CellType nextCellType = m_map->GetCellTypeAtPosition( nextPosition.x, nextPosition.y );

	switch ( nextCellType )
	{
		case CELL_TYPE_WATER:
		{

		}
		case CELL_TYPE_LAVA:
		{

		}
		default:
		{
			break;
		}
	}

	return outLocalG;
}


//-----------------------------------------------------------------------------------------------
float Pathfinder::ComputeEstimatedCostFromPosToPos( const IntVector2& currentPosition, const IntVector2& goalPosition )
{
	// Manhattan distance
	int differenceInY = abs( goalPosition.y - currentPosition.y );
	int differenceInX = abs( goalPosition.x - currentPosition.x );

	return ( ( float ) ( differenceInY + differenceInX ) );


	// Diagonal distance
	// return 1.0f * ( differenceInX + differenceInY ) + ( 1.4f - 2.0f * 1.0f ) * min( differenceInX, differenceInY );
}


//-----------------------------------------------------------------------------------------------
void Pathfinder::Render() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	float fontCellHeight = 15.0f;

	// Calculate offset based on map size
	float xOffset = ( 80.0f - m_map->m_mapWidth ) * 10.0f;
	float yOffset = ( 45.0f - m_map->m_mapHeight ) * 10.0f;
	Vector2 offset( xOffset, yOffset );

	Rgba greenOverlay = Rgba( 0, 255, 0, 127 );
	Rgba redOverlay = Rgba( 255, 0, 0, 127 );
	Rgba blueOverlay = Rgba( 0, 0, 255, 127 );

	IntVector2 currentCellPosition;
	Vector2 scaledCellPosition;
	Vector2 cellBottomLeft;

	if ( m_isFinished )
	{
		for ( PathNode* node : m_path )
		{
			currentCellPosition = node->m_position;
			scaledCellPosition = Vector2( ( float ) currentCellPosition.x * 20.0f, ( float ) currentCellPosition.y * 20.0f );
			cellBottomLeft = Vector2( offset + scaledCellPosition );
			g_theRenderer->DrawQuad( cellBottomLeft, Vector2( cellBottomLeft.x + fontCellHeight, cellBottomLeft.y + fontCellHeight ), greenOverlay );
		}
	}
	else
	{
		// Draw goal cell overlay
		scaledCellPosition = Vector2( ( float ) m_goalPosition.x * 20.0f, ( float ) m_goalPosition.y * 20.0f );
		cellBottomLeft = Vector2( offset + scaledCellPosition );
		g_theRenderer->DrawQuad( cellBottomLeft, Vector2( cellBottomLeft.x + fontCellHeight, cellBottomLeft.y + fontCellHeight ), greenOverlay );

		// Draw open list cell overlays
		for ( PathNode* node : m_openList )
		{
			currentCellPosition = node->m_position;
			scaledCellPosition = Vector2( ( float ) currentCellPosition.x * 20.0f, ( float ) currentCellPosition.y * 20.0f );
			cellBottomLeft = Vector2( offset + scaledCellPosition );
			g_theRenderer->DrawQuad( cellBottomLeft, Vector2( cellBottomLeft.x + fontCellHeight, cellBottomLeft.y + fontCellHeight ), blueOverlay );
		}

		// Draw closed list cell overlays
		for ( PathNode* node : m_closedList )
		{
			currentCellPosition = node->m_position;
			scaledCellPosition = Vector2( ( float ) currentCellPosition.x * 20.0f, ( float ) currentCellPosition.y * 20.0f );
			cellBottomLeft = Vector2( offset + scaledCellPosition );
			g_theRenderer->DrawQuad( cellBottomLeft, Vector2( cellBottomLeft.x + fontCellHeight, cellBottomLeft.y + fontCellHeight ), redOverlay );
		}
	}
}