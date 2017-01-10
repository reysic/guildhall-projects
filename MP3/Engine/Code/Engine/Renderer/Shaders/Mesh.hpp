#pragma once

#include <vector>

#include "Engine/Renderer/VertexPCT.hpp"


//-----------------------------------------------------------------------------------------------
class Mesh
{
public:
	Mesh();
	void MakeCubeMesh();
	void MakeSphereMesh();

public:
	std::vector< Vertex3D_PCT > m_verts;
	std::vector< uint16_t > m_indices;
};