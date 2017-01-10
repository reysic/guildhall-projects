#include "Game/Player.hpp"
#include "Game/TheGame.hpp"
#include "Game/TheApp.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"

const float MOVE_SPEED = 2.f; //Blocks per Seconds
const float VERT_MOD = 1.5f;

const float PLAYER_HEIGHT = 1.85f;
const float PLAYER_WIDTH = 0.6f;
const float PLAYER_EYE_HEIGHT = 1.62f;
const float PHYS_MOVE_SPEED = 40.f;

Player::Player()
{
	m_pos = Vector3(0.f, 0.f, 0.f);

	g_camera = Camera3D();
	g_camera.m_position = m_pos;
	Vector3 mins = Vector3(m_pos.x - (PLAYER_WIDTH / 2.f), m_pos.y - (PLAYER_WIDTH / 2.f), m_pos.z - (PLAYER_HEIGHT / 2.f));
	Vector3 maxs = Vector3(m_pos.x + (PLAYER_WIDTH / 2.f), m_pos.y + (PLAYER_WIDTH / 2.f), m_pos.z + (PLAYER_HEIGHT / 2.f));
	m_bounds = AABB3(mins, maxs);
	m_moveMod = 1.f;
	m_velocity = Vector3(0.f, 0.f, 0.f);

	m_isMovingForward = false;
	m_isMovingBackwards = false;
	m_isMovingLeft = false;
	m_isMovingRight = false;
	m_isMovingUp = false;
	m_isMovingDown = false;
	m_isSprinting = false;
	m_canAddMoveDelta = false;
	m_deltaMove = 0.f;

}

Player::Player(Vector3 position)
{
	m_pos = position;

	g_camera = Camera3D();
	g_camera.m_position = m_pos;
	Vector3 mins = Vector3(m_pos.x - (PLAYER_WIDTH / 2.f), m_pos.y - (PLAYER_WIDTH / 2.f), m_pos.z - (PLAYER_HEIGHT / 2.f));
	Vector3 maxs = Vector3(m_pos.x + (PLAYER_WIDTH / 2.f), m_pos.y + (PLAYER_WIDTH / 2.f), m_pos.z + (PLAYER_HEIGHT / 2.f));
	m_bounds = AABB3(mins, maxs);
	m_moveMod = 1.f;
	m_velocity = Vector3(0.f, 0.f, 0.f);

	m_isMovingForward = false;
	m_isMovingBackwards = false;
	m_isMovingLeft = false;
	m_isMovingRight = false;
	m_isMovingUp = false;
	m_isMovingDown = false;
	m_isSprinting = false;
	m_canAddMoveDelta = false;
	m_deltaMove = 0.f;
}

void Player::Update(float deltaSeconds)
{
	MoveAndUpdateBounds(m_pos);
	m_canAddMoveDelta = false;
	if (m_isSprinting)
	{
		m_moveMod = 4.f;
	}
	else
	{
		m_moveMod = 1.f;
	}

	
	if (m_isMovingForward)
	{
		m_pos += g_camera.GetForwardXY() * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	else if (m_isMovingBackwards)
	{
		m_pos -= g_camera.GetForwardXY() * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	if (m_isMovingLeft)
	{
		m_pos += g_camera.GetLeftXY() * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	else if (m_isMovingRight)
	{
		m_pos -= g_camera.GetLeftXY() * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	if (m_isMovingUp)
	{
		m_pos.z += VERT_MOD * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	else if (m_isMovingDown)
	{
		m_pos.z -= VERT_MOD * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	//m_pos = GetPosForCameraPos(world->m_camera.m_position);
	UpdateCamera();
	MoveAndUpdateBounds(m_pos);
	
}

void Player::ClearMoveStates()
{
	m_isMovingForward = false;
	m_isMovingBackwards = false;
	m_isMovingLeft = false;
	m_isMovingRight = false;
	m_isMovingUp = false;
	m_isMovingDown = false;
	m_isSprinting = false;
}

void Player::MoveAndUpdateBounds(Vector3 position)
{
	m_pos = position;
	m_bounds = GetBoundsForPos(m_pos);
	m_curVerts[0] = Vector3(m_bounds.m_mins.x, m_bounds.m_mins.y, m_bounds.m_mins.z);
	m_curVerts[1] = Vector3(m_bounds.m_mins.x, m_bounds.m_maxs.y, m_bounds.m_mins.z);
	m_curVerts[2] = Vector3(m_bounds.m_maxs.x, m_bounds.m_maxs.y, m_bounds.m_mins.z);
	m_curVerts[3] = Vector3(m_bounds.m_maxs.x, m_bounds.m_mins.y, m_bounds.m_mins.z);

	//Midsection, counter clockwise
	m_curVerts[4] = Vector3(m_bounds.m_mins.x, m_bounds.m_mins.y, (m_bounds.m_mins.z + (PLAYER_HEIGHT / 2.f)));
	m_curVerts[5] = Vector3(m_bounds.m_mins.x, m_bounds.m_maxs.y, (m_bounds.m_mins.z + (PLAYER_HEIGHT / 2.f)));
	m_curVerts[6] = Vector3(m_bounds.m_maxs.x, m_bounds.m_maxs.y, (m_bounds.m_mins.z + (PLAYER_HEIGHT / 2.f)));
	m_curVerts[7] = Vector3(m_bounds.m_maxs.x, m_bounds.m_mins.y, (m_bounds.m_mins.z + (PLAYER_HEIGHT / 2.f)));

	//Top face, counter clockwise
	m_curVerts[8] = Vector3(m_bounds.m_mins.x, m_bounds.m_mins.y, m_bounds.m_maxs.z);
	m_curVerts[9] = Vector3(m_bounds.m_mins.x, m_bounds.m_maxs.y, m_bounds.m_maxs.z);
	m_curVerts[10] = Vector3(m_bounds.m_maxs.x, m_bounds.m_maxs.y, m_bounds.m_maxs.z);
	m_curVerts[11] = Vector3(m_bounds.m_maxs.x, m_bounds.m_mins.y, m_bounds.m_maxs.z);
}

void Player::UpdateCamera() //Using position, calculate the camera position
{
	World* world = g_theGame->m_world;
	g_camera.m_position.SetXYZ(m_pos.x, m_pos.y, m_pos.z);

	//world->m_camera.m_position.SetXYZ(m_pos.x, m_pos.y, (m_pos.z - (PLAYER_HEIGHT / 2.f) + PLAYER_EYE_HEIGHT));
	world->m_camera.m_position = g_camera.m_position;
	world->m_camera.m_orientation = g_camera.m_orientation;
}

AABB3 Player::GetBoundsForPos(Vector3 position)
{
	Vector3 mins = Vector3(position.x - (PLAYER_WIDTH / 2.f), position.y - (PLAYER_WIDTH / 2.f), position.z - (PLAYER_HEIGHT / 2.f));
	Vector3 maxs = Vector3(position.x + (PLAYER_WIDTH / 2.f), position.y + (PLAYER_WIDTH / 2.f), mins.z + PLAYER_HEIGHT);
	return AABB3(mins, maxs);
}

AABB3 Player::GetBoundsForCameraPos(Vector3 position)
{
	Vector3 mins = Vector3(position.x - (PLAYER_WIDTH / 2.f), position.y - (PLAYER_WIDTH / 2.f), position.z - PLAYER_EYE_HEIGHT);
	Vector3 maxs = Vector3(position.x + (PLAYER_WIDTH / 2.f), position.y + (PLAYER_WIDTH / 2.f), mins.z + PLAYER_HEIGHT);
	return AABB3(mins, maxs);
}

Vector3 Player::GetPosForCameraPos(Vector3 position)
{
	Vector3 result = Vector3(position.x, position.y, position.z - PLAYER_EYE_HEIGHT + (PLAYER_HEIGHT / 2.f));
	return result;
}