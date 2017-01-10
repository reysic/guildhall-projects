#include "Engine/MathUtils.hpp"
#include "Engine/Renderer/Shaders/Mesh.hpp"


//-----------------------------------------------------------------------------------------------
Mesh::Mesh()
{
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeCubeMesh()
{
	// Add all vertexes to the mesh for VAO
	// Front face
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, -.5f, .5f), Rgba::WHITE, Vector2(0.f, 0.f))); //tl
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, -.5f, -.5f), Rgba::WHITE, Vector2(0.f, 1.f))); //bl
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, -.5f, -.5f), Rgba::WHITE, Vector2(1.f, 1.f))); //br
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, -.5f, .5f), Rgba::WHITE, Vector2(1.f, 0.f))); //tr

																							//  Back face
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, .5f, .5f), Rgba::WHITE, Vector2(0.f, 0.f))); //tl
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, .5f, -.5f), Rgba::WHITE, Vector2(0.f, 1.f))); //bl
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, .5f, -.5f), Rgba::WHITE, Vector2(1.f, 1.f))); //br
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, .5f, .5f), Rgba::WHITE, Vector2(1.f, 0.f))); //tr

																							// Top face
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, .5f, .5f), Rgba::WHITE, Vector2(0.f, 0.f))); //tl
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, -.5f, .5f), Rgba::WHITE, Vector2(0.f, 1.f))); //bl
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, -.5f, .5f), Rgba::WHITE, Vector2(1.f, 1.f))); //br
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, .5f, .5f), Rgba::WHITE, Vector2(1.f, 0.f))); //tr

																						   // Bottom Face
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, .5f, -.5f), Rgba::WHITE, Vector2(0.f, 0.f))); //tr
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, -.5f, -.5f), Rgba::WHITE, Vector2(0.f, 1.f))); //br
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, -.5f, -.5f), Rgba::WHITE, Vector2(1.f, 1.f))); //bl
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, .5f, -.5f), Rgba::WHITE, Vector2(1.f, 0.f))); //tl

																							 // Right face
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, .5f, .5f), Rgba::WHITE, Vector2(1.f, 0.f))); //tr
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, -.5f, .5f), Rgba::WHITE, Vector2(0.f, 0.f))); //br
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, -.5f, -.5f), Rgba::WHITE, Vector2(0.f, 1.f))); //bl
	m_verts.push_back(Vertex3D_PCT(Vector3(.5f, .5f, -.5f), Rgba::WHITE, Vector2(1.f, 1.f))); //tl

																							// Left face
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, .5f, -.5f), Rgba::WHITE, Vector2(0.f, 1.f))); //tl
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, -.5f, -.5f), Rgba::WHITE, Vector2(1.f, 1.f))); //bl
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, -.5f, .5f), Rgba::WHITE, Vector2(1.f, 0.f))); //br
	m_verts.push_back(Vertex3D_PCT(Vector3(-.5f, .5f, .5f), Rgba::WHITE, Vector2(0.f, 0.f))); //tr

	// Add all indices to the mesh for IBO
	m_indices = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};
}


//-----------------------------------------------------------------------------------------------
void Mesh::MakeSphereMesh()
{
	// Add all vertexes to the mesh for VAO, add all indices to the mesh for IBO
	float radius = 1.0f;
	int numRings = 100;
	int numSectors = 100;
	int currentRow = 0;
	int nextRow = 0;
	int nextSector;

	float const R = 1.0f / (float)(numRings - 1);
	float const S = 1.0f / (float)(numSectors - 1);

	for (int r = 0; r < numRings; ++r)
	{
		for (int s = 0; s < numSectors; ++s)
		{
			float const z = sin(-pi / 2 + pi * r * R);
			float const y = cos(2 * pi * s * S) * sin(pi * r * R);
			float const x = sin(2 * pi * s * S) * sin(pi * r * R);

			m_verts.push_back(Vertex3D_PCT(Vector3(x * radius + 5.f, y * radius, -z * radius), Rgba::WHITE, Vector2 (1 - s * S, r * R)));

			currentRow = r * numSectors;
			nextRow = (r + 1) * numSectors;
			nextSector = (s + 1) % numSectors;

			if (r < numRings - 1)
			{
				m_indices.push_back((uint16_t)(nextRow + nextSector));
				m_indices.push_back((uint16_t)(nextRow + s));
				m_indices.push_back((uint16_t)(currentRow + s));

				m_indices.push_back((uint16_t)(currentRow + nextSector));
				m_indices.push_back((uint16_t)(nextRow + nextSector));
				m_indices.push_back((uint16_t)(currentRow + s));
			}
		}
	}
}