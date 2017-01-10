#pragma once
#include "Engine/EngineBase.hpp"
#include "Engine/IntVector2.hpp"
#include "MapTile.hpp"
#include <string>
#include <vector>
#include <set>


//-----------------------------------------------------------------------------------------------
extern bool g_debugVerbose;


/////////////////////////////////////////////////////////////////////////////////////////////////
class PathNode
{
public:
	PathNode( const IntVector2& tileCoords, PathNode* parent, float gCostCumulative, float hDistToEndGuess );
	float			GetF() const { return m_gCostCumulative + m_hDistToEndGuess; }

public:
	IntVector2		m_tileCoords;
	PathNode*		m_parent;
	float			m_gCostCumulative;
	float			m_hDistToEndGuess;
};


//-----------------------------------------------------------------------------------------------
typedef std::vector< IntVector2 >	TileCoordsList;
typedef std::vector< PathNode* >	PathNodeList;


//-----------------------------------------------------------------------------------------------
// Globals
extern const IntVector2 STEP_EAST;
extern const IntVector2 STEP_WEST;
extern const IntVector2 STEP_NORTH;
extern const IntVector2 STEP_SOUTH;


/////////////////////////////////////////////////////////////////////////////////////////////////
class Map
{
public:
	Map( int width, int height );
	~Map();
	void		Generate();
	IntVector2	GetRandomInteriorNonSolidCoords() const;
	void		ChangeTileType( int tileIndex, TileType newType );
	void		ChangeTileType( int tileX, int tileY, TileType newType );
	int			GetTileIndexForCoords( int tileX, int tileY ) const;
	IntVector2	GetTileCoordsForIndex( int tileIndex ) const;
	int			CountAdjacentTilesOfType( int tileIndex, TileType neighborTypeToCount ) const;
	bool		IsTileAdjacentTo( int tileIndex, TileType checkForNeighborType ) const;
	bool		IsTileSolid( const IntVector2& tileCoords ) const;
	bool		IsTileOnEdge( int tileIndex ) const;
	TileType	GetTileTypeAtCoords( const IntVector2& tileCoords ) const;
	char		GetGlyphForTile( int tileX, int tileY ) const;
	bool		IsPositionInList( int tileX, int tileY, const TileCoordsList& tileList ) const;
	void		PrintMap( const TileCoordsList& specialPositionsToMark = TileCoordsList() ) const;
	float		GetGCostForTileTypeAt( const IntVector2& tileCoords ) const;
	float		GuessEstimatedCost( const IntVector2& fromHere, const IntVector2& toHere ) const;
	void		OpenNeighborAsAppropriate( const IntVector2& neighborCoords, PathNode* parent, PathNodeList& openNodes, PathNodeList& closedNodes, const IntVector2& goalCoords ) const;
	void		OpenNeighborsAsAppropriate( PathNode* node, PathNodeList& openNodes, PathNodeList& closedNodes, const IntVector2& goalCoords ) const;
	void		ComputePath_Reference( const IntVector2& start, const IntVector2& goal, TileCoordsList& out_path ) const;
	void		ComputePath_Optimized( const IntVector2& start, const IntVector2& goal, TileCoordsList& out_path ) const;

	float			GetGCostForTileType( TileType tileType ) const;
	unsigned char	GetGlyphForTileType( TileType tileType );
	bool			IsTileTypeSolid( TileType tileType ) const;
	std::string		GetNameForTileType( TileType tileType );

public:
	int							m_width;
	int							m_height;
	std::vector< MapTile* >		m_tiles;
	std::vector< float >		m_gCosts;
	std::vector< unsigned char >m_tileGlyphs;
};


