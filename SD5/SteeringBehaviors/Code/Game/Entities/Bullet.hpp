#pragma once

#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Bullet : public Entity
{
public:
	// Constructors & Destructors
	Bullet( const WorldCoords& spawnPosition, float spawnOrientationDegrees,
		float spawnCosmeticRadius, BulletType bulletType );
	~Bullet();

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
	void RenderDebug() const;
	bool IsProjectile() const { return true; }

private:
	BulletType m_bulletType;
};