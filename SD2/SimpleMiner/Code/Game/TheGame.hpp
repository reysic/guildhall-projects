#pragma once

#include "Game/Camera3D.hpp"
#include "Game/Player3D.hpp"
#include "Game/World.hpp"
#include "Game/BlockInfo.hpp"


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
enum PhysicsMode
{
	PHYSICS_MODE_WALKING,	// 0
	PHYSICS_MODE_FLYING,	// 1
	PHYSICS_MODE_NOCLIP,	// 2
	NUM_PHYSICS_MODES
};


//-----------------------------------------------------------------------------------------------
enum CameraMode
{
	CAMERA_MODE_FIRST_PERSON, // 0
	CAMERA_MODE_FROM_BEHIND,  // 1
	CAMERA_MODE_FIXED_ANGLE,  // 2
	NUM_CAMERA_MODES
};


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	TheGame();
	void Update( float deltaSeconds );
	void HandleMouseAndKeyboardInput();
	void ClearPlayerMoveStates();
	void UpdateCameraPositionAndOrientation();
	void Render() const;
	void SetUp3DView() const;
	void SetUp2DView() const;
	void DrawDebugStuff3D() const;
	void DrawHUD() const;
	void DrawDebugStuff2D() const;
	void LoadPlayerFromDisk();
	void FixAndClampPlayerAngles();
	Vector3 GetPlayerForwardXYZ() const;
	Vector3 GetPlayerForwardXY() const;
	Vector3 GetPlayerLeftXY() const;
	void UpdatePlayerMovement( float deltaSeconds );
	void DoPreventativePhysics( float deltaSeconds );
	BlockInfo GetBlockInfoForWorldPosition( const Vector3& worldPosition );
	void IncrementHUDSelectedBlockType();
	void DecrementHUDSelectedBlockType();
	void UpdateBoundingBoxAndVerts();
	void KillVelocityInDirection( const Vector3& normalVector );

public:
	Camera3D m_camera;
	World* m_world;
	unsigned char m_physicsMode;
	unsigned char m_cameraMode;
	Player3D m_player;
};