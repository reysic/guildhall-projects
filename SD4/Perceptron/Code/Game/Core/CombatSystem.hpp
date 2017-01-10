#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
class CombatSystem;
class Agent;


//-----------------------------------------------------------------------------------------------
extern CombatSystem* g_theCombatSystem;


//-----------------------------------------------------------------------------------------------
// This struct should be a complete representation of an attack
// When it's time to perform an attack, the AttackData is made
struct AttackData
{
	int damage; // ideally support a range, like 2-5
	Agent* attacker;
	Agent* target; // GameEntity* if you want to attack doors
	float chanceToHit;
	bool didAttackHit;
	int damageDealt;
	// ResultEnum result;
};


//-----------------------------------------------------------------------------------------------
// Updates faction standings and decrements health following attack
class CombatSystem
{
public:
	CombatSystem();

	void PerformAttack( AttackData& attackData );
};