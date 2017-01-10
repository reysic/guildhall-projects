#pragma once

#include <string>
#include <map>


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class Agent;


//-----------------------------------------------------------------------------------------------
class Behavior
{
public:
	Behavior( const std::string& name, const XMLNode& behaviorNode );
	Behavior( const Behavior& copyFrom );
	virtual ~Behavior() {}

	virtual float CalcUtility() = 0;
	virtual void Run() = 0;
	virtual Behavior* GetClone() = 0;
	virtual bool DoesPassChanceToRun();

	const std::string GetName() const { return m_name; }
	void SetOwningAgent( Agent* agent ) { m_owningAgent = agent; }

protected:
	std::string m_name;
	Agent* m_owningAgent;
	float m_chanceToRun;
};


//-----------------------------------------------------------------------------------------------
class BehaviorRegistration;


//-----------------------------------------------------------------------------------------------
typedef Behavior* ( BehaviorCreationFunc ) ( const std::string& name, const XMLNode& behaviorNode );
typedef std::map< std::string, BehaviorRegistration* > BehaviorRegistrationMap;
typedef std::pair< std::string, BehaviorRegistration* > BehaviorRegistrationPair;
class BehaviorRegistration
{
public:
	BehaviorRegistration() {}
	BehaviorRegistration( const std::string& name, BehaviorCreationFunc* creationFunc );

	static Behavior* CreateBehaviorByName( const std::string& name, const XMLNode& behaviorNode );

public:
	static BehaviorRegistrationMap* s_behaviorRegistrationMap;

private:
	std::string m_name;
	BehaviorCreationFunc* m_creationFunc;
};