#include "Game/ClothCollisionHelper.hpp"
#include "Game/Ball.hpp"
#include "Game/TheApp.hpp"
#include "Engine/RayTriangleIntersection.h"
#include "Engine/Renderer/Renderer.hpp"

ClothCollisionHelper::ClothCollisionHelper()
{
	m_closestNode = nullptr;
	m_secondClosestNode = nullptr;
	m_thirdClosestNode = nullptr;
}

void ClothCollisionHelper::SetTriangles()
{
	if (m_closestNode)
	{
		Vector3Double startPos = Vector3Double(-999., -999., -999.);
		Vector3Double rightPos = Vector3Double( -999., -999., -999. );
		Vector3Double botPos = Vector3Double( -999., -999., -999. );
		Vector3Double leftPos = Vector3Double( -999., -999., -999. );
		Vector3Double upPos = Vector3Double( -999., -999., -999. );


		Node* upNode	= nullptr;
		Node* downNode	= nullptr;
		Node* rightNode	= nullptr;
		Node* leftNode	= nullptr;

		const IntVector2& startCoords = m_closestNode->m_coordinates;

		// x is col, y is row
		if (startCoords.x < m_cloth->m_width - 1)
			rightNode = m_cloth->m_nodes[(startCoords.y * m_cloth->m_width) + startCoords.x + 1];
		if (startCoords.x > 0)
			leftNode = m_cloth->m_nodes[(startCoords.y * m_cloth->m_width) + startCoords.x - 1];
		if (startCoords.y < m_cloth->m_height - 1)
			downNode = m_cloth->m_nodes[((startCoords.y + 1) * m_cloth->m_width) + startCoords.x];
		if (startCoords.y > 0)
			upNode = m_cloth->m_nodes[((startCoords.y - 1) * m_cloth->m_width) + startCoords.x];

		startPos = m_closestNode->m_position;
		if (rightNode)
			rightPos = rightNode->m_position;
		if (leftNode)
			leftPos = leftNode->m_position;
		if (downNode)
			botPos = downNode->m_position;
		if (upNode)
			upPos = upNode->m_position;
		
		Triangle tempTri;
		m_closestTriangles.clear();


		// x is col, y is row

		if (upNode && rightNode)
		{
			tempTri.Set( startPos, upPos, rightPos );
			tempTri.SetPointers( m_closestNode, upNode, rightNode );
			IncreaseTriangleSize( tempTri );
			m_closestTriangles.push_back( tempTri );
		}
		if (rightNode && downNode)
		{
			tempTri.Set( startPos, rightPos, botPos );
			tempTri.SetPointers( m_closestNode, rightNode, downNode );
			IncreaseTriangleSize( tempTri );
			m_closestTriangles.push_back( tempTri );
		}
		if (downNode && leftNode)
		{
			tempTri.Set( startPos, botPos, leftPos );
			tempTri.SetPointers( m_closestNode, downNode, leftNode );
			IncreaseTriangleSize( tempTri );
			m_closestTriangles.push_back( tempTri );
		}
		if (leftNode && upNode)
		{
			tempTri.Set( startPos, leftPos, upPos );
			tempTri.SetPointers( m_closestNode, leftNode, upNode );
			IncreaseTriangleSize( tempTri );
			m_closestTriangles.push_back( tempTri );
		}

	}
	if (m_secondClosestNode)
// 	if (1 ==0)
	{
		Vector3Double startPos = Vector3Double( -999., -999., -999. );
		Vector3Double rightPos = Vector3Double( -999., -999., -999. );
		Vector3Double botPos = Vector3Double( -999., -999., -999. );
		Vector3Double leftPos = Vector3Double( -999., -999., -999. );
		Vector3Double upPos = Vector3Double( -999., -999., -999. );


		Node* upNode = nullptr;
		Node* downNode = nullptr;
		Node* rightNode = nullptr;
		Node* leftNode = nullptr;

		const IntVector2& startCoords = m_secondClosestNode->m_coordinates;

		// x is col, y is row
		if (startCoords.x < m_cloth->m_width - 1)
			rightNode = m_cloth->m_nodes[(startCoords.y * m_cloth->m_width) + startCoords.x + 1];
		if (startCoords.x > 0)
			leftNode = m_cloth->m_nodes[(startCoords.y * m_cloth->m_width) + startCoords.x - 1];
		if (startCoords.y < m_cloth->m_height - 1)
			downNode = m_cloth->m_nodes[((startCoords.y + 1) * m_cloth->m_width) + startCoords.x];
		if (startCoords.y > 0)
			upNode = m_cloth->m_nodes[((startCoords.y - 1) * m_cloth->m_width) + startCoords.x];

		startPos = m_secondClosestNode->m_position;
		if (rightNode)
			rightPos = rightNode->m_position;
		if (leftNode)
			leftPos = leftNode->m_position;
		if (downNode)
			botPos = downNode->m_position;
		if (upNode)
			upPos = upNode->m_position;

		Triangle tempTri;
		//m_closestTriangles.clear();


		// x is col, y is row

		if (upNode && rightNode)
		{
			tempTri.Set( startPos, upPos, rightPos );
			tempTri.SetPointers( m_secondClosestNode, upNode, rightNode );
			IncreaseTriangleSize( tempTri );
			m_closestTriangles.push_back( tempTri );
		}
		if (rightNode && downNode)
		{
			tempTri.Set( startPos, rightPos, botPos );
			tempTri.SetPointers( m_secondClosestNode, rightNode, downNode );
			IncreaseTriangleSize( tempTri );
			m_closestTriangles.push_back( tempTri );
		}
		if (downNode && leftNode)
		{
			tempTri.Set( startPos, botPos, leftPos );
			tempTri.SetPointers( m_secondClosestNode, downNode, leftNode );
			IncreaseTriangleSize( tempTri );
			m_closestTriangles.push_back( tempTri );
		}
		if (leftNode && upNode)
		{
			tempTri.Set( startPos, leftPos, upPos );
			tempTri.SetPointers( m_secondClosestNode, leftNode, upNode );
			IncreaseTriangleSize( tempTri );
			m_closestTriangles.push_back( tempTri );
		}
	}
}

// ClothCollisionHelper::ClothCollisionHelper( Cloth* cloth, Ball* ball )
// {
// 	m_ball = ball;
// 	m_cloth = cloth;
// }

void ClothCollisionHelper::DoClotheHelper()
{
	 GetSetClosestVerts();
	 CheckAndDoCollision();

	 m_lastImpactPos = Vector3::ZERO;


	 if (m_ball->m_state.m_position.x < m_cloth->m_minXY.x)
	 {
		 m_ball->m_state.m_position.x = ( float ) m_cloth->m_minXY.x;
		 m_ball->m_state.m_velocity.x = m_ball->m_state.m_velocity.x * BOX_WALL_VEL_REFLECTION;
	 }
	 if (m_ball->m_state.m_position.y < m_cloth->m_minXY.y)
	 {
		 m_ball->m_state.m_position.y = ( float ) m_cloth->m_minXY.y;
		 m_ball->m_state.m_velocity.y = m_ball->m_state.m_velocity.y * BOX_WALL_VEL_REFLECTION;
	 }

	 if (m_ball->m_state.m_position.x > m_cloth->m_maxXY.x)
	 {
		 m_ball->m_state.m_position.x = ( float ) m_cloth->m_maxXY.x;
		 m_ball->m_state.m_velocity.x = m_ball->m_state.m_velocity.x * BOX_WALL_VEL_REFLECTION;
	 }

	 if (m_ball->m_state.m_position.y > m_cloth->m_maxXY.y)
	 {
		 m_ball->m_state.m_position.y = ( float ) m_cloth->m_maxXY.y;
		 m_ball->m_state.m_velocity.y = m_ball->m_state.m_velocity.y * BOX_WALL_VEL_REFLECTION;
	 }

}

void ClothCollisionHelper::CollectPossibleCollisionVerts()
{

}

void ClothCollisionHelper::GetSetClosestVerts()
{
	std::vector<Node*>& clothNodes = m_cloth->m_nodes;

	float closestDistSqrd =			99999999.0f;
	float secondClosestDistSqrd =	999999999.0f;
	float thirdClosestDistSqrd =	9999999999.0f;

	bool goodClose = false;
	bool goodSecond = false;
	bool goodThird = false;
								 
	if (m_closestNode != nullptr)
		goodClose = true;
	else
		goodClose = false;
	if(goodClose)
		closestDistSqrd = m_closestNode->m_position.DistanceToSquared( m_ball->m_state.m_position );
// 	else
// 		m_closestNode = m_cloth->m_nodes[0];

	if (m_secondClosestNode)
		goodSecond = true;
	else
		goodSecond = false;
	if(goodSecond)
		secondClosestDistSqrd = m_secondClosestNode->m_position.DistanceToSquared( m_ball->m_state.m_position );
// 	else
// 		m_closestNode = m_cloth->m_nodes[0];

	if (m_thirdClosestNode)
		goodThird = true;
	else
		goodThird = false;
	if(goodThird)
		thirdClosestDistSqrd = m_thirdClosestNode->m_position.DistanceToSquared( m_ball->m_state.m_position );
// 	else
// 		m_closestNode = m_cloth->m_nodes[0];


	if ( g_theApp->m_inputSystem.IsKeyDown( 'B' ) )
	{
		bool breakNow = true;
		breakNow;
	}

	if (goodClose)
	{
		Vector3 closest = Vector3( ( float ) m_closestNode->m_position.x, ( float ) m_closestNode->m_position.y, ( float ) m_closestNode->m_position.z );
	//	g_theRenderer->AddDebugSphere( closest, .18f, Rgba::RED, 0, RenderCommand::XRAY );
	}
// 	if (goodSecond)
// 	{
// 		Vector3 second = Vector3( m_secondClosestNode->m_position.x, m_secondClosestNode->m_position.y, m_secondClosestNode->m_position.z );
// 		g_theRenderer->AddDebugSphere( second, .18f, Rgba::GREEN, 0, RenderCommand::XRAY );
// 	}
// 	if (goodThird) {
// 		Vector3 third = Vector3( m_thirdClosestNode->m_position.x, m_thirdClosestNode->m_position.y, m_thirdClosestNode->m_position.z );
// 		g_theRenderer->AddDebugSphere( third, .18f, Rgba::BLACK, 0, RenderCommand::XRAY );
// 	}


	for (int i = 0; ( unsigned int ) i < m_cloth->m_nodes.size(); ++i)
	{
		float tempDistSqrd = clothNodes[i]->m_position.DistanceToSquared( m_ball->m_state.m_position);

		if (tempDistSqrd < closestDistSqrd)
		{
			if (clothNodes[i] != m_closestNode)
			{
				m_thirdClosestNode = m_secondClosestNode;
				thirdClosestDistSqrd = secondClosestDistSqrd;

				m_secondClosestNode = m_closestNode;
				secondClosestDistSqrd = closestDistSqrd;

				m_closestNode = clothNodes[i];
				closestDistSqrd = tempDistSqrd;
			}
		}
		else if (tempDistSqrd < secondClosestDistSqrd)
		{
			if (clothNodes[i] != m_secondClosestNode)
			{
				if (clothNodes[i] != m_closestNode)
				{
					m_thirdClosestNode = m_secondClosestNode;
					thirdClosestDistSqrd = secondClosestDistSqrd;

					m_secondClosestNode = clothNodes[i];
					secondClosestDistSqrd = tempDistSqrd;
				}
			}
		}
		else if (tempDistSqrd < thirdClosestDistSqrd)
		{
			if (clothNodes[i] != m_thirdClosestNode)
			{
				if (clothNodes[i] != m_secondClosestNode)
				{
					if (clothNodes[i] != m_closestNode)
					{
						m_thirdClosestNode = clothNodes[i];
						thirdClosestDistSqrd = tempDistSqrd;
					}
				}
			}
		}
	}


}

void ClothCollisionHelper::CheckAndDoCollision()
{
	if (g_theApp->m_inputSystem.IsKeyDown( 'N' ))
		bool breakNow = true;

	/*
	You've got 3 points: A,B,C
	make u = B - A
	v = C - A

	The normal of the plane is n_j = u x v = \epsilon_{jkl} u_{k} v_{l}
	To compute d, just take a point that you know it is in the plane and solve
	n_{j} P_{j} = d
	*/



	/*
	float denom = normal.dot(ray.direction);
	if (abs(denom) > 0.0001f) // your favorite epsilon
	{
		float t = (center - ray.origin).dot(normal) / denom;
		if (t >= 0) return true; // you might want to allow an epsilon here too
	}
	return false;
	*/

	Vector3 prevBallPos = m_ball->m_prevState.m_position;
	Vector3 ballPos = m_ball->m_state.m_position;

	Vector3 ballMotionVect = prevBallPos - ballPos;
	double ballMotionMag = Vector3Double( ballMotionVect ).GetMagnitude();
	Vector3Double ballDirection = Vector3Double( ballMotionVect ).GetNormalized();

	double meanX = (m_closestNode->m_position.x + m_secondClosestNode->m_position.x + m_thirdClosestNode->m_position.x) / 3.0;
	double meanY = (m_closestNode->m_position.y + m_secondClosestNode->m_position.y + m_thirdClosestNode->m_position.y) / 3.0;
	double meanZ = (m_closestNode->m_position.z + m_secondClosestNode->m_position.z + m_thirdClosestNode->m_position.z) / 3.0;

	Vector3Double center = Vector3Double( meanX, meanY, meanZ );
	
//	double denom = norm.Dot( ballDirection );


	double superT = 0;
	double superU = 0;
	double superV = 0;
	double _T = 0;
	double _U = 0;
	double _V = 0;

	/*
		u & v are barycentric coords. 
		u & v sort of represent a & of the way through the triangle
	*/
	SetTriangles();

	bool atLeastOneCollision = false;
	bool alreadyPushedUpZ = false;
	for (int i = 0; ( unsigned int ) i < m_closestTriangles.size(); ++i)
	{

// 		double rayOrigin[3] = { ballPos.x, ballPos.y, ballPos.z };
		double rayOrigin[3] = { prevBallPos.x, prevBallPos.y, prevBallPos.z };
		double rayDirection[3] = { ballDirection.x, ballDirection.y, ballDirection.z };

// 		double vert0[3] = { m_closestNode->m_position.x, m_closestNode->m_position.y, m_closestNode->m_position.z };
// 		double vert1[3] = { m_secondClosestNode->m_position.x, m_secondClosestNode->m_position.y, m_secondClosestNode->m_position.z };
// 		double vert2[3] = { m_thirdClosestNode->m_position.x, m_thirdClosestNode->m_position.y, m_thirdClosestNode->m_position.z };

		double vert0[3] = { m_closestTriangles[i].a.x, m_closestTriangles[i].a.y, m_closestTriangles[i].a.z };
		double vert1[3] = { m_closestTriangles[i].b.x, m_closestTriangles[i].b.y, m_closestTriangles[i].b.z };
		double vert2[3] = { m_closestTriangles[i].c.x, m_closestTriangles[i].c.y, m_closestTriangles[i].c.z };

		if (g_theApp->m_inputSystem.IsKeyDown( 'C' ))
			bool breakNow = true;



		int firstTest = 0;
		// 	if(m_closestNode && m_secondClosestNode && m_thirdClosestNode)
		if (m_closestTriangles.size() > 0)
			firstTest = intersect_triangle( rayOrigin, rayDirection, vert0, vert1, vert2, &_T, &_U, &_V );

		if (firstTest == 1)
		{
			Vector3 cpoint = prevBallPos + ( Vector3( ( float ) rayDirection[0], ( float ) rayDirection[1], ( float ) ( rayDirection[2]) * _T ) );
			
			double gimmie = 2.;

// 			if(ballMotionMag >= _T && _T >= 0)
			if ( _T >= 0)
			{
				Vector3 normedVel = m_ball->m_state.m_velocity.GetNormalized();

				Vector3Double u = Vector3Double( vert1[0], vert1[1], vert1[2] ) - Vector3Double(vert0[0], vert0[1], vert0[2]);
				Vector3Double v = Vector3Double( vert2[0], vert2[1], vert2[2] ) - Vector3Double( vert0[0], vert0[1], vert0[2] );

				Vector3Double norm = u.Cross( v );
				norm.SetNormalized();
				
				bool dotProdTest = (norm.Dot( normedVel ) > 0);
// 				bool dotProdTest = (norm.Dot( Vector3Double(rayDirection[0], rayDirection[1], rayDirection[2]) ) > 0);
				bool breakNow = true;

				float distToClosest = ( Vector3Double(ballPos.x, ballPos.y, ballPos.z) - m_closestNode->m_position).GetMagnitude();
				bool distTest = (distToClosest < 1.9f && distToClosest > -1.5f);

				if (dotProdTest && distTest)
				{
					atLeastOneCollision = true;

				//	g_theRenderer->AddDebugSphere( m_ball->m_prevState.m_position, .2f, Rgba::PINK, 5.f, RenderCommand::XRAY );
					//g_theRenderer->AddDebugSphere( cpoint, .1f, Rgba::BLUE, 5.f, RenderCommand::XRAY );

					Vector3Double ballPosDouble = Vector3Double( prevBallPos.x, prevBallPos.y, prevBallPos.z );

					float distToA = Vector3Double( ballPosDouble - m_closestTriangles[i].nodeA->m_position ).GetMagnitude();
					float distToB = Vector3Double( ballPosDouble - m_closestTriangles[i].nodeB->m_position ).GetMagnitude();
					float distToC = Vector3Double( ballPosDouble - m_closestTriangles[i].nodeC->m_position ).GetMagnitude();

					float distTotal = distToA + distToB + distToC;

					float magA = 1.f / ( distToA / distTotal);
					float magB = 1.f / (distToB / distTotal);
					float magC = 1.f / (distToC / distTotal);

					float ballToClothForceMultiplier = 2.0f;
					float ballVelMag = m_ball->m_state.m_velocity.GetMagnitude();

					float impulseTime = .10f;
					Vector3Double force;

					Vector3Double ballNormVel = Vector3Double( m_ball->m_state.m_velocity.GetNormalized() );

					force = Vector3Double( ballNormVel * (ballVelMag * magA * ballToClothForceMultiplier) );
					m_closestTriangles[i].nodeA->m_impulses.push_back( Node::Impulse( force, impulseTime ) );

					force = Vector3Double( ballNormVel * (ballVelMag * magB * ballToClothForceMultiplier) );
					m_closestTriangles[i].nodeB->m_impulses.push_back( Node::Impulse( force, impulseTime ) );

					force = Vector3Double( ballNormVel * (ballVelMag * magC * ballToClothForceMultiplier) );
					m_closestTriangles[i].nodeC->m_impulses.push_back( Node::Impulse( force, impulseTime ) );

					Vector3Double v10 = m_closestTriangles[i].c - m_closestTriangles[i].a;
					Vector3Double v20 = m_closestTriangles[i].b - m_closestTriangles[i].a;

					//            !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					//m_ball->m_state.m_position = cpoint;

					Vector3Double Vel = m_ball->m_state.m_velocity;

					Vector3Double normPara = (Vel.Dot( norm )) * norm;
					Vector3Double normPerp = Vel - normPara;

					// reduceing the vertical velocity component to simulate soft collision;
					normPara = normPara * CLOTH_IMPACT_VERT_COMPONENT_REDUCTION;
					normPara = normPara * -1.;
					Vel = normPara + normPerp;
					Vel.z += .01;
					m_ball->m_state.m_velocity = Vector3( Vel.x, Vel.y, Vel.z );
					//m_ball->m_state.m_position = m_lastImpactPos;
				}
			}
		}
		else  // did not collide with cloth triangle
		{
			if (!atLeastOneCollision && !alreadyPushedUpZ)
// 			if (1 == 0)
			{
				Vector3Double u = Vector3Double( vert1[0], vert1[1], vert1[2] ) - Vector3Double( vert0[0], vert0[1], vert0[2] );
				Vector3Double v = Vector3Double( vert2[0], vert2[1], vert2[2] ) - Vector3Double( vert0[0], vert0[1], vert0[2] );

				Vector3Double norm = u.Cross( v );
				norm.SetNormalized();
				Vector3 planeCenter;
				planeCenter.x = vert0[0] + vert1[0] + vert2[0] / 3.0;
				planeCenter.y = vert0[1] + vert1[1] + vert2[1] / 3.0;
				planeCenter.z = vert0[2] + vert1[2] + vert2[2] / 3.0;
				
				if (ballMotionMag > (planeCenter - m_ball->m_prevState.m_position).GetMagnitude())
				{
					Vector3 normedVel = m_ball->m_state.m_velocity.GetNormalized();
					bool dotProdTest = (norm.Dot( normedVel ) > 0);
// 					bool dotProdTest = (norm.Dot( Vector3Double( rayDirection[0], rayDirection[1], rayDirection[2] ) ) > 0);
					if (!dotProdTest)
					{
						m_ball->m_state.m_position.z += BALL_Z_PUSH_AMOUNT;
						alreadyPushedUpZ = true;
					}
				}
			}
		}
	}
}

void ClothCollisionHelper::IncreaseTriangleSize( Vector3Double& vertA, Vector3Double& vertB, Vector3Double& vertC )
{
	Vector3Double directionA = ((vertB - vertA) + (vertC - vertA)).GetNormalized();
	Vector3Double directionB = ((vertA - vertB) + (vertC - vertB)).GetNormalized();
	Vector3Double directionC = ((vertA - vertC) + (vertB - vertC)).GetNormalized();

	vertA = vertA + (directionA * TRIANGLE_SIZE_BUFFER_MAG);
	vertB = vertB + (directionA * TRIANGLE_SIZE_BUFFER_MAG);
	vertC = vertC + (directionA * TRIANGLE_SIZE_BUFFER_MAG);
}

void ClothCollisionHelper::IncreaseTriangleSize( Triangle& tri )
{
	Vector3Double& vertA = tri.a;
	Vector3Double& vertB = tri.b;
	Vector3Double& vertC = tri.c;

	Vector3Double directionA = ((vertB - vertA) + (vertC - vertA)).GetNormalized();
	Vector3Double directionB = ((vertA - vertB) + (vertC - vertB)).GetNormalized();
	Vector3Double directionC = ((vertA - vertC) + (vertB - vertC)).GetNormalized();

	tri.a = vertA + (directionA * TRIANGLE_SIZE_BUFFER_MAG);
	tri.b = vertB + (directionA * TRIANGLE_SIZE_BUFFER_MAG);
	tri.c = vertC + (directionA * TRIANGLE_SIZE_BUFFER_MAG);

	/*
	vertA = vertA + (directionA * TRIANGLE_SIZE_BUFFER_MAG);
	vertB = vertB + (directionA * TRIANGLE_SIZE_BUFFER_MAG);
	vertC = vertC + (directionA * TRIANGLE_SIZE_BUFFER_MAG);
	*/
}
