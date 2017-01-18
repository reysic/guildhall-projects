#include "Map.hpp"
#include "Engine/ErrorWarningAssert.hpp"
#include "Engine/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
bool g_debugVerbose = false;


//-----------------------------------------------------------------------------------------------
// Globals
//
const IntVector2 STEP_EAST( 1, 0 );
const IntVector2 STEP_WEST( -1, 0 );
const IntVector2 STEP_NORTH( 0, 1 );
const IntVector2 STEP_SOUTH( 0, -1 );


//-----------------------------------------------------------------------------------------------
PathNode::PathNode( const IntVector2& tileCoords, PathNode* parent, float gCostCumulative, float hDistToEndGuess )
	: m_tileCoords( tileCoords )
	, m_parent( parent )
	, m_gCostCumulative( gCostCumulative )
	, m_hDistToEndGuess( hDistToEndGuess )
{
}


//-----------------------------------------------------------------------------------------------
Map::Map( int width, int height )
	: m_width( width )
	, m_height( height )
{
	int numTiles = m_width * m_height;
	for( int tileIndex = 0; tileIndex < numTiles; ++ tileIndex )
	{
		MapTile* newTile = new MapTile( TILE_TYPE_UNKNOWN );
		m_tiles.push_back( newTile );
	}

	Generate();
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
}


//-----------------------------------------------------------------------------------------------
void Map::Generate()
{
	int numTiles = (int) m_tiles.size();
	GUARANTEE_OR_DIE( numTiles == m_width * m_height, Stringf( "%i x %i Map should have %i tiles (had %i)!", m_width, m_height, m_width * m_height, numTiles ) );

	// 1. Fill with a mix of tiles
	for( int tileIndex = 0; tileIndex < numTiles; ++ tileIndex )
	{
		TileType newType = TILE_TYPE_FLAT;

		if( GetRandomChance( .20f ) )
			newType = TILE_TYPE_GRASS;

		if( GetRandomChance( .03f ) )
			newType = TILE_TYPE_MUD;

		if( GetRandomChance( .02f ) )
			newType = TILE_TYPE_WALL;

		ChangeTileType( tileIndex, newType );
	}

	// 2. Let MUD and WALL tiles "grow" by picking random tiles to become MUD/WALL if adjacent to existing MUD/WALL
	int numMudSpreadAttempts = numTiles;
	for( int i = 0; i < numMudSpreadAttempts; ++ i )
	{
		// Pick a random tile location
		int tileX = GetRandomIntInRange( 1, m_width-2 ); // Pick random "interior" tile
		int tileY = GetRandomIntInRange( 1, m_height-2 );
		int tileIndex = GetTileIndexForCoords( tileX, tileY );
		if( IsTileAdjacentTo( tileIndex, TILE_TYPE_WALL ) )
		{
			ChangeTileType( tileIndex, TILE_TYPE_WALL );
		}
		else if( IsTileAdjacentTo( tileIndex, TILE_TYPE_MUD ) )
		{
			ChangeTileType( tileIndex, TILE_TYPE_MUD );
		}
	}

	// 3. Put WALLs around the outer edge
	for( int tileX = 0; tileX < m_width; ++ tileX )
	{
		ChangeTileType( tileX, 0, TILE_TYPE_WALL );
		ChangeTileType( tileX, m_height-1, TILE_TYPE_WALL );
	}

	for( int tileY = 0; tileY < m_height; ++ tileY )
	{
		ChangeTileType( 0, tileY, TILE_TYPE_WALL );
		ChangeTileType( m_width-1, tileY, TILE_TYPE_WALL );
	}
}


//-----------------------------------------------------------------------------------------------
IntVector2 Map::GetRandomInteriorNonSolidCoords() const
{
	// Loop forever until we find an interior (non-edge) location which is not solid
	for( ;; )
	{
		int tileX = GetRandomIntInRange( 1, m_width-2 );
		int tileY = GetRandomIntInRange( 1, m_height-2 );
		IntVector2 tileCoords( tileX, tileY );
		if( !IsTileSolid( tileCoords ) )
			return tileCoords;
	}
}

//-----------------------------------------------------------------------------------------------
void Map::ChangeTileType( int tileIndex, TileType newType )
{
	delete m_tiles[ tileIndex ];
	m_tiles[ tileIndex ] = new MapTile( newType );
}


//-----------------------------------------------------------------------------------------------
void Map::ChangeTileType( int tileX, int tileY, TileType newType )
{
	int tileIndex = GetTileIndexForCoords( tileX, tileY );
	ChangeTileType( tileIndex, newType );
}


//-----------------------------------------------------------------------------------------------
int Map::GetTileIndexForCoords( int tileX, int tileY ) const
{
	return tileX + (tileY * m_width);
}


//-----------------------------------------------------------------------------------------------
IntVector2 Map::GetTileCoordsForIndex( int tileIndex ) const
{
	int tileX = tileIndex % m_width;
	int tileY = tileIndex / m_width;
	return IntVector2( tileX, tileY );
}


//-----------------------------------------------------------------------------------------------
int Map::CountAdjacentTilesOfType( int tileIndex, TileType neighborTypeToCount ) const
{
	IntVector2 tileCoords = GetTileCoordsForIndex( tileIndex );
	TileType eastType	= GetTileTypeAtCoords( tileCoords + STEP_EAST );
	TileType westType	= GetTileTypeAtCoords( tileCoords + STEP_WEST );
	TileType northType	= GetTileTypeAtCoords( tileCoords + STEP_NORTH );
	TileType southType	= GetTileTypeAtCoords( tileCoords + STEP_SOUTH );

	int count = 0;
	count += (eastType == neighborTypeToCount) ? 1 : 0;
	count += (westType == neighborTypeToCount) ? 1 : 0;
	count += (northType == neighborTypeToCount) ? 1 : 0;
	count += (southType == neighborTypeToCount) ? 1 : 0;

	return count;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileAdjacentTo( int tileIndex, TileType checkForNeighborType ) const
{
	int count = CountAdjacentTilesOfType( tileIndex, checkForNeighborType );
	return count > 0;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileSolid( const IntVector2& tileCoords ) const
{
	TileType tileType = GetTileTypeAtCoords( tileCoords );
	return MapTile::IsTileTypeSolid( tileType );
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileOnEdge( int tileIndex ) const
{
	int tileX = tileIndex % m_width;
	if( tileX == 0 || tileX == m_width-1 )
		return true;

	int tileY = tileIndex / m_width;
	if( tileY == 0 || tileY == m_height-1 )
		return true;

	return false;
}


//-----------------------------------------------------------------------------------------------
TileType Map::GetTileTypeAtCoords( const IntVector2& tileCoords ) const
{
	if( tileCoords.x < 0 || tileCoords.x >= m_width || tileCoords.y < 0 || tileCoords.y >= m_height )
		return TILE_TYPE_UNKNOWN;

	int tileIndex = GetTileIndexForCoords( tileCoords.x, tileCoords.y );
	return m_tiles[ tileIndex ]->m_type;
}


//-----------------------------------------------------------------------------------------------
char Map::GetGlyphForTile( int tileX, int tileY ) const
{
	TileType tileType = GetTileTypeAtCoords( IntVector2( tileX, tileY ) );
	char tileGlyph = MapTile::GetGlyphForTileType( tileType );
	return tileGlyph;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsPositionInList( int tileX, int tileY, const TileCoordsList& tileList ) const
{
	int numTilesInList = (int) tileList.size();
	for( int i = 0; i < numTilesInList; ++ i )
	{
		const IntVector2& tileCoords = tileList[ i ];
		if( (tileCoords.x == tileX) && (tileCoords.y == tileY) )
			return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
void Map::PrintMap( const TileCoordsList& specialPositionsToMark ) const
{
	std::string rowString;

	// Print row headers across top
	rowString = "    ";
	for( int colX = 0; colX < m_width; ++ colX )
	{
		char glyph = ' ';
		if( colX % 10 == 0 )
		{
			int tens = (colX / 10) % 10;
			glyph = '0' + (char) tens;
		}

		rowString += glyph;
	}
	printf( "%s\n", rowString.c_str() );
	rowString = "    ";
	for( int colX = 0; colX < m_width; ++ colX )
	{
		rowString += (colX % 10) + '0';
	}
	printf( "%s\n", rowString.c_str() );

	// Print map rows
	for( int rowY = m_height-1; rowY >= 0; -- rowY )
	{
		rowString = Stringf( "%3i ", rowY );
		for( int colX = 0; colX < m_width; ++ colX )
		{
			unsigned char tileGlyph = GetGlyphForTile( colX, rowY );
			if( IsPositionInList( colX, rowY, specialPositionsToMark ) )
				tileGlyph = ASCII_SOLID_SHADED_BLOCK;

			rowString += tileGlyph;
		}
		rowString += Stringf( " %-3i", rowY );
		printf( "%s\n", rowString.c_str() );
	}

	// Print row footer across bottom
	rowString = "    ";
	for( int colX = 0; colX < m_width; ++ colX )
	{
		rowString += (colX % 10) + '0';
	}
	printf( "%s\n", rowString.c_str() );
	rowString = "    ";
	for( int colX = 0; colX < m_width; ++ colX )
	{
		char glyph = ' ';
		if( colX % 10 == 0 )
		{
			int tens = (colX / 10) % 10;
			glyph = '0' + (char) tens;
		}

		rowString += glyph;
	}
	printf( "%s\n", rowString.c_str() );
}


//-----------------------------------------------------------------------------------------------
float Map::GetGCostForTileTypeAt( const IntVector2& tileCoords ) const
{
	TileType tileType = GetTileTypeAtCoords( tileCoords );
	return MapTile::GetGCostForTileType( tileType );
}


//-----------------------------------------------------------------------------------------------
float Map::GuessEstimatedCost( const IntVector2& fromHere, const IntVector2& toHere ) const
{
	int xDist = abs( toHere.x - fromHere.x );
	int yDist = abs( toHere.y - fromHere.y );
	return (float)( xDist + yDist ); // simple Manhattan distance, since we're moving 4-way cardinal on 2D tile grid
}


//-----------------------------------------------------------------------------------------------
void DeleteNodesInSet( PathNodeList& setOfNodes )
{
	for( PathNode* pathNode : setOfNodes )
	{
		delete pathNode;
	}

	setOfNodes.clear();
}


//-----------------------------------------------------------------------------------------------
// Starts at <endNode> and works backward along the parent-chain to <startNode> (null parent).
//
// We actually prefer our paths in this backward format, since we can just pop each step in
//	the path off the back of the list when we reach it.
//
void BuildBackwardsPath( const PathNode* endNode, TileCoordsList& out_path )
{
	const PathNode* currentNode = endNode;
	while( currentNode )
	{
		out_path.push_back( currentNode->m_tileCoords );
		currentNode = currentNode->m_parent;
	}
}


//-----------------------------------------------------------------------------------------------
PathNode* FindTileInNodeList( const IntVector2& tileCoords, PathNodeList& nodeList )
{
	for( PathNode* node : nodeList )
	{
		if( node->m_tileCoords == tileCoords )
			return node;
	}

	return nullptr;
}


//-----------------------------------------------------------------------------------------------
bool IsSecondNodeBetter( PathNode* first, PathNode* second )
{
	if( !first )
		return true;

	if( !second )
		return false;

	if( second->GetF() < first->GetF() )
		return true;

	if( first->GetF() < second->GetF() )
		return false;

	if( second->m_hDistToEndGuess < first->m_hDistToEndGuess )
		return true;

	if( first->m_hDistToEndGuess < second->m_hDistToEndGuess )
		return false;

	return false;
}


//-----------------------------------------------------------------------------------------------
PathNodeList::iterator FindBestNodeInList( PathNodeList& nodeList )
{
	PathNodeList::iterator bestIter = nodeList.end();
	PathNode* best = nullptr;
	for( PathNodeList::iterator nodeIter = nodeList.begin(); nodeIter != nodeList.end(); ++ nodeIter )
	{
		PathNode* node = *nodeIter;
		if( IsSecondNodeBetter( best, node ) )
		{
			best = node;
			bestIter = nodeIter;
		}
	}

	return bestIter;
}


//-----------------------------------------------------------------------------------------------
void Map::OpenNeighborAsAppropriate( const IntVector2& neighborCoords, PathNode* parent, PathNodeList& openNodes, PathNodeList& closedNodes, const IntVector2& goalCoords ) const
{
	if( g_debugVerbose ) printf( "DEBUG:     Considering neighbor (%i,%i) from parent (%i,%i)...\n", neighborCoords.x, neighborCoords.y, parent->m_tileCoords.x, parent->m_tileCoords.y );

	TileType neighborType = GetTileTypeAtCoords( neighborCoords );
	if( MapTile::IsTileTypeSolid( neighborType ) )
	{
		if( g_debugVerbose ) printf( "DEBUG:       Skipping - neighbor (%i,%i) is solid\n", neighborCoords.x, neighborCoords.y );
		return; // Don't open inaccessible tiles
	}

	if( FindTileInNodeList( neighborCoords, closedNodes ) )
	{
		if( g_debugVerbose ) printf( "DEBUG:       Skipping - neighbor (%i,%i) is already CLOSED\n", neighborCoords.x, neighborCoords.y );
		return; // Don't reopen already-closed tiles
	}

	float gCostForNeighbor = MapTile::GetGCostForTileType( neighborType );
	float gCostToNeighborCumulative = parent->m_gCostCumulative + gCostForNeighbor;
	if( g_debugVerbose ) printf( "DEBUG:       Neighbor (%i,%i) adds gCost=%.0f, gCum=%.0f\n", neighborCoords.x, neighborCoords.y, gCostForNeighbor, gCostToNeighborCumulative );

	PathNode* alreadyOpenNeighbor = FindTileInNodeList( neighborCoords, openNodes );
	if( alreadyOpenNeighbor )
	{
		if( g_debugVerbose ) printf( "DEBUG:       Neighbor (%i,%i) is already open!\n", neighborCoords.x, neighborCoords.y );

		// This tile is already open; check if we've discovered a cheaper way to reach it
		if( gCostToNeighborCumulative < alreadyOpenNeighbor->m_gCostCumulative )
		{
			if( g_debugVerbose ) printf( "DEBUG:     Changing neighbor (%i,%i) to gCum=%.0f (was %.0f), parent to (%i,%i), was (%i,%i).\n",
				neighborCoords.x, neighborCoords.y,
				gCostToNeighborCumulative, alreadyOpenNeighbor->m_gCostCumulative,
				parent->m_tileCoords.x, parent->m_tileCoords.y,
				alreadyOpenNeighbor->m_parent->m_tileCoords.x, alreadyOpenNeighbor->m_parent->m_tileCoords.y );

			// We have found a cheaper way!  Update the cheaper cost and set us as parent.
			alreadyOpenNeighbor->m_gCostCumulative = gCostToNeighborCumulative;
			alreadyOpenNeighbor->m_parent = parent;
			return;
		}
		else
		{
			if( g_debugVerbose ) printf( "DEBUG:     Leaving neighbor (%i,%i) at gCum=%.0f, since our new-route gCost=%.0f is worse.\n",
				neighborCoords.x, neighborCoords.y,
				alreadyOpenNeighbor->m_gCostCumulative, gCostToNeighborCumulative );
		}
	}
	else
	{
		// This tile has never been opened before; open it!
		float hDistToEndGuess = GuessEstimatedCost( neighborCoords, goalCoords );
		PathNode* newNeighborNode = new PathNode( neighborCoords, parent, gCostToNeighborCumulative, hDistToEndGuess );
		openNodes.push_back( newNeighborNode );
		if( g_debugVerbose ) printf( "DEBUG:     Created new PathNode for (%i,%i) with g=%.0f, h=%.0f, f=%.0f; added to open set (%i now open)\n",
			neighborCoords.x, neighborCoords.y, newNeighborNode->m_gCostCumulative, newNeighborNode->m_hDistToEndGuess, newNeighborNode->m_gCostCumulative + newNeighborNode->m_hDistToEndGuess, (int) openNodes.size() );
	}
}


//-----------------------------------------------------------------------------------------------
// Adds a new PathNode for each cardinal (NSEW) neighboring tile to the open set, unless:
//	1. it is solid;
//	2. it is already in the closed set.
//	3. it is already in the open set, in which case update its gCost and parent IF we've found a cheaper route to it.
// 
void Map::OpenNeighborsAsAppropriate( PathNode* node, PathNodeList& openNodes, PathNodeList& closedNodes, const IntVector2& goalCoords ) const
{
	const IntVector2& myCoords = node->m_tileCoords;
	if( g_debugVerbose ) printf( "DEBUG:   Considering neighbors for (%i,%i)...\n", myCoords.x, myCoords.y );

	const IntVector2 eastCoords = myCoords + STEP_EAST;
	const IntVector2 northCoords = myCoords + STEP_NORTH;
	const IntVector2 southCoords = myCoords + STEP_SOUTH;
	const IntVector2 westCoords = myCoords + STEP_WEST;

	OpenNeighborAsAppropriate( eastCoords, node, openNodes, closedNodes, goalCoords );
	OpenNeighborAsAppropriate( northCoords, node, openNodes, closedNodes, goalCoords );
	OpenNeighborAsAppropriate( southCoords, node, openNodes, closedNodes, goalCoords );
	OpenNeighborAsAppropriate( westCoords, node, openNodes, closedNodes, goalCoords );
}


//-----------------------------------------------------------------------------------------------
// Note: DO NOT MODIFY THIS FUNCTION, or any of the other _Reference() functions.
//	Instead, modify the _Optimized() versions of ComputePath, etc.
//
void Map::ComputePath_Reference( const IntVector2& start, const IntVector2& goal, TileCoordsList& out_path ) const
{
	if( g_debugVerbose ) printf( "\nDEBUG: ComputePath_Reference( %i,%i -> %i,%i )...\n", start.x, start.y, goal.x, goal.y );

	out_path.clear();
	PathNodeList openList;
	PathNodeList closedList;
	if( IsTileSolid( start ) )
		return;

	// Push start node to open set
	openList.push_back( new PathNode( start, nullptr, 0.f, GuessEstimatedCost( start, goal ) ) );
	if( g_debugVerbose ) printf( "DEBUG: Opening start node (%i,%i)...\n", start.x, start.y );

	// Iterate until the end is found (or we've exhausted all leads)
	for( ;; )
	{
		if( g_debugVerbose ) printf( "DEBUG: Starting next iteration with %i open and %i closed...\n", (int) openList.size(), (int) closedList.size() );
		if( openList.empty() )
			return; // FAILURE - We're out of leads, and we never found the end.  No path possible!

		// Select the "best" open node (the one with the lowest f=g+h cost)
		PathNodeList::iterator bestNodeIter = FindBestNodeInList( openList );
		PathNode* node = *bestNodeIter;
		if( g_debugVerbose )
		{
			for( PathNode* openNode : openList )
			{
				int parentX = openNode->m_parent ? openNode->m_parent->m_tileCoords.x : -1;
				int parentY = openNode->m_parent ? openNode->m_parent->m_tileCoords.y : -1;
				printf( "DEBUG:    Open: (%i,%i), g=%.0f, h=%.0f, f=%.0f, p=(%i,%i)\n",
					openNode->m_tileCoords.x,
					openNode->m_tileCoords.y,
					openNode->m_gCostCumulative,
					openNode->m_hDistToEndGuess,
					openNode->m_gCostCumulative + openNode->m_hDistToEndGuess,
					parentX, parentY );
			}
			printf( "DEBUG: Out of %i open nodes, best is (%i,%i), g=%.0f, h=%.0f, f=%.0f\n", (int) openList.size(), node->m_tileCoords.x, node->m_tileCoords.y, node->m_gCostCumulative, node->m_hDistToEndGuess, node->m_gCostCumulative + node->m_hDistToEndGuess );
		}

		// Check if it is the end
		if( node->m_tileCoords == goal )
		{
			if( g_debugVerbose ) printf( "DEBUG:   Reached goal at (%i,%i)!\n\n", node->m_tileCoords.x, node->m_tileCoords.y );
			BuildBackwardsPath( node, out_path );
			DeleteNodesInSet( openList );
			DeleteNodesInSet( closedList );

			if( g_debugVerbose ) PrintMap( out_path );
			return; // SUCCESS - path found!
		}

		// Move it from the open list to the closed list
		openList.erase( bestNodeIter );
		if( g_debugVerbose ) printf( "DEBUG:   Removed (%i,%i) from open set (%i now open)\n", node->m_tileCoords.x, node->m_tileCoords.y, (int) openList.size() );
		closedList.push_back( node );
		if( g_debugVerbose ) printf( "DEBUG:   Added (%i,%i) to closed set (%i now closed)\n", node->m_tileCoords.x, node->m_tileCoords.y, (int) closedList.size() );

		// Add each neighbor to open set, if appropriate
		OpenNeighborsAsAppropriate( node, openList, closedList, goal );
	}
}


//-----------------------------------------------------------------------------------------------
// Note: Modify this implementation (and change Map or MapTile any way you like, so long as
//	you do not break the Reference implementation, above.  When in doubt, create a new/separate
//	function, class, etc. so Reference can continue to use the old ones.
// 
void Map::ComputePath_Optimized( const IntVector2& start, const IntVector2& goal, TileCoordsList& out_path ) const
{
	// #ToDo: Paste the Reference implementation here, then optimize it.
	ComputePath_Reference( start, goal, out_path );
}


