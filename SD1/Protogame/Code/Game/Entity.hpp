#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2;

//-----------------------------------------------------------------------------------------------
class Entity
{
public:
	Entity();
	virtual void Update( float deltaSeconds );
	void Move( float deltaSeconds );
	virtual void Render() const;

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
};