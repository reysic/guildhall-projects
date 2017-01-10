#pragma once


//-----------------------------------------------------------------------------------------------
// Forward Declaractions
class SpriteSheet;
class SpriteAnimation;


//-----------------------------------------------------------------------------------------------
class Explosion
{
public:
	// Constructors & Destructors
	Explosion( Vector2 explosionLocation, float sizeScalingFactor, float durationInSeconds );
	~Explosion();

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
	bool IsFinished() const { return m_isFinished; };

private:
	Vector2 m_position;
	float m_sizeScalingFactor;
	float m_ageInSeconds;
	float m_duration;
	AABB2 m_bounds;
	SpriteSheet* m_explosionSpriteSheet;
	SpriteAnimation* m_explosionSpriteAnimation;
	bool m_isFinished;
};