#pragma once

#include "Game/Entities/Entity.hpp"


//-----------------------------------------------------------------------------------------------
class Star : public Entity
{
public:
	// Constructors & Destructors
	Star( const WorldCoords& spawnPosition );
	~Star();

	// Methods
	void Update( float deltaSeconds );
	void Render() const;
};