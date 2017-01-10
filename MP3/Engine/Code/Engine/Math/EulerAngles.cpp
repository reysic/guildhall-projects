#include "Engine/Math/EulerAngles.hpp"

const EulerAngles EulerAngles::ZERO = EulerAngles(0.f, 0.f,0.f);

EulerAngles::EulerAngles(float pitchDegreesAboutX, float rollDegreesAboutY, float yawDegreesAboutZ)
{
	m_pitchDegreesAboutX = pitchDegreesAboutX;
	m_rollDegreesAboutY = rollDegreesAboutY;
	m_yawDegreesAboutZ = yawDegreesAboutZ;
}

EulerAngles::EulerAngles()
{
	m_pitchDegreesAboutX = 0.f;
	m_rollDegreesAboutY = 0.f;
	m_yawDegreesAboutZ = 0.f;
}

