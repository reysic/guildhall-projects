//-----------------------------------------------------------------------------------------------
// PlayerBullet.hpp
// Bullet fired by player tank


#pragma once


//-----------------------------------------------------------------------------------------------
const float PLAYER_BULLET_SPEED = 5.0f;
const std::string PLAYER_BULLET_TEXTURE_FILE = "Data/Images/Bullet.png";
const int PLAYER_BULLET_STARTING_HEALTH = 1;


//-----------------------------------------------------------------------------------------------
class PlayerBullet : public Entity
{
public:
	// Constructors
	PlayerBullet( const Vector2& tankPosition, float tankOrientationDegrees, float tankCosmeticRadius );

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
	bool IsProjectile() const;

private:
};