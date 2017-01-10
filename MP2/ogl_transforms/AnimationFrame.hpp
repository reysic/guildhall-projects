#pragma once

#include "Quaternion.hpp"
#include "AxisAngle.hpp"

class AnimationFrame {
public:
	AnimationFrame();
	AnimationFrame(const AxisAngle& a1, const AxisAngle& a2) : q1(a1), q2(a2) {}

public:
	AxisAngle q1;
	AxisAngle q2;
};