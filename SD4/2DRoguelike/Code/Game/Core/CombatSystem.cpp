#include "Game/Core/CombatSystem.hpp"
#include "Game/Entities/Agent.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
CombatSystem* g_theCombatSystem = nullptr;


//-----------------------------------------------------------------------------------------------
CombatSystem::CombatSystem()
{
}


//-----------------------------------------------------------------------------------------------
void CombatSystem::PerformAttack( AttackData& attackData )
{
	bool doesAttackHit = GetRandomChance( attackData.chanceToHit );

	// Adjust faction standings
	Agent* attacker = attackData.attacker;
	Agent* target = attackData.target;
	target->AdjustFactionStatus( attacker, FACTION_ACTION_ATTACKED_ME ); // #TODO: Adjust differently if attack did not hit

	if ( doesAttackHit )
	{
		// Attack hit
		int totalDamage = attackData.target->TakeDamage( attackData.damageDealt );

		// Print messages if attack involved the player
		if ( attackData.attacker->IsPlayer() )
		{
			if ( totalDamage == 0 )
			{
				g_theGame->PrintMessage( "Your attack bounces off " + attackData.target->m_name + "!", Rgba::RED );
			}
			else
			{
				g_theGame->PrintMessage( "You hit " + attackData.target->m_name + " for " +
					std::to_string( totalDamage ) + " damage.", Rgba::CYAN );
			}
		}
		else if ( attackData.target->IsPlayer() )
		{
			if ( totalDamage == 0 )
			{
				g_theGame->PrintMessage( attackData.attacker->m_name + "'s attack bounces off you!", Rgba::CYAN );
			}
			else
			{
				g_theGame->PrintMessage( attackData.attacker->m_name + " hits you for " +
					std::to_string( totalDamage ) + " damage.", Rgba::RED );
			}
		}
	}
	else
	{
		// Attack missed
		if ( attackData.attacker->IsPlayer() )
		{
			g_theGame->PrintMessage( "You missed " + attackData.target->m_name + "!", Rgba::RED );
		}
		else if ( attackData.target->IsPlayer() )
		{
			g_theGame->PrintMessage( attackData.attacker->m_name + " missed you!", Rgba::CYAN );
		}
	}
}