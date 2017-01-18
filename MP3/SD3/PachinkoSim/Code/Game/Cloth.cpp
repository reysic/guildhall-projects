#include "Game/Cloth.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Renderer/VertexPCT.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/GameCommon.hpp"
std::string CLOTH_FILENAME_1 = "Data/Images/cloth2.jpg";
std::string CLOTH_FILENAME_2 = "Data/Images/cloth1.jpg";
std::string CLOTH_FILENAME_3 = "Data/Images/colbert.jpg";
std::string CLOTH_FILENAME_4 = "Data/Images/holy.jpg";


//-----------------------------------------------------------------------------------------------
Cloth::Cloth()
{
}


//-----------------------------------------------------------------------------------------------
Cloth::Cloth( const Vector3Double& topLeftClothPosition )
	: m_width( 20 )
	, m_height( 20 )
	, m_topLeftClothPosition( topLeftClothPosition )
{

	m_clothTexture1 = Texture::CreateOrGetTexture(CLOTH_FILENAME_1);
	m_clothTexture2 = Texture::CreateOrGetTexture(CLOTH_FILENAME_2);
	m_clothTexture3 = Texture::CreateOrGetTexture(CLOTH_FILENAME_3);
	m_clothTexture4 = Texture::CreateOrGetTexture(CLOTH_FILENAME_4);
	// Make the nodes contained within the cloth
	for (int row = 0; row < m_height; ++row)
	{
		for (int column = 0; column < m_width; ++column)
		{
			float xTex = RangeMap(column, 0.f, m_width - 1, 0.f, 1.f);
			float yTex = RangeMap(row, 0.f, m_width - 1, 0.f, 1.f);
			Vector2 texCoords = Vector2(xTex, yTex);
			IntVector2 coords = IntVector2(column, row);
			Node* newNode = new Node( Vector3Double( m_topLeftClothPosition.x + ( 1.0f * column ), m_topLeftClothPosition.y - (1.0f * row), m_topLeftClothPosition.z  ), 1.0 , texCoords, coords);
			
			// just 2 fixed anchors
			if (1 == 0)
			{
				if (row == 0 && column == 0)
					newNode->m_isFixed = true;
				if (row == 0 && column == m_width - 1)
					newNode->m_isFixed = true;
			}
			else if( 1==0 ) //  4 corners
			{
				if(column == 0 || column == m_width -1)
					if (row == 0 || row == m_height - 1 )
						newNode->m_isFixed = true;
			}
			else  // all edges are anchors
			{
				if (column == 0 || column == m_width - 1 || row == 0 || row == m_height - 1)
					newNode->m_isFixed = true;

				// raise the 2 sides and top row
				if (column == 0 || column == m_width - 1 || row == 0)
				{
					newNode->m_position.z += 1.f;
					newNode->m_prevPos.z += 1.f;
				}

		//		lower the bottom row near scoring
				if (row == m_height - 1)
				{
// 					newNode->m_position.z -= .2f;
// 					newNode->m_prevPos.z -= .2f;
				}
			}


			

			m_nodes.push_back( newNode );
			if (newNode->m_isFixed)
			{
				m_fixedNodes.push_back(newNode);
			}
		}
	}
	SetPins();

	int nodeVectorIndex = 0;
	// Connect the nodes within the cloth with springs
	for ( int row = 0; row < m_height; ++row)
	{
		for ( int column = 0; column < m_width; ++column)
		{
			if ( column < ( m_width - 1 ) )
			{
				MakeAndConnectRight( nodeVectorIndex );
			}
			if ( row < ( m_height - 1 ) )
			{
				MakeAndConnectBelow( nodeVectorIndex) ;
			}
			if (column < (m_width - 1)  && row < (m_height - 1))
			{
				MakeAndConnectShearBR( nodeVectorIndex );
			}
			if (column != 0 && row < (m_height - 1))
			{
				MakeAndConnectShearBL( nodeVectorIndex );
			}
			if (column < (m_width - 2))
			{
				MakeAndConnectBendR( nodeVectorIndex );
			}
			if (row < (m_height - 2))
			{
				MakeAndConnectBendB( nodeVectorIndex );
			}

			nodeVectorIndex++;
		}
	}
	m_minXY = Vector3Double(m_topLeftClothPosition.x, m_topLeftClothPosition.y - m_height, 0);
	m_maxXY = Vector3Double(m_topLeftClothPosition.x + m_width - 1, m_topLeftClothPosition.y, 0);
	
}


//-----------------------------------------------------------------------------------------------
void Cloth::Update( float deltaSeconds )
{
	for ( int i = 0; i < m_nodes.size(); ++i )
	{
		m_nodes[ i ]->Update( deltaSeconds );
	}
}


//-----------------------------------------------------------------------------------------------
void Cloth::Render() const
{
	std::vector<Vertex3D_PCT> verts;
	for (int i = 0; ( unsigned int ) i < m_nodes.size(); ++i)
	{
		IntVector2 coordsTopLeft = GetCoordsAtIndex(i);
		IntVector2 coordsTopRight = coordsTopLeft;
		coordsTopRight.x += 1;
		IntVector2 coordsBottomRight = coordsTopRight;
		coordsBottomRight.y -= 1;
		IntVector2 coordsBottomLeft = coordsTopLeft;
		coordsBottomLeft.y -= 1;

		Node* topLeft = GetNodeAtCoords(coordsTopLeft);
		Node* topRight = GetNodeAtCoords(coordsTopRight);
		Node* bottomRight = GetNodeAtCoords(coordsBottomRight);
		Node* bottomLeft = GetNodeAtCoords(coordsBottomLeft);

		if (topLeft != nullptr
			&& topRight != nullptr
			&& bottomRight != nullptr
			&& bottomLeft != nullptr)
		{
			Vertex3D_PCT clothTriBot[3];
			Vertex3D_PCT clothTriTop[3];
			//Bot
			clothTriBot[0].m_texCoords = topLeft->m_texCoords;
			clothTriBot[1].m_texCoords = bottomRight->m_texCoords;
			clothTriBot[2].m_texCoords = bottomLeft->m_texCoords;

			clothTriBot[0].m_position = topLeft->m_position;
			clothTriBot[1].m_position = bottomRight->m_position;
			clothTriBot[2].m_position = bottomLeft->m_position;

			//Color
			for (int i = 0; i < 3; i++)
			{
				clothTriBot[i].m_color = Rgba::WHITE;
				clothTriTop[i].m_color = Rgba::WHITE;
			}

			//Top
			clothTriTop[0].m_texCoords = topLeft->m_texCoords;
			clothTriTop[1].m_texCoords = topRight->m_texCoords;
			clothTriTop[2].m_texCoords = bottomRight->m_texCoords;

			clothTriTop[0].m_position = topLeft->m_position;
			clothTriTop[1].m_position = topRight->m_position;
			clothTriTop[2].m_position = bottomRight->m_position;

			for (int i = 0; i < 3; i++)
			{
				verts.push_back(clothTriBot[i]);
			}
			for (int i = 0; i < 3; i++)
			{
				verts.push_back(clothTriTop[i]);
			}
		}
	}

	//Draw from verts
	if (g_texIndexToDraw == 0)
	{
		g_theRenderer->BindTexture(*m_clothTexture1);
	}
	else if (g_texIndexToDraw == 1)
	{
		g_theRenderer->BindTexture(*m_clothTexture2);
	}
	else if (g_texIndexToDraw == 2)
	{
		g_theRenderer->BindTexture(*m_clothTexture3);
	}
	else if (g_texIndexToDraw == 3)
	{
		g_theRenderer->BindTexture(*m_clothTexture4);
	}
	else
	{
		g_theRenderer->BindTexture(*m_clothTexture1);
	}
	
	g_theRenderer->DrawVertexArray_PCT(&verts[0], verts.size(), PRIMITIVE_TRIANGLES);

	
	for ( int i = 0; ( unsigned int ) i < m_nodes.size(); ++i )
	{
		IntVector2 coords = m_nodes[i]->m_coordinates;
		
		if (coords.x != 0 && coords.x != m_width - 1 && coords.y != 0 && coords.y != m_height - 1)
		{
			if (m_nodes[i]->m_isFixed)
				m_nodes[i]->Render();
		}
		
	}
	/*
	for ( int i = 0; i < m_nodes.size(); ++i )
	{
		if(m_nodes[i]->m_isFixed)
			m_nodes[ i ]->Render();
	}
	*/
// 	for ( auto i : m_springs )
// 	{
// 		i->Render();
// 	}
	
}


//-----------------------------------------------------------------------------------------------
void Cloth::MakeAndConnectRight( const int& nodeVectorIndex )
{
	Spring* newSpring = new Spring( m_nodes[ nodeVectorIndex ], m_nodes[ nodeVectorIndex + 1 ], Spring::SPRING_STRUCTURAL, UNSTRETCHED_LENGTH_DEFAULT, STRUCTURAL_STIFFNESS, DAMPING_DEFAULT );
	m_nodes[nodeVectorIndex]->AddSpring( m_nodes[nodeVectorIndex + 1], newSpring );
	m_springs.insert( newSpring );
}


//-----------------------------------------------------------------------------------------------
void Cloth::MakeAndConnectBelow( int nodeVectorIndex )
{
	Spring* newSpring = new Spring( m_nodes[ nodeVectorIndex ], m_nodes[ nodeVectorIndex + m_width ], Spring::SPRING_STRUCTURAL, UNSTRETCHED_LENGTH_DEFAULT, STRUCTURAL_STIFFNESS, DAMPING_DEFAULT );
	m_nodes[nodeVectorIndex]->AddSpring( m_nodes[nodeVectorIndex + m_width], newSpring );
	m_springs.insert( newSpring );
}



void Cloth::MakeAndConnectShearBR( int nodeVectorIndex )
{
	Spring* newSpring = new Spring( m_nodes[nodeVectorIndex], m_nodes[nodeVectorIndex + 1 + m_width], Spring::SPRING_SHEAR, SHEAR_UNSTRETECHED_LENGTH, SHEAR_STIFFNESS, DAMPING_DEFAULT );
	m_nodes[nodeVectorIndex]->AddSpring( m_nodes[nodeVectorIndex + 1 + m_width], newSpring );
	m_springs.insert( newSpring );
}

void Cloth::MakeAndConnectShearBL( int nodeVectorIndex )
{
	Spring* newSpring = new Spring( m_nodes[nodeVectorIndex], m_nodes[nodeVectorIndex + -1 + m_width], Spring::SPRING_SHEAR, SHEAR_UNSTRETECHED_LENGTH, SHEAR_STIFFNESS, DAMPING_DEFAULT );
	m_nodes[nodeVectorIndex]->AddSpring( m_nodes[nodeVectorIndex + -1 + m_width], newSpring );
	m_springs.insert( newSpring );
}

void Cloth::MakeAndConnectBendR( int nodeVectorIndex )
{
	Spring* newSpring = new Spring( m_nodes[nodeVectorIndex], m_nodes[nodeVectorIndex + 2], Spring::SPRING_BEND, BEND_UNSTRETECHED_LENGTH, BEND_STIFFNESS, DAMPING_DEFAULT );
	m_nodes[nodeVectorIndex]->AddSpring( m_nodes[nodeVectorIndex + 2], newSpring );
	m_springs.insert( newSpring );
}

void Cloth::MakeAndConnectBendB( int nodeVectorIndex )
{
	Spring* newSpring = new Spring( m_nodes[nodeVectorIndex], m_nodes[nodeVectorIndex + m_width + m_width], Spring::SPRING_BEND, BEND_UNSTRETECHED_LENGTH, BEND_STIFFNESS, DAMPING_DEFAULT );
	m_nodes[nodeVectorIndex]->AddSpring( m_nodes[nodeVectorIndex + m_width + m_width], newSpring );
	m_springs.insert( newSpring );
}

//-----------------------------------------------------------------------------------------------
void Cloth::AddSpring( Node* nodeA, Node* nodeB, Spring::SPRING_TYPE type, float unstretchedLength, float stiffness, float dampingCoefficient )
{
	Spring* newSpring = new Spring( nodeA, nodeB, type, unstretchedLength, stiffness, dampingCoefficient );
	m_springs.insert( newSpring );
}

void Cloth::SetPins()
{
	int colPinCount = 0;
	int raisedPinHeight = 0;
	int pinSpread = 3;

	for (int row = 1; row < m_height - 1; ++row)
	{
		for (int column = 1; column < m_width - 1; ++column)
		{
			int index = row * m_width + column;
			if (row % 4 == 0)
			{
				if (index % pinSpread == 0)
				{

					m_nodes[index]->m_position.z += raisedPinHeight;
					m_nodes[index]->m_prevPos.z += raisedPinHeight;
					m_nodes[index]->m_isFixed = true;
					m_fixedNodes.push_back(m_nodes[index]);
				}
			}
			else if (row % 4 == 2)
			{
				int mod = index;
				int mod2 = mod % pinSpread;
				if (mod2 == 0)
				{

					m_nodes[index]->m_position.z += raisedPinHeight;
					m_nodes[index]->m_prevPos.z += raisedPinHeight;
					m_nodes[index]->m_isFixed = true;
					m_fixedNodes.push_back(m_nodes[index]);
				}
			}
			else
			{
				//Do nothing!
// 				int mod = index + 0;
// 				int mod2 = mod % 4;
// 				if (mod2 == 0)
// 				{
// 
// 					m_nodes[index]->m_position.z += 1.f;
// 					m_nodes[index]->m_isFixed = true;
// 					m_fixedNodes.push_back(m_nodes[index]);
// 				}
			}
			
			colPinCount++;
		}
	}


}

void Cloth::AdjustStiffness(Spring::SPRING_TYPE springType, const float& amount)
{
	
	for (auto it = m_springs.begin(); it != m_springs.end(); ++it)
	{
		Spring& springRef = **it; 
		if (springRef.m_type == springType)
		{
			springRef.m_stiffness += amount;
		}
	}
}

void Cloth::AddVelocity( const Vector3Double& velocity )
{
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		Node& nodeRef = **it;
		if (!nodeRef.m_isFixed )
		{
			nodeRef.m_velocity += velocity;
			nodeRef.m_prevVelocity += velocity;
		}
	}
}

void Cloth::InstantDisplacement( const Vector3Double& displacement )
{
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		Node& nodeRef = **it;
		if (!nodeRef.m_isFixed)
		{
			nodeRef.m_position += displacement;
			nodeRef.m_prevPos += displacement;
		}
	}
}

Node* Cloth::GetNodeAtCoords(IntVector2 pos) const
{
	int index = pos.y * m_width + pos.x;

	if ( ( unsigned int ) index < m_nodes.size())
	{
		if (m_nodes[index]->m_coordinates == pos)
		{
			return m_nodes[index];
		}
	}
	return nullptr;
}

IntVector2 Cloth::GetCoordsAtIndex(int index) const
{
	IntVector2 pos;
	pos.x = index % m_width;
	pos.y = (index / m_width) % m_height;
	return pos;
}
