#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2; // Forward declaration of Vector2


//-----------------------------------------------------------------------------------------------
const float PLAYER_BALL_SPEED = 750.0f;
const std::string PLAYER_DISC_TEXTURE_FILE = "Data/Images/Disc";
const int PLAYER_DISC_STARTING_HEALTH = 3;


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
	float		m_orientationDegrees;
	float		m_angularVelocityDegreesPerSecond;
	float		m_physicsRadius;
	float		m_cosmeticRadius;
	float		m_ageInSeconds;
	bool		m_isDead;
	Rgba		m_tint;
	Texture*	m_texture;
	int			m_health;
	AABB2		m_bounds;

private:
	bool		m_canDrawAimLine;
	Vector2		m_forwardDirection;
};