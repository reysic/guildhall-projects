#pragma once


class AimLine
{
public:
	// Constructors & Destructors
	AimLine();

	// Methods
	void Render() const;

	// Accessors & Mutators
	void SetLineStationPosition( const Vector2& lineStartPosition ) { m_lineStartPosition = lineStartPosition; }
	void SetForwardDirection( const Vector2& forwardDirection ) { m_forwardDirection = forwardDirection; }

private:
	Vector2 m_lineStartPosition;
	Vector2 m_forwardDirection;
};