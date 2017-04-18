#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2;


//-----------------------------------------------------------------------------------------------
class PlayerBall
{
public:
	// Constructors & Destructors
	PlayerBall( const Vector2& spawnPosition );

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
	void CheckForScreenEdgeCollision();

public:
	Vector2		m_position;
	Vector2		m_velocity;
	float		m_radius;
	Rgba		m_tint;
	bool		m_hasBeenShotThisTurn;
};