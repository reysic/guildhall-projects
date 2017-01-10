#pragma once

#include <vector>
#include <list>

#include "Engine/Math/IntAABB2.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Game/Cells/Cell.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
enum Direction;


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	Map( const IntVector2& mapSizeInCells, const std::string& name );

	void Render() const;

	// Set
	void SetAreaCellType( const IntAABB2& area, CellType type );
	void SetCellAsType( Cell cell, CellType type );
	void SetCellAsTypeAtIndex( int index, CellType type );
	void SetAllCellsAsNotVisible();
	void SetSize( const IntVector2& size );

	// Get
	int GetVectorIndexForCellLocation( const IntVector2& location );
	int GetVectorIndexForCellLocation( int cellX, int cellY );
	std::vector< Cell > GetNeighborsInRange( const IntVector2& position, int range );
	std::vector< Cell > GetNeighborsInRangeOfType( const IntVector2& position, int range, CellType type );
	std::vector< IntVector2 > GetValidAdjacentPositions( const IntVector2& position, int movementProperties );
	Cell& GetCellAtPosition( const IntVector2& position );
	CellType GetCellTypeAtPosition( int cellX, int cellY );
	IntVector2 GetCellPositionForWorldPosition( const Vector2& position );
	IntVector2 GetRandomAirCellPosition();
	static Direction GetRandomDirection();
	float GetDistanceBetweenCellPositions( const IntVector2& position1, const IntVector2& position2 );
	Direction GetDirectionToCellPositionFromCellPosition( const IntVector2& toCellPosition, 
		const IntVector2& fromCellPosition );
	Direction GetOppositeDirection( Direction inDirection );
	Item* GetFirstItemAtCellPosition( const IntVector2& position );

	// Bools
	bool IsPositionValid( int cellX, int cellY );
	bool IsPositionValid( const IntVector2& position );
	bool IsAreaValid( const IntAABB2& area );
	bool DoesAreaContainCellType( const IntAABB2& area, CellType type );
	bool IsCellValid( Cell cell );
	bool IsCellOfType( Cell cell, CellType type );
	bool HasLineOfSight( const Vector2& start, const Vector2& end, bool updateCellVisibility );
	bool AreCellPositionsAdjacent( const IntVector2& position1, const IntVector2& position2 );
	bool DoesCellPositionContainAgent( const IntVector2& position );

	// Utils
	void RemoveAgentAtCellPosition( const IntVector2& position );
	void AddItemToCellPosition( const IntVector2& position, Item* itemToAdd );
	void RemoveFirstItemAtCellPosition( const IntVector2& position );
	XMLNode SaveToXML();
	std::string GetXMLGlyphForCell( Cell cell );

public:
	IntVector2 m_mapSizeInCells;
	std::string m_name;
	std::vector< Cell > m_cells;
	int m_numCells;
	int m_currentCellIndex;
	int m_mapWidth;
	int m_mapHeight;
	bool m_debugRender;
	Vector2 m_renderingOffset;
};