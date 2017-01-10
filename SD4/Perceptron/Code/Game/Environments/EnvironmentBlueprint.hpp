#pragma once

#include <map>
#include <string>
#include <vector>

#include "Engine/Math/IntVector2.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Generators/Generator.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;
class EnvironmentGenerationProcess;


//-----------------------------------------------------------------------------------------------
class EnvironmentBlueprint
{
public:
	EnvironmentBlueprint( const std::string& path );

	static void LoadEnvironmentBlueprints(); // Now your game can just call EnvironmentBlueprint::
											 // LoadEnvironmentBlueprints to load all the blueprints.
											 // This way you don't have a lot of code in your game
											 // that handles loading blueprints.
											 // This function should use an engine convenience
											 // function (OpenXMLDocument()).

	static std::map< std::string, EnvironmentBlueprint* >* s_environmentBlueprintRegistrationMap;
	
	std::string GetEnvironmentName() { return m_name; }

	Map* CreateInitializedMap();
	bool GenerateStep( Map* map, int currentStepNumber );
	void ClearBlueprint();

	unsigned int m_currentProcessIndex;
	unsigned int m_currentProcessStepsElapsed;
	Generator* m_currentGenerator;

protected:
	std::string m_name;
	IntVector2 m_size;
	std::vector< EnvironmentGenerationProcess* > m_environmentGenerationProcesses;
};