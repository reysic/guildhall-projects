#pragma once


//-----------------------------------------------------------------------------------------------
class PowerUp
{
public:
	// Constructors & Destructors
	PowerUp( const IntVector2& gridPosition );

	// Methods
	void Update( float deltaSeconds );
	void Render() const;

public:
	IntVector2	m_gridPosition;
	Vector2		m_worldPosition;
	float		m_radius;
};