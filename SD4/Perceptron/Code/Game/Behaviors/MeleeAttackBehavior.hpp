#pragma once

#include <string>
#include <map>

#include "Game/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
enum Direction;


//-----------------------------------------------------------------------------------------------
class MeleeAttackBehavior : public Behavior
{
public:
	MeleeAttackBehavior( const std::string& name, const XMLNode& blueprintNode );
	MeleeAttackBehavior( const MeleeAttackBehavior& copyFrom );

	virtual float CalcUtility();
	virtual void Run();
	virtual Behavior* GetClone();
	virtual bool DoesPassChanceToRun();

	static Behavior* CreateBehavior( const std::string& name, const XMLNode& blueprintNode ) { return new MeleeAttackBehavior( name, blueprintNode ); };
	static BehaviorRegistration s_meleeAttackBehaviorRegistration;

private:
	float m_chanceToHit;
};