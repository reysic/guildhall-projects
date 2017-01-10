#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Behaviors/RangedAttackBehavior.hpp"
#include "Game/NPCs/NPC.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Core/CombatSystem.hpp"


//-----------------------------------------------------------------------------------------------
BehaviorRegistration RangedAttackBehavior::s_rangedAttackBehaviorRegistration( "RangedAttack", &RangedAttackBehavior::CreateBehavior );


//-----------------------------------------------------------------------------------------------
RangedAttackBehavior::RangedAttackBehavior( const std::string& name, const XMLNode& blueprintNode )
	: Behavior( name, blueprintNode )
	, m_chanceToHit( 1.0f )
{
	m_chanceToHit = ReadXMLAttribute( blueprintNode, "chanceToHit", m_chanceToHit );
}


//-----------------------------------------------------------------------------------------------
RangedAttackBehavior::RangedAttackBehavior( const RangedAttackBehavior& copyFrom )
	: Behavior( copyFrom )
	, m_chanceToHit( copyFrom.m_chanceToHit )
{
}


//-----------------------------------------------------------------------------------------------
float RangedAttackBehavior::CalcUtility()
{
	if ( m_owningAgent->GetTargetEnemy() != nullptr )
	{
		Map* map = m_owningAgent->m_map;
		IntVector2 selfPosition = m_owningAgent->m_position;
		IntVector2 targetPosition = m_owningAgent->GetTargetEnemy()->m_position;
		if ( map->GetDistanceBetweenCellPositions( selfPosition, targetPosition ) < 6.0f )
		{
			return 15.0f;
		}
		else
		{
			return 0.0f;
		}
	}
	return 0.0f;
}


//-----------------------------------------------------------------------------------------------
void RangedAttackBehavior::Run()
{
	AttackData attData;
	attData.attacker = m_owningAgent;
	attData.target = m_owningAgent->GetTargetEnemy();
	attData.chanceToHit = m_chanceToHit;
	attData.damageDealt = m_owningAgent->GetAgentAttackInt();
	g_theCombatSystem->PerformAttack( attData );
}


//-----------------------------------------------------------------------------------------------
Behavior* RangedAttackBehavior::GetClone()
{
	Behavior* clone = new RangedAttackBehavior( *this );
	return clone;
}


//-----------------------------------------------------------------------------------------------
bool RangedAttackBehavior::DoesPassChanceToRun()
{
	return true;
}