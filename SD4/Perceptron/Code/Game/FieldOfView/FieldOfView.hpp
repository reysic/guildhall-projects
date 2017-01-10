#pragma once

#include "Game/Maps/Map.hpp"
#include "Game/Entities/Agent.hpp"


//-----------------------------------------------------------------------------------------------
class FieldOfView
{
	static void CalculateFieldOfViewForAgent( Agent* agent, float viewDistance, Map* map );
};