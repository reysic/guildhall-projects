#pragma once


//-----------------------------------------------------------------------------------------------
const float HIT_ANIMATION_LENGTH_SEC = 0.25f;


//-----------------------------------------------------------------------------------------------
class Tile
{
public:
	// Constructors & Destructors
	Tile( int numHitsRemaining, const IntVector2& gridPosition );

	// Methods
	void Update( float deltaSeconds );
	void UpdateTint();
	void UpdateHitAnimation( float deltaSeconds );
	void Render();
	void StartHitAnimation();

public:
	int m_numHitsRemaining;
	IntVector2 m_gridPosition;
	Vector2 m_worldPosition;

private:
	Rgba m_tint;
	bool m_isAnimating;
	float m_animationStartTime;
	float m_quadRenderOffset;
};