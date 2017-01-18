#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Behaviors/PickUpItemBehavior.hpp"
#include "Game/NPCs/NPC.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Core/CombatSystem.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
BehaviorRegistration PickUpItemBehavior::s_pickUpItemBehaviorRegistration( "PickUpItem", &PickUpItemBehavior::CreateBehavior );


//-----------------------------------------------------------------------------------------------
PickUpItemBehavior::PickUpItemBehavior( const std::string& name, const XMLNode& blueprintNode )
	: Behavior( name, blueprintNode )
	, m_targetItem( nullptr )
{
}


//-----------------------------------------------------------------------------------------------
PickUpItemBehavior::PickUpItemBehavior( const PickUpItemBehavior& copyFrom )
	: Behavior( copyFrom )
	, m_targetItem( copyFrom.m_targetItem )
{
}


//-----------------------------------------------------------------------------------------------
float PickUpItemBehavior::CalcUtility()
{
	if ( m_owningAgent->GetVisibleItems().size() == 0 )
	{
		return 0.0f;
	}
	else
	{
		for ( Item* item : m_owningAgent->GetVisibleItems() )
		{
			if ( m_owningAgent->IsItemBetterThanEquipped( item ) )
			{
				m_targetItem = item;
				float distance = m_owningAgent->m_map->GetDistanceBetweenCellPositions( m_owningAgent->m_position, m_targetItem->m_position );
				distance += 5.f;
				return distance; // #TODO: Change to a value based on distance to item
			}
		}
	}
	return 0.0f;
}


//-----------------------------------------------------------------------------------------------
void PickUpItemBehavior::Run()
{
	if ( m_targetItem == nullptr )
	{
		return;
	}

	Map* map = m_owningAgent->m_map;
	IntVector2 selfPosition = m_owningAgent->m_position;
	IntVector2 targetPosition = m_targetItem->m_position;
	m_owningAgent->m_pathfinder = new Pathfinder( selfPosition, targetPosition, map, m_owningAgent );
	m_owningAgent->m_pathfinder->TakeOneStep();
	m_owningAgent->m_pathfinder->TakeOneStep();
	IntVector2 pathStepPos = m_owningAgent->m_pathfinder->m_closedList.back()->m_position;
	Direction moveDir = map->GetDirectionToCellPositionFromCellPosition( pathStepPos, selfPosition );


	if ( m_owningAgent->TestAndMoveOneStep( moveDir ) && moveDir != DIRECTION_INVALID )
	{
		if ( m_owningAgent->m_position == m_targetItem->m_position )
		{
			m_owningAgent->AddItemToInventory( m_targetItem );
			m_owningAgent->m_map->RemoveFirstItemAtCellPosition( m_owningAgent->m_position );
			g_theGame->RemoveGameEntity( m_targetItem );
			m_owningAgent->EquipBestItemOfType( m_targetItem->GetItemType() );

			g_theGame->PrintMessage( m_owningAgent->m_name + " picked up " + m_targetItem->m_name + "." );
			m_targetItem = nullptr;
		}
	}


	delete m_owningAgent->m_pathfinder;
	m_owningAgent->m_pathfinder = nullptr;
}


//-----------------------------------------------------------------------------------------------
Behavior* PickUpItemBehavior::GetClone()
{
	Behavior* clone = new PickUpItemBehavior( *this );
	return clone;
}


//-----------------------------------------------------------------------------------------------
bool PickUpItemBehavior::DoesPassChanceToRun()
{
	return true;
}