#pragma once

#include <vector>
#include <set>

#include "Engine/Math/Vector3Double.hpp"
#include "Game/Node.hpp"
#include "Game/Spring.hpp"
#include "Engine/Renderer/Texture.hpp"


//-----------------------------------------------------------------------------------------------
class Cloth
{
public:
	Cloth();
	Cloth( const Vector3Double& topLeftClothPosition );
	void Update( float deltaSeconds );
	void Render() const;
	void MakeAndConnectRight(  const int& nodeVectorIndex );
	void MakeAndConnectBelow( int nodeVectorIndex );
	void MakeAndConnectShearBR( int nodeVectorIndex );
	void MakeAndConnectShearBL( int nodeVectorIndex );
	void MakeAndConnectBendR( int nodeVectorIndex );
	void MakeAndConnectBendB( int nodeVectorIndex );
	void AddSpring( Node* nodeA, Node* nodeB, Spring::SPRING_TYPE type, float unstretchedLength, float stiffness, float dampingCoefficient );
	void SetPins();


	void AdjustStiffness( Spring::SPRING_TYPE springType, const float& amount );
	void AddVelocity( const Vector3Double& velocity );
	void InstantDisplacement( const Vector3Double& displacement );

	Node* GetNodeAtCoords(IntVector2 pos) const;
	IntVector2 GetCoordsAtIndex(int index) const;


public:
	std::vector< Node* > m_nodes;
	std::set< Spring* > m_springs;
	std::vector <Node*> m_fixedNodes;

	int m_width;
	int m_height;
	Vector3Double m_topLeftClothPosition;
	Vector3Double m_minXY;
	Vector3Double m_maxXY;
	Texture* m_clothTexture1;
	Texture* m_clothTexture2;
	Texture* m_clothTexture3;
	Texture* m_clothTexture4;

	
};