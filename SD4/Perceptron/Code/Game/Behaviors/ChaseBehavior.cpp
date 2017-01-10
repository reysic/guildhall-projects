#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Behaviors/ChaseBehavior.hpp"
#include "Game/NPCs/NPC.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Core/CombatSystem.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
BehaviorRegistration ChaseBehavior::s_chaseBehaviorRegistration( "Chase", &ChaseBehavior::CreateBehavior );


//-----------------------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior( const std::string& name, const XMLNode& blueprintNode )
	: Behavior( name, blueprintNode )
{
}


//-----------------------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior( const ChaseBehavior& copyFrom )
	: Behavior( copyFrom )
{
}


//-----------------------------------------------------------------------------------------------
float ChaseBehavior::CalcUtility()
{
	if ( m_owningAgent->GetTargetEnemy() != nullptr )
	{
		return 5.0f;
	}
	return 0.0f;
}


//-----------------------------------------------------------------------------------------------
void ChaseBehavior::Run()
{
	Map* map = m_owningAgent->m_map;
	IntVector2 selfPosition = m_owningAgent->m_position;
	IntVector2 targetPosition = m_owningAgent->GetTargetEnemy()->m_position;
	m_owningAgent->m_pathfinder = new Pathfinder( selfPosition, targetPosition, map, m_owningAgent );
	m_owningAgent->m_pathfinder->TakeOneStep();
	m_owningAgent->m_pathfinder->TakeOneStep();
	IntVector2 pathStepPos = m_owningAgent->m_pathfinder->m_closedList.back()->m_position;
	Direction moveDir = map->GetDirectionToCellPositionFromCellPosition( pathStepPos, selfPosition );
	if ( m_owningAgent->TestAndMoveOneStep( moveDir ) && moveDir != DIRECTION_INVALID 
		&& map->GetCellAtPosition( selfPosition ).m_isVisible )
	{
		g_theGame->PrintMessage( m_owningAgent->m_name + " chases " + m_owningAgent->GetTargetEnemy()->m_name + "." );
	}
	delete m_owningAgent->m_pathfinder;
	m_owningAgent->m_pathfinder = nullptr;
}


//-----------------------------------------------------------------------------------------------
Behavior* ChaseBehavior::GetClone()
{
	Behavior* clone = new ChaseBehavior( *this );
	return clone;
}


//-----------------------------------------------------------------------------------------------
bool ChaseBehavior::DoesPassChanceToRun()
{
	return GetRandomChance( 0.75f );
}