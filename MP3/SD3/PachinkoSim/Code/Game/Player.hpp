#pragma once
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/Camera3D.hpp"
//#include "Game/GameCommon.hpp"
//#include "Engine/Audio/Audio.hpp"

class Player
{
public:
	Player();
	Player(Vector3 position);

	Vector3 m_pos;
	Camera3D m_playerCam;
	Vector3 m_velocity;
	float m_moveMod;
	AABB3 m_bounds;
	Vector3 m_curVerts[12];

	bool m_isMovingForward;
	bool m_isMovingBackwards;
	bool m_isMovingLeft;
	bool m_isMovingRight;
	bool m_isMovingUp;
	bool m_isMovingDown;
	bool m_isSprinting;
	bool m_canAddMoveDelta;

	float m_deltaMove;
	float m_footstepThreshold;

	void Update(float deltaSeconds);
	void ClearMoveStates();

	void MoveAndUpdateBounds(Vector3 position);
	void PushPlayerCardinal();
	void KillVelocity(Vector3 displacementVector);


	void UpdateCamera();


	AABB3 GetBoundsForPos(Vector3 position);
	AABB3 GetBoundsForCameraPos(Vector3 position);
	Vector3 GetPosForCameraPos(Vector3 position);
};