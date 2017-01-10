#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Behaviors/FollowBehavior.hpp"
#include "Game/NPCs/NPC.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Core/CombatSystem.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
BehaviorRegistration FollowBehavior::s_followBehaviorRegistration( "Follow", &FollowBehavior::CreateBehavior );


//-----------------------------------------------------------------------------------------------
FollowBehavior::FollowBehavior( const std::string& name, const XMLNode& blueprintNode )
	: Behavior( name, blueprintNode )
{
}


//-----------------------------------------------------------------------------------------------
FollowBehavior::FollowBehavior( const FollowBehavior& copyFrom )
	: Behavior( copyFrom )
{
}


//-----------------------------------------------------------------------------------------------
float FollowBehavior::CalcUtility()
{
	if ( m_owningAgent->GetTargetAlly() != nullptr )
	{
		return 7.0f;
	}
	return 0.0f;
}


//-----------------------------------------------------------------------------------------------
void FollowBehavior::Run()
{
	Map* map = m_owningAgent->m_map;
	IntVector2 selfPosition = m_owningAgent->m_position;
	IntVector2 targetPosition = m_owningAgent->GetTargetAlly()->m_position;
	m_owningAgent->m_pathfinder = new Pathfinder( selfPosition, targetPosition, map, m_owningAgent );
	m_owningAgent->m_pathfinder->TakeOneStep();
	m_owningAgent->m_pathfinder->TakeOneStep();
	IntVector2 pathStepPos = m_owningAgent->m_pathfinder->m_closedList.back()->m_position;
	Direction moveDir = map->GetDirectionToCellPositionFromCellPosition( pathStepPos, selfPosition );
	if ( m_owningAgent->TestAndMoveOneStep( moveDir ) && moveDir != DIRECTION_INVALID
		&& map->GetCellAtPosition( selfPosition ).m_isVisible )
	{
		g_theGame->PrintMessage( m_owningAgent->m_name + " follows " + m_owningAgent->GetTargetAlly()->m_name + "." );
	}
	delete m_owningAgent->m_pathfinder;
	m_owningAgent->m_pathfinder = nullptr;
}


//-----------------------------------------------------------------------------------------------
Behavior* FollowBehavior::GetClone()
{
	Behavior* clone = new FollowBehavior( *this );
	return clone;
}


//-----------------------------------------------------------------------------------------------
bool FollowBehavior::DoesPassChanceToRun()
{
	return GetRandomChance( 0.9f );
}