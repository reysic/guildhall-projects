#pragma once

#include "Game/Generators/Generator.hpp"


//-----------------------------------------------------------------------------------------------
class CellularAutomataGenerator : public Generator
{
public:
	CellularAutomataGenerator( const std::string& name ) : Generator( name ) {};
	static Generator* CreateGenerator( const std::string& name ) { return new CellularAutomataGenerator( name ); };
	virtual Map* CreateInitializedMap( const IntVector2& size, const std::string& name );
																				// This should just bring back a map that's
																				// ready to run for this generator
																				// This should make a map that is 60% air,
																				// 40% stone
	virtual bool GenerateStep( Map* map, int currentStepNumber ); // For loop through the map, and for each cell look at its
															       // 8 neighbors, and implement the logic from
																   // http://www.bitstorm.org/gameoflife/
																   // There is one gotcha: When you're for looping through,
																   // don't immediately change the cells, change the cells
																   // after you gone through all of them. You can have your
																   // cells store their future type, then convert them to
																   // that type after the for loop

// 	static Generator* CreateGenerator( const std::string& name ) { return new CellularAutomataGenerator(); }
// 	static GeneratorRegistration s_cellularAutomataGeneratorRegistration;
// 
// 	static GeneratorRegistration s_bigCaverns;
// 	static GeneratorRegistration s_smallCaverns;
	static GeneratorRegistration s_CAGRegistration;
};