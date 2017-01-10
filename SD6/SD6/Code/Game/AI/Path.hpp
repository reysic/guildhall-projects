#pragma once

#include <vector>

#include "Engine/Math/Vector2.hpp"


//-----------------------------------------------------------------------------------------------
class Path
{
public:
	Path();
	~Path();
	
	void Render() const;

	void AddNode( const Vector2& nodePosition );


public:
	std::vector< Vector2 > m_nodes;
};