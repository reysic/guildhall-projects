#pragma once

#include "Engine/Math/AABB3.hpp"
#include "GameCommon.hpp"


//-----------------------------------------------------------------------------------------------
class Player3D
{
public:
	Player3D();

public:
	Vector3 m_position;
	Vector3 m_velocity;
	bool m_movingForward;
	bool m_movingBackward;
	bool m_movingLeft;
	bool m_movingRight;
	bool m_movingUp;
	bool m_movingDown;
	EulerAngles m_orientation;
	AABB3 m_boundingBox;
};