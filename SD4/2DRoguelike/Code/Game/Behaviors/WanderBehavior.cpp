#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Behaviors/WanderBehavior.hpp"
#include "Game/NPCs/NPC.hpp"
#include "Game/Maps/Map.hpp"


//-----------------------------------------------------------------------------------------------
BehaviorRegistration WanderBehavior::s_wanderBehaviorRegistration( "Wander", &WanderBehavior::CreateBehavior );


//-----------------------------------------------------------------------------------------------
WanderBehavior::WanderBehavior( const std::string& name, const XMLNode& blueprintNode )
	: Behavior( name, blueprintNode )
	, m_chanceToRest( 0.5f )
	, m_chanceToGoStraight( 0.3f )
	, m_lastDirection( DIRECTION_INVALID )

{
	m_chanceToRest = ReadXMLAttribute( blueprintNode, "chanceToRest", m_chanceToRest );
	m_chanceToGoStraight = ReadXMLAttribute( blueprintNode, "chanceToGoStraight", m_chanceToGoStraight );
}


//-----------------------------------------------------------------------------------------------
WanderBehavior::WanderBehavior( const WanderBehavior& copyFrom )
	: Behavior( copyFrom )
	, m_chanceToRest( copyFrom.m_chanceToRest )
	, m_chanceToGoStraight( copyFrom.m_chanceToGoStraight )
	, m_lastDirection( copyFrom.m_lastDirection )
{
}


//-----------------------------------------------------------------------------------------------
float WanderBehavior::CalcUtility()
{
	return 1.0f;
}


//-----------------------------------------------------------------------------------------------
void WanderBehavior::Run()
{
	bool isResting = GetRandomChance( m_chanceToGoStraight );

	if ( isResting )
	{
		m_lastDirection = DIRECTION_INVALID;
		return;
	}

	if ( m_lastDirection != DIRECTION_INVALID )
	{
		bool isGoingStraight = GetRandomChance( m_chanceToGoStraight );
		if ( isGoingStraight )
		{

			bool didMove = m_owningAgent->TestAndMoveOneStep( m_lastDirection );
			if ( didMove )
			{
				return;
			}

		}
	}

	Direction newDirection = DIRECTION_INVALID;

	while ( newDirection == m_lastDirection )
	{
		newDirection = Map::GetRandomDirection();
	}

	// If stuck, stand still
	bool didMove = m_owningAgent->TestAndMoveOneStep( newDirection );
	if ( didMove )
	{
		m_lastDirection = newDirection;
	}
	else
	{
		m_lastDirection = DIRECTION_INVALID;
	}
}


//-----------------------------------------------------------------------------------------------
Behavior* WanderBehavior::GetClone()
{
	Behavior* clone = new WanderBehavior( *this );
	return clone;
}


//-----------------------------------------------------------------------------------------------
bool WanderBehavior::DoesPassChanceToRun()
{
	return true;
}