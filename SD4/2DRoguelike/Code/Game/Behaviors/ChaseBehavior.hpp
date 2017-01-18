#pragma once

#include <string>
#include <map>

#include "Game/Behaviors/Behavior.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
enum Direction;


//-----------------------------------------------------------------------------------------------
class ChaseBehavior : public Behavior
{
public:
	ChaseBehavior( const std::string& name, const XMLNode& blueprintNode );
	ChaseBehavior( const ChaseBehavior& copyFrom );

	virtual float CalcUtility();
	virtual void Run();
	virtual Behavior* GetClone();
	virtual bool DoesPassChanceToRun();

	static Behavior* CreateBehavior( const std::string& name, const XMLNode& blueprintNode ) { return new ChaseBehavior( name, blueprintNode ); };
	static BehaviorRegistration s_chaseBehaviorRegistration;

private:
};