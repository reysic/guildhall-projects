#include "Engine/Core/Camera3D.hpp"
#include "Engine/MathUtils.hpp"


Camera3D::Camera3D()
{
	m_position.SetXYZ(0.f, 0.f, 0.f);
	m_orientation = EulerAngles::ZERO;
}

Vector3 Camera3D::GetForwardXYZ()
{
	//Yaw = theta
	//phi = pitch nose down
	//(cos theta)*(cos phi),(sin theta)*(cos phi),(-sin phi)

	float cosYaw = CosDegrees(m_orientation.m_yawDegreesAboutZ);
	float sinYaw = SinDegrees(m_orientation.m_yawDegreesAboutZ);
	float cosPitch = CosDegrees(m_orientation.m_pitchDegreesAboutX);
	float sinPitch = SinDegrees(m_orientation.m_pitchDegreesAboutX);
	return Vector3((cosYaw*cosPitch), (sinYaw * cosPitch), -sinPitch);

}

Vector3 Camera3D::GetForwardXY()
{
	float cosYaw = CosDegrees(m_orientation.m_yawDegreesAboutZ);
	float sinYaw = SinDegrees(m_orientation.m_yawDegreesAboutZ);
	return Vector3(sinYaw, cosYaw, 0.f);
}

Vector3 Camera3D::GetLeftXY()
{
	Vector3 forwardXY = GetForwardXY();
	return Vector3(-forwardXY.y, forwardXY.x, 0.f);
}

void Camera3D::FixAndClampAngles()
{

	m_orientation.m_pitchDegreesAboutX = ClampFloat(m_orientation.m_pitchDegreesAboutX, -90.f, 90.f);
	m_orientation.m_rollDegreesAboutY = ClampFloatCircular(m_orientation.m_rollDegreesAboutY, 0.f, 360.f);
	m_orientation.m_yawDegreesAboutZ = ClampFloatCircular(m_orientation.m_yawDegreesAboutZ, 0.f, 360.f);

}

