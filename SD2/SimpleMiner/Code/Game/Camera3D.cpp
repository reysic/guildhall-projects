#include "Engine/Math/MathUtils.hpp"
#include "Game/Camera3D.hpp"


//-----------------------------------------------------------------------------------------------
Camera3D::Camera3D()
	: m_position( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_orientation( EulerAngles( 0.0f, 0.0f, 0.0f ) )
{
}


//-----------------------------------------------------------------------------------------------
void Camera3D::FixAndClampAngles()
{
	while ( m_orientation.m_yawDegreesAboutZ < 0.0f )
	{
		m_orientation.m_yawDegreesAboutZ += 360.0f;
	}

	while ( m_orientation.m_yawDegreesAboutZ >= 360.0f )
	{
		m_orientation.m_yawDegreesAboutZ -= 360.0f;
	}

	m_orientation.m_pitchDegreesAboutY = ClampFloat( m_orientation.m_pitchDegreesAboutY, -89.9f, 89.9f );
	m_orientation.m_rollDegreesAboutX = ClampFloat( m_orientation.m_rollDegreesAboutX, -89.9f, 89.9f );
}


//-----------------------------------------------------------------------------------------------
Vector3 Camera3D::GetForwardXYZ() const
{
	float cosYaw = CosDegrees( m_orientation.m_yawDegreesAboutZ );
	float sinYaw = SinDegrees( m_orientation.m_yawDegreesAboutZ );

	float cosPitch = CosDegrees( m_orientation.m_pitchDegreesAboutY );
	float sinPitch = SinDegrees( m_orientation.m_pitchDegreesAboutY );

	return Vector3( cosYaw * cosPitch, sinYaw * cosPitch, -sinPitch );
}


//-----------------------------------------------------------------------------------------------
Vector3 Camera3D::GetForwardXY() const
{
	float cosYaw = CosDegrees( m_orientation.m_yawDegreesAboutZ );
	float sinYaw = SinDegrees( m_orientation.m_yawDegreesAboutZ );
	return Vector3( cosYaw, sinYaw, 0.0f );
}


//-----------------------------------------------------------------------------------------------
Vector3 Camera3D::GetLeftXY() const
{
	Vector3 forwardXY = GetForwardXY();
	return Vector3( -forwardXY.y, forwardXY.x, 0.0f );
}