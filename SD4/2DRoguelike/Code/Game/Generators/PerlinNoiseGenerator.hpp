#pragma once

#include "Game/Generators/Generator.hpp"


//-----------------------------------------------------------------------------------------------
class PerlinNoiseGenerator : public Generator
{
public:
	PerlinNoiseGenerator( const std::string& name ) : Generator( name ) {};
	static Generator* CreateGenerator( const std::string& name ) { return new PerlinNoiseGenerator( name ); };
	virtual Map* CreateInitializedMap( const IntVector2& size, const std::string& name );
	virtual bool GenerateStep( Map* map, int currentStepNumber );

public:
	static GeneratorRegistration s_LakesRegistration;
	static GeneratorRegistration s_LavaRegistration;
	std::vector< float > m_perlinNoiseForMapCells;
	int m_randX = 0;
	int m_randY = 0;
};