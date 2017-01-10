#pragma once

#include <string>
#include <map>

#include "Game/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
enum Direction;


//-----------------------------------------------------------------------------------------------
class WanderBehavior : public Behavior
{
public:
	WanderBehavior( const std::string& name, const XMLNode& blueprintNode );
	WanderBehavior( const WanderBehavior& copyFrom );

	virtual float CalcUtility();
	virtual void Run();
	virtual Behavior* GetClone();
	virtual bool DoesPassChanceToRun();

	static Behavior* CreateBehavior( const std::string& name, const XMLNode& blueprintNode ) { return new WanderBehavior( name, blueprintNode ); };
	static BehaviorRegistration s_wanderBehaviorRegistration;

private:
	float m_chanceToRest;
	float m_chanceToGoStraight;
	Direction m_lastDirection;
};