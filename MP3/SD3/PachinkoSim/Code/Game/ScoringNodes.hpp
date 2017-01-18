#pragma once

#include <vector>


//-----------------------------------------------------------------------------------------------
class Cloth;
class Node;
class Ball;
class Vector3Double;


//-----------------------------------------------------------------------------------------------
class ScoringNodes
{
public:
	ScoringNodes( const Cloth& cloth );
	void Update( float deltaSeconds );
	void Render() const;
	float IsColliding( const Ball& ball, const Vector3Double& scoringNodePosition, float scoringNodeRadius );

public:
	std::vector< Node* > m_scoringNodes;
};