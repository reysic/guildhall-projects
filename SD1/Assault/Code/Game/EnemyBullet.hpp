//-----------------------------------------------------------------------------------------------
// EnemyBullet.hpp
// Bullet fired by enemy tanks and turrets


#pragma once


//-----------------------------------------------------------------------------------------------
const float ENEMY_BULLET_SPEED = 5.0f;
const std::string ENEMY_BULLET_TEXTURE_FILE = "Data/Images/Bullet.png";
const int ENEMY_BULLET_STARTING_HEALTH = 1;


//-----------------------------------------------------------------------------------------------
class EnemyBullet : public Entity
{
public:
	// Constructors
	EnemyBullet( const Vector2& enemyPosition, float enemyOrientationDegrees, float enemyCosmeticRadius );

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
	bool IsProjectile() const;

private:
};