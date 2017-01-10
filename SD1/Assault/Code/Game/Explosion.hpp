//-----------------------------------------------------------------------------------------------
// Explosion.hpp
// Renders explosion animation


//-----------------------------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------------------------
#include <string>


//-----------------------------------------------------------------------------------------------
class SpriteSheet;
class SpriteAnimation;


//-----------------------------------------------------------------------------------------------
const std::string EXPLOSION_SPRITE_SHEET_FILE = "Data/Images/Explosion_5x5.png";


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