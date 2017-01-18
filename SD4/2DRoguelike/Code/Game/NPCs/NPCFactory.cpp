#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Game/NPCs/NPCFactory.hpp"


//-----------------------------------------------------------------------------------------------
std::map< std::string, NPCFactory* >* NPCFactory::s_npcFactories = nullptr;


//-----------------------------------------------------------------------------------------------
NPCFactory::NPCFactory( const XMLNode& npcBlueprintNode )
	: m_name( "" )
{
	if ( npcBlueprintNode.getAttribute( "name" ) )
	{
		std::string nameStr = npcBlueprintNode.getAttribute( "name" );
		m_name = nameStr;

		PopulateFromXMLNode( npcBlueprintNode );
	}
}


//-----------------------------------------------------------------------------------------------
void NPCFactory::LoadNPCBlueprints()
{
	if ( s_npcFactories == nullptr )
	{
		s_npcFactories = new std::map< std::string, NPCFactory* >;
	}

	std::vector< std::string > npcFiles = EnumerateFilesInFolder( "Data/NPCs",
		"*.NPC.xml" );

	for each ( std::string str in npcFiles )
	{
		XMLNode node = XMLNode::openFileHelper( str.c_str() );
		XMLNode rootNode = node.getChildNode( 0 ); // NPCBlueprints

		if ( !rootNode.IsContentEmpty() )
		{
			int npcCount = rootNode.nChildNode(); // # of NPCBlueprint nodes

			for ( int i = 0; i < npcCount; i++ )
			{
				XMLNode thisNode = rootNode.getChildNode( i );
				std::string name = thisNode.getName();
				if ( name != "NPCBlueprint" )
				{
					// Not a valid node,skip
					continue;
				}
				NPCFactory* newFactory = new NPCFactory( thisNode );
				if ( newFactory != nullptr && newFactory->m_name != "" )
				{
					s_npcFactories->insert( std::pair< std::string, NPCFactory* >( newFactory->m_name, newFactory ) );
				}

			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
NPC* NPCFactory::CreateByName( const std::string& factoryName )
{
	NPC* npc = nullptr;
	NPCFactory* npcFactory = nullptr;

	ASSERT_OR_DIE( s_npcFactories != nullptr, "No NPC Factories." );

	std::map< std::string, NPCFactory* >::iterator npcFactoryIter = s_npcFactories->find( factoryName );

	if ( npcFactoryIter != s_npcFactories->end() )
	{
		npcFactory = npcFactoryIter->second;
		npc = npcFactory->Create();
		return npc;
	}
	else
	{
		DebuggerPrintf( "Did not find factory of name %s\n", factoryName );
	}
	return nullptr;
}


//-----------------------------------------------------------------------------------------------
void NPCFactory::PopulateFromXMLNode( const XMLNode& npcBlueprintNode )
{
	m_templateNPC = new NPC( npcBlueprintNode, GAME_ENTITY_NPC );
}