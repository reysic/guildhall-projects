#pragma once

#include "Vector3.hpp"

struct AxisAngle {
	AxisAngle();
	AxisAngle(float angle, const Vector3& axis) : m_angle(angle), m_axis(axis) {}

	float m_angle;
	Vector3 m_axis;
};