#pragma once

#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class PlayerBullet : public Entity
{
public:
	// Constructors & Destructors
	PlayerBullet( const WorldCoords& spawnPosition, float spawnOrientationDegrees, 
		float spawnCosmeticRadius );
	~PlayerBullet();

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
	bool IsProjectile() const { return true; }
};