#pragma once

class EulerAngles
{
public:
	EulerAngles();
	EulerAngles(float pitchDegreesAboutX, float rollDegreesAboutY, float yawDegreesAboutZ);

	static const EulerAngles ZERO;

	float m_pitchDegreesAboutX;
	float m_rollDegreesAboutY;
	float m_yawDegreesAboutZ;
};