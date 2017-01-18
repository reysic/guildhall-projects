#pragma once

#include "Game/Entities/Agent.hpp"
#include "Game/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class NPC : public Agent
{
public:
	NPC( const XMLNode& npcBlueprintNode, GameEntityType type );
	NPC( const NPC& copyFrom );
	~NPC();

	virtual float Update();

protected:
	std::vector< Behavior* > m_behaviors;
};