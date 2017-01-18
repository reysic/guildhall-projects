#pragma once

#include <string>
#include <vector>
#include <map>

//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Agent;
class Faction;


//-----------------------------------------------------------------------------------------------
enum FactionStatus
{
	FACTION_STATUS_HATES = -20,
	FACTION_STATUS_DISLIKES = -10,
	FACTION_STATUS_NEUTRAL = 0,
	FACTION_STATUS_LIKES = 10,
	FACTION_STATUS_LOVES = 20,
};


//-----------------------------------------------------------------------------------------------
enum FactionAction
{
	FACTION_ACTION_KILLED_ALLY = -7,
	FACTION_ACTION_ATTACKED_ME = -5,
	FACTION_ACTION_ATTACKED_ALLY = -3,
	FACTION_ACTION_TRIED_TO_ATTACK_ME = -2,
	FACTION_ACTION_TRIED_TO_ATTACK_ALLY = -1,
	FACTION_ACTION_HEALED_ALLY = 3,
	FACTION_ACTION_HEALED_ME = 5,
};



//-----------------------------------------------------------------------------------------------
struct FactionRelationship
{
	int value;
	std::string name;
};


//-----------------------------------------------------------------------------------------------
typedef std::map<int, FactionRelationship > FactionRelationshipMap;
typedef std::pair<int, FactionRelationship > FactionRelationshipPair;
typedef std::map<std::string, Faction*> FactionMap;
typedef std::pair<std::string, Faction*> FactionPair;
typedef FactionMap::iterator FactionMapIter;
typedef FactionRelationshipMap::iterator FactionRelationshipIter;
class Faction
{
public:
	Faction();

	void PopulateFromXMLNode( const XMLNode& factionNode );

	const std::string& GetName() const { return m_name; }
	int GetFactionID() const { return m_factionID; }
	static std::string GetFactionStatusAsString( FactionStatus status );
	static void LoadAllFactions();
	static Faction* CreateOrGetFactionByName( const std::string& name );
	void AdjustFactionStatus( Agent* instigator, FactionAction instigatorAction );
	FactionStatus GetFactionStatusForTargetAgent( Agent* targetAgent);
	FactionStatus GetFactionStatusForIntValue( int value );

	
	static FactionMap* s_globalFactionMap;
	static int s_nextID;
	static const int INVALID_FACTION_ID;

private:
	Faction( const std::string& name );
	Faction( const XMLNode& factionNode );

	std::string m_name;
	int m_factionID;
	FactionRelationshipMap m_factionRelationships; // FactionID
	FactionRelationshipMap m_agentRelationships; // AgentID
};