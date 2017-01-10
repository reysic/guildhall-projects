#pragma once
#include "Game/Player.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <map>
#include <deque>

const IntVector2 snapBackPos = IntVector2(800, 450);
const float mouseSensitivity = 0.025f;
const float DIST_TO_RAYCAST = 8.f;
const int ACTIVE_CHUNK_RADIUS = 14;
const int FLUSH_CHUNK_RADIUS = 16;
const float MIN_FLUSH_DIST = 16.f * FLUSH_CHUNK_RADIUS;
const float MIN_FLUSH_DIST_SQUARED = MIN_FLUSH_DIST * MIN_FLUSH_DIST;
const float MIN_ACTIVE_DIST = 16.f * ACTIVE_CHUNK_RADIUS;
const float MIN_ACTIVE_DIST_SQUARED = MIN_ACTIVE_DIST * MIN_ACTIVE_DIST;
const float DIG_TIME_SOUND_THRESHOLD = 0.19f;
const float FACE_SELECTION_DIFF = 0.01f;




class World
{
public:
	World();
	Player m_player;
	Camera3D m_camera;
	float m_moveMod;
	Vector3 m_startTrace;
	Vector3 m_endTrace;


	void Update(float deltaSeconds);
	void UpdateCameraAndPlayer(float deltaSeconds);
	void Render();
};