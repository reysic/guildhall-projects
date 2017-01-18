#pragma once
#include "Engine/Math/Vector3.hpp"
#include <math.h>
// #include "Game/Node.hpp"

class Node;

const float UNSTRETCHED_LENGTH_DEFAULT = .90f;
const float SHEAR_UNSTRETECHED_LENGTH = ( float ) sqrt( (UNSTRETCHED_LENGTH_DEFAULT * UNSTRETCHED_LENGTH_DEFAULT) * 2.f );
const float BEND_UNSTRETECHED_LENGTH = UNSTRETCHED_LENGTH_DEFAULT * 2.f;
const float STRUCTURAL_STIFFNESS = 50.50f;
const float SHEAR_STIFFNESS = 5.50f;
const float BEND_STIFFNESS = 13.f;
const float DAMPING_DEFAULT = .9f;

class Spring {
public:
	enum SPRING_TYPE
	{
		SPRING_STRUCTURAL = 0,
		SPRING_SHEAR,
		SPRING_BEND
	};
	Spring();
	Spring(Node* nodeA = nullptr, Node* nodeB = nullptr, SPRING_TYPE type = SPRING_STRUCTURAL, float unstrLen = UNSTRETCHED_LENGTH_DEFAULT,
		float stiffness = STRUCTURAL_STIFFNESS, float damping = DAMPING_DEFAULT);

	void Render();
	Node* m_nodeA;
	Node* m_nodeB;
	SPRING_TYPE m_type;
	float m_unstretchedLength;
	float m_stiffness;
	float m_dampingCoefficient;

private:

};