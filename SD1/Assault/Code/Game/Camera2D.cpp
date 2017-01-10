//-----------------------------------------------------------------------------------------------
// Camera2D.cpp
// 2D camera used to follow player tank


//-----------------------------------------------------------------------------------------------
#include "Engine/Math/Vector2.hpp"
#include "Game/Camera2D.hpp"


//-----------------------------------------------------------------------------------------------
Camera2D::Camera2D()
	: m_worldPosition( Vector2( 0.f, 0.f ) )
	, m_orientationDegrees( 0.f )
	, m_numTilesInViewVertically( 10 )
{
}


//-----------------------------------------------------------------------------------------------
WorldPosition Camera2D::GetWorldPosition() const
{
	return m_worldPosition;
}


//-----------------------------------------------------------------------------------------------
float Camera2D::GetOrientationDegrees() const
{
	return m_orientationDegrees;
}


//-----------------------------------------------------------------------------------------------
float Camera2D::GetNumTilesInViewVertically() const
{
	return m_numTilesInViewVertically;
}


//-----------------------------------------------------------------------------------------------
void Camera2D::SetWorldPosition( WorldPosition newWorldPosition )
{
	m_worldPosition = newWorldPosition;
}


//-----------------------------------------------------------------------------------------------
void Camera2D::SetOrientationDegrees( float newOrientationDegrees )
{
	m_orientationDegrees = newOrientationDegrees;
}


//-----------------------------------------------------------------------------------------------
void Camera2D::SetNumTilesInViewVertically( float newNumTilesInViewVertically )
{
	m_numTilesInViewVertically = newNumTilesInViewVertically;
}
