//-----------------------------------------------------------------------------------------------
// Camera2D.hpp
// 2D camera used to follow player tank


#pragma once

typedef Vector2 WorldPosition;

//-----------------------------------------------------------------------------------------------
class Camera2D
{
public:
	// Constructors
	Camera2D();

	// Accessors
	WorldPosition GetWorldPosition() const;
	float GetOrientationDegrees() const;
	float GetNumTilesInViewVertically() const;

	// Mutators
	void SetWorldPosition( WorldPosition newWorldPosition );
	void SetOrientationDegrees( float newOrientationDegrees );
	void SetNumTilesInViewVertically( float newNumTilesInViewVertically );

private:
	// Members
	WorldPosition m_worldPosition;
	float m_orientationDegrees;
	float m_numTilesInViewVertically;
};