#pragma once

#include "../Math/ShortVector2.hpp"
#include "../Environment/Map.hpp"


//-----------------------------------------------------------------------------------------------
class Ant
{
public:
	Ant( int id );
	~Ant();

public:
	ShortVector2 m_position;
	ShortVector2 m_goalPosition;
	CellCoordsList m_activePath;
	int m_id;
	EntityType m_type;
};