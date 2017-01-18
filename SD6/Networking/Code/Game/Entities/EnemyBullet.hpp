#pragma once

#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class EnemyBullet : public Entity
{
public:
	// Constructors & Destructors
	EnemyBullet( const WorldCoords& spawnPosition, float spawnOrientationDegrees,
		float spawnCosmeticRadius );
	~EnemyBullet();

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
	bool IsProjectile() const { return true; }
};