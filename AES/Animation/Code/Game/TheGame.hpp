#pragma once

#include "Engine/Renderer/Cameras/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Meshes/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Animation/Skeleton.hpp"
#include "Engine/Animation/Motion.hpp"


//-----------------------------------------------------------------------------------------------
class TheGame;
class MeshRenderer;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	TheGame();
	~TheGame();
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
	void ClearBuilderPointers( MeshBuilder*& meshBuilder, MeshRenderer*& modelRenderer, Mesh*& mesh );
	void PopulateJointMeshRenderer( Skeleton* skeleton );

public:
	float m_age;

	int m_lightNum;
	int m_maxLights;
	std::vector< Light > m_lights;
	const Mesh *m_cubeMesh;
	const Mesh *m_sphereMesh;
	Mesh *m_planeMesh;
	MeshBuilder *m_meshBuilder;
	GPUProgram *m_programSolidColor;
	GPUProgram *m_programDot3;
	Material *m_materialWhite;
	Material *m_materialOrange;
	Material *m_materialLava;
	Material *m_materialPlain;
	std::vector< MeshRenderer* > m_cubeMeshRenderer;
	std::vector< MeshRenderer* > m_lightMeshRenderer;

	MeshBuilder* m_modelMeshBuilder;
	MeshRenderer* m_modelRenderer;
	Mesh* m_modelMesh;

	Skeleton* m_skeleton;
	std::vector< MeshRenderer* > m_jointMeshRenderer;

	Motion* m_motion;

	std::vector< mat44_fl > m_boneMatrices;
	float m_modelScale;
};