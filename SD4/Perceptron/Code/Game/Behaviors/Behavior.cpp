#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Game/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
BehaviorRegistrationMap* BehaviorRegistration::s_behaviorRegistrationMap = nullptr;


//-----------------------------------------------------------------------------------------------
BehaviorRegistration::BehaviorRegistration( const std::string& name, BehaviorCreationFunc* creationFunc )
	: m_name( name )
	, m_creationFunc( creationFunc )
{
	if ( s_behaviorRegistrationMap == nullptr )
	{
		s_behaviorRegistrationMap = new BehaviorRegistrationMap;
	}

	if ( s_behaviorRegistrationMap->find( name ) == s_behaviorRegistrationMap->end() )
	{
		s_behaviorRegistrationMap->insert( std::pair< std::string, BehaviorRegistration* >( name, this ) );
	}
}


//-----------------------------------------------------------------------------------------------
Behavior* BehaviorRegistration::CreateBehaviorByName( const std::string& name, const XMLNode& behaviorNode )
{
	Behavior* behavior = nullptr;
	BehaviorRegistration* behaviorRegistration = nullptr;

	ASSERT_OR_DIE( s_behaviorRegistrationMap != nullptr, "Behavior map nullptr!" );

	BehaviorRegistrationMap::iterator behaviorRegistrationIter = s_behaviorRegistrationMap->find( name );

	if ( behaviorRegistrationIter != s_behaviorRegistrationMap->end() )
	{
		behaviorRegistration = behaviorRegistrationIter->second;
		behavior = ( *behaviorRegistration->m_creationFunc )( behaviorRegistration->m_name, behaviorNode );
	}
	else
	{
		DebuggerPrintf( "Did not find behavior of name %s\n", name );
	}

	return behavior;
}


//-----------------------------------------------------------------------------------------------
Behavior::Behavior( const std::string& name, const XMLNode& behaviorNode )
	: m_name( name )
	, m_chanceToRun( 1.0f )
{
	m_chanceToRun = ReadXMLAttribute( behaviorNode, "chanceToRun", m_chanceToRun );
}


//-----------------------------------------------------------------------------------------------
Behavior::Behavior( const Behavior& copyFrom )
	: m_name( copyFrom.m_name )
	, m_chanceToRun( copyFrom.m_chanceToRun )
{
}


//-----------------------------------------------------------------------------------------------
bool Behavior::DoesPassChanceToRun()
{
	return false;
}