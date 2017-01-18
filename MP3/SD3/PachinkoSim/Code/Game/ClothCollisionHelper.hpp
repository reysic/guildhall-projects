#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector3Double.hpp"
#include "Game/Spring.hpp"
//#include "Game/Ball.hpp"
#include "Game/Cloth.hpp"
#include "Game/Node.hpp"
#include <vector>

const double CLOTH_IMPACT_VERT_COMPONENT_REDUCTION = .50;
const float BOX_WALL_VEL_REFLECTION = -.7f;
const double TRIANGLE_SIZE_BUFFER_MAG = .05;
const float BALL_Z_PUSH_AMOUNT = .02f;

class Ball;

class ClothCollisionHelper {
public:
	ClothCollisionHelper();
// 	ClothCollisionHelper( Cloth* cloth, Ball* ball );

	//  First just brute forcing by checking distances to all nodes

	// Hope to use a sorted set of lists to improve performance later


	struct Triangle {
		Vector3Double a;
		Vector3Double b;
		Vector3Double c;

		Node* nodeA;
		Node* nodeB;
		Node* nodeC;


		void Set( Vector3Double v0, Vector3Double v1, Vector3Double v2 ) {
			a = v0;
			b = v1;
			c = v2;
		}

		void SetPointers(  Node* A,  Node* B,  Node* C )
		{
			nodeA = A;
			nodeB = B;
			nodeC = C;
		}
	};

	float m_longestStructural;
	float m_longestShear;
	float m_longestBend;

	Node* m_closestNode			= nullptr;
	Node* m_secondClosestNode	= nullptr;
	Node* m_thirdClosestNode	= nullptr;

	Ball* m_ball;

	Cloth* m_cloth;

	Vector3 m_lastImpactPos;

	std::vector<Triangle> m_closestTriangles;
	void SetTriangles();

	void DoClotheHelper();

	void CollectPossibleCollisionVerts();
	void GetSetClosestVerts();
	void CheckAndDoCollision();

	void IncreaseTriangleSize( Vector3Double& vertA, Vector3Double& vertB, Vector3Double& vertC );
	void IncreaseTriangleSize( Triangle& tri );


private:
};