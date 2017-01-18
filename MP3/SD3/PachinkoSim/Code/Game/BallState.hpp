#pragma once

#include "Engine/Math/Vector3.hpp"


//-------------------------------------------------------------------------------------------------
struct BallState
{
public:
	Vector3 m_position;
	Vector3 m_velocity;
public:
	BallState(Vector3 const &position, Vector3 const &velocity);
};