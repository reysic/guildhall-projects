#pragma once

#include <string>
#include <map>

#include "Game/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
enum Direction;


//-----------------------------------------------------------------------------------------------
class FleeBehavior : public Behavior
{
public:
	FleeBehavior( const std::string& name, const XMLNode& blueprintNode );
	FleeBehavior( const FleeBehavior& copyFrom );

	virtual float CalcUtility();
	virtual void Run();
	virtual Behavior* GetClone();
	virtual bool DoesPassChanceToRun();

	static Behavior* CreateBehavior( const std::string& name, const XMLNode& blueprintNode ) { return new FleeBehavior( name, blueprintNode ); };
	static BehaviorRegistration s_fleeBehaviorRegistration;

private:
};