#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2;


//-----------------------------------------------------------------------------------------------
const float PLAYER_BALL_SPEED = 750.0f;
const Vector2 PLAYER_BALL_INITIAL_POSITION( 350.0f, 10.0f );


//-----------------------------------------------------------------------------------------------
class PlayerBall
{
public:
	// Constructors & Destructors
	PlayerBall();

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
	void CheckForScreenEdgeCollision();

public:
	Vector2		m_position;
	Vector2		m_velocity;
	float		m_radius;
	Rgba		m_tint;

private:
	bool		m_canDrawAimLine;
	Vector2		m_forwardDirection;
};