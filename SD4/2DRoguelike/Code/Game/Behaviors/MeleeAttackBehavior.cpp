#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/Behaviors/MeleeAttackBehavior.hpp"
#include "Game/NPCs/NPC.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Core/CombatSystem.hpp"


//-----------------------------------------------------------------------------------------------
BehaviorRegistration MeleeAttackBehavior::s_meleeAttackBehaviorRegistration( "MeleeAttack", &MeleeAttackBehavior::CreateBehavior );


//-----------------------------------------------------------------------------------------------
MeleeAttackBehavior::MeleeAttackBehavior( const std::string& name, const XMLNode& blueprintNode )
	: Behavior( name, blueprintNode )
	, m_chanceToHit( 1.0f )
{
	m_chanceToHit = ReadXMLAttribute( blueprintNode, "chanceToHit", m_chanceToHit );
}


//-----------------------------------------------------------------------------------------------
MeleeAttackBehavior::MeleeAttackBehavior( const MeleeAttackBehavior& copyFrom )
	: Behavior( copyFrom )
	, m_chanceToHit( copyFrom.m_chanceToHit )
{
}


//-----------------------------------------------------------------------------------------------
float MeleeAttackBehavior::CalcUtility()
{
	if ( m_owningAgent->GetTargetEnemy() != nullptr )
	{
		Map* map = m_owningAgent->m_map;
		IntVector2 selfPosition = m_owningAgent->m_position;
		IntVector2 targetPosition = m_owningAgent->GetTargetEnemy()->m_position;
		if ( map->AreCellPositionsAdjacent( selfPosition, targetPosition ) )
		{
			return 10.0f;
		}
	}
	return 0.0f;
}


//-----------------------------------------------------------------------------------------------
void MeleeAttackBehavior::Run()
{
	AttackData attData;
	attData.attacker = m_owningAgent;
	attData.target = m_owningAgent->GetTargetEnemy();
	attData.chanceToHit = m_chanceToHit;
	attData.damageDealt = m_owningAgent->GetAgentAttackInt();
	g_theCombatSystem->PerformAttack( attData );
}


//-----------------------------------------------------------------------------------------------
Behavior* MeleeAttackBehavior::GetClone()
{
	Behavior* clone = new MeleeAttackBehavior( *this );
	return clone;
}


//-----------------------------------------------------------------------------------------------
bool MeleeAttackBehavior::DoesPassChanceToRun()
{
	return true;
}