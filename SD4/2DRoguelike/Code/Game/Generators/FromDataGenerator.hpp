#pragma once

#include "Game/Generators/Generator.hpp"


//-----------------------------------------------------------------------------------------------
class FromDataGenerator : public Generator
{
public:
	FromDataGenerator( const std::string& name ) : Generator( name ) {};
	static Generator* CreateGenerator( const std::string& name ) { return new FromDataGenerator( name ); };
	virtual Map* CreateInitializedMap( const IntVector2& size, const std::string& name );
	virtual bool GenerateStep( Map* map, int currentStepNumber );

public:
	static GeneratorRegistration s_FDGRegistration;
};