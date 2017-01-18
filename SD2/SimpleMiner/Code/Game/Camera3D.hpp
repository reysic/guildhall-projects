#pragma once

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/EulerAngles.hpp"


//-----------------------------------------------------------------------------------------------
class EulerAngles;


//-----------------------------------------------------------------------------------------------
class Camera3D
{
public:
	Camera3D();
	void FixAndClampAngles();
	Vector3 GetForwardXYZ() const;
	Vector3 GetForwardXY() const;
	Vector3 GetLeftXY() const;

public:
	Vector3			m_position;
	EulerAngles		m_orientation;
};