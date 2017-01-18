#pragma once

#include <map>
#include <string>

#include "Game/NPCS/NPC.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class NPCFactory
{
public:
	NPCFactory( const XMLNode& npcBlueprintNode );

	NPC* Create() { return new NPC( *m_templateNPC ); };

	static void LoadNPCBlueprints();
	static NPC* CreateByName( const std::string& factoryName );

public:
	static std::map< std::string, NPCFactory* >* s_npcFactories;
	std::string m_name;

private:
	void PopulateFromXMLNode( const XMLNode& npcBlueprintNode );

private:
	NPC* m_templateNPC;
};