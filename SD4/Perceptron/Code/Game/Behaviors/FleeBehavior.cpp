#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Behaviors/FleeBehavior.hpp"
#include "Game/NPCs/NPC.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Core/CombatSystem.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
BehaviorRegistration FleeBehavior::s_fleeBehaviorRegistration( "Flee", &FleeBehavior::CreateBehavior );


//-----------------------------------------------------------------------------------------------
FleeBehavior::FleeBehavior( const std::string& name, const XMLNode& blueprintNode )
	: Behavior( name, blueprintNode )
{
}


//-----------------------------------------------------------------------------------------------
FleeBehavior::FleeBehavior( const FleeBehavior& copyFrom )
	: Behavior( copyFrom )
{
}


//-----------------------------------------------------------------------------------------------
float FleeBehavior::CalcUtility()
{
	if ( m_owningAgent->GetTargetEnemy() != nullptr )
	{
		return 5.0f;
	}
	return 0.0f;
}


//-----------------------------------------------------------------------------------------------
void FleeBehavior::Run()
{
	Map* map = m_owningAgent->m_map;
	IntVector2 selfPosition = m_owningAgent->m_position;
	IntVector2 targetPosition = m_owningAgent->GetTargetEnemy()->m_position;
	m_owningAgent->m_pathfinder = new Pathfinder( selfPosition, targetPosition, map, m_owningAgent );
	m_owningAgent->m_pathfinder->TakeOneStep();
	m_owningAgent->m_pathfinder->TakeOneStep();
	IntVector2 pathStepPos = m_owningAgent->m_pathfinder->m_closedList.back()->m_position;
	Direction towardsDir = map->GetDirectionToCellPositionFromCellPosition( pathStepPos, selfPosition );
	Direction moveDir = map->GetOppositeDirection( towardsDir );
	if ( m_owningAgent->TestAndMoveOneStep( moveDir ) && moveDir != DIRECTION_INVALID && m_owningAgent->GetTargetEnemy()->IsPlayer() )
	{
		g_theGame->PrintMessage( m_owningAgent->m_name + " flees from you." );
	}
	delete m_owningAgent->m_pathfinder;
	m_owningAgent->m_pathfinder = nullptr;
}


//-----------------------------------------------------------------------------------------------
Behavior* FleeBehavior::GetClone()
{
	Behavior* clone = new FleeBehavior( *this );
	return clone;
}


//-----------------------------------------------------------------------------------------------
bool FleeBehavior::DoesPassChanceToRun()
{
	return GetRandomChance( 0.75f );
}