#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2;


//-----------------------------------------------------------------------------------------------
class IndicatorBall
{
public:
	// Constructors & Destructors
	IndicatorBall();

	// Methods
	void Update( float deltaSeconds );
	void Render() const;

public:
	Vector2		m_position;
	Vector2		m_velocity;
	float		m_radius;
	Rgba		m_tint;
};