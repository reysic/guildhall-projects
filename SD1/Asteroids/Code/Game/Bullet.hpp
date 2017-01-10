#pragma once


//-----------------------------------------------------------------------------------------------
const float BULLET_SPEED = 600.0f;


//-----------------------------------------------------------------------------------------------
class Bullet : public Entity
{
public:
	Bullet( const Vector2& shipPosition, float shipOrientationDegrees, float shipCosmeticRadius );
	void Update( float deltaSeconds );
	void Render() const;

private:
};