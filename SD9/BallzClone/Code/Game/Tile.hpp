#pragma once


//-----------------------------------------------------------------------------------------------
class Tile
{
public:
	// Constructors & Destructors
	Tile( int numHitsRemaining, const IntVector2& gridPosition );

	// Methods
	void Update( float deltaSeconds );
	void Render();

public:
	int m_numHitsRemaining;
	IntVector2 m_gridPosition;
	Vector2 m_worldPosition;

private:
	Rgba m_tint;
};