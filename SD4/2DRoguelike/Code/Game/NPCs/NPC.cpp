#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Game/NPCs/NPC.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
NPC::NPC( const XMLNode& npcBlueprintNode, GameEntityType type )
	: Agent( type )
{
	if ( npcBlueprintNode.getAttribute( "name" ) )
	{
		std::string nameStr = npcBlueprintNode.getAttribute( "name" );
		m_name = nameStr;
	}

	if ( npcBlueprintNode.getAttribute( "maxHealth" ) )
	{
		std::string maxHealthStr = npcBlueprintNode.getAttribute( "maxHealth" );
		m_maxHealth = stoi( maxHealthStr );
	}

	if ( npcBlueprintNode.getAttribute( "damage" ) )
	{
		std::string damageStr = npcBlueprintNode.getAttribute( "damage" );
		m_damage = stoi( damageStr );
	}

	if ( npcBlueprintNode.getAttribute( "glyph" ) )
	{
		std::string glyphStr = npcBlueprintNode.getAttribute( "glyph" );
		m_glyph = glyphStr;
	}

	if ( npcBlueprintNode.getAttribute( "color" ) )
	{
		std::string colorStr = npcBlueprintNode.getAttribute( "color" );

		size_t pos = colorStr.find( "," );
		std::string redStr = colorStr.substr( 0, pos );
		colorStr.erase( 0, pos + 1 );

		pos = colorStr.find( "," );
		std::string greenStr = colorStr.substr( 0, pos );
		colorStr.erase( 0, pos + 1 );

		std::string blueStr = colorStr;

		m_color = Rgba( ( unsigned char ) stoi( redStr ), ( unsigned char ) stoi( greenStr ), 
			( unsigned char ) stoi( blueStr ), ( unsigned char ) 255 );
	}

	if ( npcBlueprintNode.getAttribute( "doesFly" ) )
	{
		bool doesFly = ReadXMLAttribute( npcBlueprintNode, "doesFly", false );

		if ( doesFly )
		{
			m_movementProperties = 0;
		}
		else
		{
			m_movementProperties = DEFAULT_MOVEMENT_PROPERTIES | MOVE_PROP_BLOCKED_BY_LAVA; // NPCs shouldn't go into lava by default
		}

	}

	if ( npcBlueprintNode.getAttribute( "turnDuration" ) )
	{
		std::string turnDurationStr = npcBlueprintNode.getAttribute( "turnDuration" );
		m_turnDuration = stof( turnDurationStr );
	}

	// Get faction
	if ( npcBlueprintNode.getAttribute( "faction" ) )
	{
		std::string factionStr = npcBlueprintNode.getAttribute( "faction" );
		m_faction = *Faction::CreateOrGetFactionByName( factionStr );
	}

	// Get behaviors
	XMLNode behaviorsNode = npcBlueprintNode.getChildNode( "Behaviors" );
	if ( !behaviorsNode.isEmpty() )
	{
		int numChildren = behaviorsNode.nChildNode();

		for ( int childIndex = 0; childIndex < numChildren; ++childIndex )
		{
			XMLNode thisBehaviorNode = behaviorsNode.getChildNode( childIndex );

			// Check map if behavior already exists
			std::string behaviorName = thisBehaviorNode.getName();

			Behavior* newBehavior = BehaviorRegistration::CreateBehaviorByName( behaviorName, thisBehaviorNode );

			if ( newBehavior != nullptr )
			{
				m_behaviors.push_back( newBehavior );
			}
		}
	}
	else
	{
		ASSERT_OR_DIE( false, "NPC " + m_name + " does not have behaviors!" );
	}

	XMLNode factionNode = npcBlueprintNode.getChildNode( "Faction" );
	if ( !factionNode.isEmpty() )
	{
		ASSERT_OR_DIE( npcBlueprintNode.getAttribute( "name" ), "Faction node of " + m_name + " doesn't have a name!" );
		std::string factionName = "";
		factionName = ReadXMLAttribute( factionNode, "name", factionName );
		m_faction = *Faction::CreateOrGetFactionByName( factionName );
		m_faction.PopulateFromXMLNode( factionNode );
	}

	ASSERT_OR_DIE( m_faction.GetFactionID() != -1, "Invalid faction ID for " + m_name + "!" );
}


//-----------------------------------------------------------------------------------------------
NPC::NPC( const NPC& copyFrom )
	: Agent( copyFrom )
{
	for ( Behavior* behaviorToCopy : copyFrom.m_behaviors )
	{
		Behavior* behavior = behaviorToCopy->GetClone();
		behavior->SetOwningAgent( this );
		m_behaviors.push_back( behavior );
	}
}


//-----------------------------------------------------------------------------------------------
NPC::~NPC()
{
	m_behaviors.clear();
}


//-----------------------------------------------------------------------------------------------
float NPC::Update()
{
	// Call Agent::Update() to populate visible agents vector
	Agent::Update();

	// Set target agent
	m_targetEnemy = nullptr;
	float distanceToNearestEnemy = 10000.0f;
	if ( m_visibleAgents.size() > 0 )
	{
		for ( Agent* otherAgent : m_visibleAgents )
		{
			if ( m_faction.GetFactionStatusForTargetAgent( otherAgent ) == FACTION_STATUS_HATES )
			{
				if ( m_map->GetDistanceBetweenCellPositions( m_position, otherAgent->m_position ) < distanceToNearestEnemy )
				{
					m_targetEnemy = otherAgent;
				}
			}
		}
	}

	// If there's nobody I hate nearby, how about someone I love?
	m_targetAlly = nullptr;
	float distanceToNearestAlly = 10000.0f;
	if ( m_visibleAgents.size() > 0 )
	{
		for ( Agent* otherAgent : m_visibleAgents )
		{
			if ( m_faction.GetFactionStatusForTargetAgent( otherAgent ) == FACTION_STATUS_LOVES )
			{
				if ( m_map->GetDistanceBetweenCellPositions( m_position, otherAgent->m_position ) < distanceToNearestAlly )
				{
					m_targetAlly = otherAgent;
				}
			}
		}
	}

	Behavior* behaviorToRun = nullptr;

	float currentBehaviorUtility = 0.0f;

	// Determine highest utility
	for ( Behavior* behavior : m_behaviors )
	{
		// Does behavior pass chance to run?
		// Used to make behavior less predictable
		if ( !behavior->DoesPassChanceToRun() )
		{
			continue;
		}
		
		// Calculate utility of this behavior
		float thisBehaviorUtility = behavior->CalcUtility();
		if ( thisBehaviorUtility > currentBehaviorUtility )
		{
			behaviorToRun = behavior;
			currentBehaviorUtility = thisBehaviorUtility;
		}
	}

	behaviorToRun->Run();

	return m_turnDuration;
}