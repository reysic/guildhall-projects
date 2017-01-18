#pragma once

#include <string>
#include <map>

#include "Game/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
enum Direction;


//-----------------------------------------------------------------------------------------------
class FollowBehavior : public Behavior
{
public:
	FollowBehavior( const std::string& name, const XMLNode& blueprintNode );
	FollowBehavior( const FollowBehavior& copyFrom );

	virtual float CalcUtility();
	virtual void Run();
	virtual Behavior* GetClone();
	virtual bool DoesPassChanceToRun();

	static Behavior* CreateBehavior( const std::string& name, const XMLNode& blueprintNode ) { return new FollowBehavior( name, blueprintNode ); };
	static BehaviorRegistration s_followBehaviorRegistration;

private:
};