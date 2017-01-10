#pragma once

#include "Engine/Renderer/Cameras/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Meshes/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	TheGame();
	void MakeAxisLines();
	void MakeCubeMesh();
	void MakeSphereMesh();
	void SetTextures();
	void Update( float deltaSeconds );
	void UpdateFPSCounter( float deltaSeconds );
	void HandleMouseAndKeyboardInput();
	void ClearCameraMoveStates();
	void Render() const;
	void SetUp3DView() const;
	void SetUp2DView() const;
	void DrawScene() const;
	void DrawDebugStuff3D() const;
	void DrawHUD() const;
	void DrawDebugStuff2D() const;
	void UpdateCameraMovement( float deltaSeconds );
	void KillVelocityInDirection( const Vector3& normalVector );

public:
	Mesh* m_lineX;
	Mesh* m_lineY;
	Mesh* m_lineZ;
	Mesh* m_cube;
	Mesh* m_sphere;
	float m_age;
	GLuint m_diffuseTex;
	GLuint m_normalTex;
	GLuint m_specularTex;
	GLuint m_emissiveTex;
	GLuint m_dissolveTex;
	GLuint m_maskTex;
	Material* m_fvfMaterial;
	Material* m_cubeMaterial;
	Material* m_sphereMaterial;
};