#pragma once

#include <string>
#include <map>

#include "Game/Behaviors/Behavior.hpp"
#include "Game/Entities/Item.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
enum Direction;


//-----------------------------------------------------------------------------------------------
class PickUpItemBehavior : public Behavior
{
public:
	PickUpItemBehavior( const std::string& name, const XMLNode& blueprintNode );
	PickUpItemBehavior( const PickUpItemBehavior& copyFrom );

	virtual float CalcUtility();
	virtual void Run();
	virtual Behavior* GetClone();
	virtual bool DoesPassChanceToRun();

	static Behavior* CreateBehavior( const std::string& name, const XMLNode& blueprintNode ) { return new PickUpItemBehavior( name, blueprintNode ); };
	static BehaviorRegistration s_pickUpItemBehaviorRegistration;

private:
	Item* m_targetItem;
};