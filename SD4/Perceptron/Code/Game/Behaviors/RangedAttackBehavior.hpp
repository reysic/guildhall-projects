#pragma once

#include <string>
#include <map>

#include "Game/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
enum Direction;


//-----------------------------------------------------------------------------------------------
class RangedAttackBehavior : public Behavior
{
public:
	RangedAttackBehavior( const std::string& name, const XMLNode& blueprintNode );
	RangedAttackBehavior( const RangedAttackBehavior& copyFrom );

	virtual float CalcUtility();
	virtual void Run();
	virtual Behavior* GetClone();
	virtual bool DoesPassChanceToRun();

	static Behavior* CreateBehavior( const std::string& name, const XMLNode& blueprintNode ) { return new RangedAttackBehavior( name, blueprintNode ); };
	static BehaviorRegistration s_rangedAttackBehaviorRegistration;

private:
	float m_chanceToHit;
};