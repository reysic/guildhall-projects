#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/EulerAngles.hpp"

class Camera3D
{
public:
	Camera3D();
	Vector3 GetForwardXYZ();
	Vector3 GetForwardXY();
	Vector3 GetLeftXY();
	void FixAndClampAngles();

	Vector3 m_position;
	EulerAngles m_orientation;
};