#pragma once

#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Player : public Entity
{
public:
	// Constructors & Destructors
	Player( const WorldCoords& playerPosition );

	// Methods
	void Update( float deltaSeconds );
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	void Render() const;
	bool UsesCorrectivePhysics() const { return true; }

	// Accessors
	bool IsFocused() const { return m_isFocused; }

	// Mutators
	void SetIsFocused( bool newIsFocued ) { m_isFocused = newIsFocued; }

private:
	// Members
	bool m_isFocused;
	bool m_waitToSpawnNewPlayerBullet;
	float m_timeBetweenPlayerBullets;
	float m_timeLastPlayerBulletFired;
};