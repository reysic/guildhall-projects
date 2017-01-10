#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Game/Core/Faction.hpp"
#include "Game/Entities/Agent.hpp"


//-----------------------------------------------------------------------------------------------
FactionMap* Faction::s_globalFactionMap = nullptr;
int Faction::s_nextID = 1;
const int Faction::INVALID_FACTION_ID = -1;


//-----------------------------------------------------------------------------------------------
Faction::Faction( const XMLNode& factionNode )
	: m_name( "" )
	, m_factionID( ++s_nextID )
{
	if ( factionNode.getAttribute( "name" ) )
	{
		std::string nameStr = factionNode.getAttribute( "name" );
		m_name = nameStr;

		PopulateFromXMLNode( factionNode );
	}
}


//-----------------------------------------------------------------------------------------------
Faction::Faction( const std::string& name )
	: m_name( name )
	, m_factionID( ++s_nextID )
{

}

Faction::Faction()
	:m_name( "INVALID" )
	, m_factionID( INVALID_FACTION_ID )
{

}


//-----------------------------------------------------------------------------------------------
void Faction::PopulateFromXMLNode( const XMLNode& factionNode )
{
	m_name = ReadXMLAttribute( factionNode, "name", m_name );

	std::string loveFactionStr = ReadXMLAttribute( factionNode, "loves", "" );
	std::string likeFactionStr = ReadXMLAttribute( factionNode, "likes", "" );
	std::string neutralFactionStr = ReadXMLAttribute( factionNode, "neutral", "" );
	std::string dislikeFactionStr = ReadXMLAttribute( factionNode, "dislikes", "" );
	std::string hateFactionStr = ReadXMLAttribute( factionNode, "hates", "" );

	std::vector< std::string > loveFactions = TokenizeStringOnDelimiter( loveFactionStr );
	std::vector< std::string > likeFactions = TokenizeStringOnDelimiter( likeFactionStr );
	std::vector< std::string > neutralFactions = TokenizeStringOnDelimiter( neutralFactionStr );
	std::vector< std::string > dislikeFactions = TokenizeStringOnDelimiter( dislikeFactionStr );
	std::vector< std::string > hateFactions = TokenizeStringOnDelimiter( hateFactionStr );

	for ( std::string str : loveFactions )
	{
		Faction* thisFaction = CreateOrGetFactionByName( str );
		int factionID = thisFaction->GetFactionID();
		FactionRelationship relationship;
		relationship.name = thisFaction->GetName();
		relationship.value = FACTION_STATUS_LOVES;
		m_factionRelationships.insert( FactionRelationshipPair( factionID, relationship ) );
	}

	for ( std::string str : likeFactions )
	{
		Faction* thisFaction = CreateOrGetFactionByName( str );
		int factionID = thisFaction->GetFactionID();
		FactionRelationship relationship;
		relationship.name = thisFaction->GetName();
		relationship.value = FACTION_STATUS_LIKES;
		m_factionRelationships.insert( FactionRelationshipPair( factionID, relationship ) );
	}

	for ( std::string str : neutralFactions )
	{
		Faction* thisFaction = CreateOrGetFactionByName( str );
		int factionID = thisFaction->GetFactionID();
		FactionRelationship relationship;
		relationship.name = thisFaction->GetName();
		relationship.value = FACTION_STATUS_NEUTRAL;
		m_factionRelationships.insert( FactionRelationshipPair( factionID, relationship ) );
	}

	for ( std::string str : dislikeFactions )
	{
		Faction* thisFaction = CreateOrGetFactionByName( str );
		int factionID = thisFaction->GetFactionID();
		FactionRelationship relationship;
		relationship.name = thisFaction->GetName();
		relationship.value = FACTION_STATUS_DISLIKES;
		m_factionRelationships.insert( FactionRelationshipPair( factionID, relationship ) );
	}

	for ( std::string str : hateFactions )
	{
		Faction* thisFaction = CreateOrGetFactionByName( str );
		int factionID = thisFaction->GetFactionID();
		FactionRelationship relationship;
		relationship.name = thisFaction->GetName();
		relationship.value = FACTION_STATUS_HATES;
		m_factionRelationships.insert( FactionRelationshipPair( factionID, relationship ) );
	}
}


//-----------------------------------------------------------------------------------------------
std::string Faction::GetFactionStatusAsString( FactionStatus status )
{
	switch ( status )
	{
	case FACTION_STATUS_HATES:
		return "Hates";
		break;
	case FACTION_STATUS_DISLIKES:
		return "Dislikes";
		break;
	case FACTION_STATUS_NEUTRAL:
		return "Neutral";
		break;
	case FACTION_STATUS_LIKES:
		return "Likes";
		break;
	case FACTION_STATUS_LOVES:
		return "Loves";
		break;
	default:
		return "";
		break;
	}
}


//-----------------------------------------------------------------------------------------------
void Faction::LoadAllFactions()
{
	if ( s_globalFactionMap == nullptr )
	{
		s_globalFactionMap = new FactionMap;
	}

	std::vector< std::string > factionFiles = EnumerateFilesInFolder( "Data/Factions",
		"*.Faction.xml" );

	for each ( std::string str in factionFiles )
	{
		XMLNode node = XMLNode::openFileHelper( str.c_str() );
		XMLNode rootNode = node.getChildNode( 0 );

		if ( !rootNode.IsContentEmpty() )
		{
			int factionCount = rootNode.nChildNode();

			for ( int nodeIndex = 0; nodeIndex < factionCount; ++nodeIndex )
			{
				XMLNode thisNode = rootNode.getChildNode( nodeIndex );
				std::string name = thisNode.getName();
				if ( name != "Faction" )
				{
					// Not a valid node
					continue;
				}
				if ( thisNode.getAttribute( "name" ) )
				{

					Faction* newFaction = CreateOrGetFactionByName( thisNode.getAttribute( "name" ) );
					if ( newFaction != nullptr && newFaction->m_name != "" )
					{
						newFaction->PopulateFromXMLNode( thisNode );
						s_globalFactionMap->insert( FactionPair( newFaction->m_name, newFaction ) );
					}
				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
Faction* Faction::CreateOrGetFactionByName( const std::string& name )
{

	FactionMapIter factionIter = Faction::s_globalFactionMap->find( name );

	if ( factionIter != Faction::s_globalFactionMap->end() )
	{
		return factionIter->second;
	}
	else
	{
		Faction* newFaction = new Faction( name );
		Faction::s_globalFactionMap->insert( FactionPair( newFaction->GetName(), newFaction ) );
		return newFaction;
	}
}


//-----------------------------------------------------------------------------------------------
void Faction::AdjustFactionStatus( Agent* instigator, FactionAction instigatorAction )
{
	int factionID = instigator->GetFactionID();
	int entityID = instigator->GetEntityID();

	FactionRelationshipIter agentIter = m_agentRelationships.find( entityID );

	if ( agentIter != m_agentRelationships.end() )
	{
		// Previously encountered agent
		int curValue = agentIter->second.value;

		curValue += instigatorAction;

		agentIter->second.value = curValue;
	}
	else
	{
		// Newly encountered agent
		FactionRelationshipIter factionRelationshipIter = m_factionRelationships.find( factionID );

		FactionRelationship newRelationship;
		newRelationship.name = instigator->m_name;

		if ( factionRelationshipIter != m_factionRelationships.end() )
		{
			// Faction found in map, pull from global value
			newRelationship.value = factionRelationshipIter->second.value;
		}
		else
		{
			// Faction not in map, default to neutral
			newRelationship.value = FACTION_STATUS_NEUTRAL;
		}

		// Adjust relationship value by faction action value
		newRelationship.value += instigatorAction;
		
		// Add to agent relationships map
		m_agentRelationships.insert( FactionRelationshipPair( entityID, newRelationship ) );
	}	
}


//-----------------------------------------------------------------------------------------------
FactionStatus Faction::GetFactionStatusForTargetAgent( Agent* targetAgent )
{
	int factionID = targetAgent->GetFactionID();
	int entityID = targetAgent->GetEntityID();

	FactionRelationshipIter agentRelationshipIter = m_agentRelationships.find( entityID );

	int curValue = 0;

	if ( agentRelationshipIter != m_agentRelationships.end() )
	{
		// Previously encountered agent
		curValue = agentRelationshipIter->second.value;
	}
	else
	{
		// Newly encountered agent
		FactionRelationshipIter factionRelationshipIter = m_factionRelationships.find( factionID );

		if ( factionRelationshipIter != m_factionRelationships.end() )
		{
			// Faction found in map, pull from global value
			curValue = factionRelationshipIter->second.value;
		}
		else
		{
			// Faction not in map, default to neutral
			curValue = FACTION_STATUS_NEUTRAL;
		}

	}
	return GetFactionStatusForIntValue( curValue );
}


//-----------------------------------------------------------------------------------------------
FactionStatus Faction::GetFactionStatusForIntValue( int value )
{
	if ( value <= FACTION_STATUS_HATES )
	{
		return FACTION_STATUS_HATES;
	}
	else if ( value <= FACTION_STATUS_DISLIKES )
	{
		return FACTION_STATUS_DISLIKES;
	}
	else if ( value < FACTION_STATUS_LIKES )
	{
		return FACTION_STATUS_NEUTRAL;
	}
	else if ( value < FACTION_STATUS_LOVES )
	{
		return FACTION_STATUS_LIKES;
	}
	else
	{
		return FACTION_STATUS_LOVES;
	}
}
