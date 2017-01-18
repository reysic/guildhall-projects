#pragma once

#include "Game/Maps/Map.hpp"


//-----------------------------------------------------------------------------------------------
struct PathNode
{
	PathNode()
		: m_position( IntVector2( 0, 0 ) )
		, m_parent( nullptr )
		, m_g( 0 )
		, m_h( 0 )
	{
		m_f = m_g + m_h;
	}
	PathNode( const IntVector2 position, PathNode* parent, float g, float h )
		: m_position( position )
		, m_parent( parent )
		, m_g( g )
		, m_h( h )
	{
		m_f = m_g + m_h;
	}
	IntVector2 m_position;
	PathNode* m_parent;
	float m_g;
	float m_h;
	float m_f;
};


//-----------------------------------------------------------------------------------------------
class Pathfinder
{
public:
	Pathfinder( const IntVector2& startPosition, const IntVector2& goalPosition, Map* map, Agent* owningAgent );
	bool TakeOneStep();
	float ComputeLocalG( const IntVector2& currentPosition, const IntVector2& nextPosition );
	float ComputeEstimatedCostFromPosToPos( const IntVector2& currentPosition, const IntVector2& goalPosition );
	void Render() const;

public:
	IntVector2 m_startPosition;
	IntVector2 m_goalPosition;
	Map* m_map;
	Agent* m_owningAgent;
	bool m_isFinished;
	std::vector< PathNode* > m_openList;
	std::vector< PathNode* > m_closedList;
	std::vector< PathNode* > m_path;
};