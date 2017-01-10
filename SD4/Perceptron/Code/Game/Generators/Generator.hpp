#pragma once

#include <string>
#include <map>

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Tools/Parsers/xmlParser.h"
#include "Game/Maps/Map.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"
#include "Game/NPCs/NPC.hpp"


//-----------------------------------------------------------------------------------------------
class Generator
{
public:
	Generator( const std::string& name ) : m_name( name ) {};
	virtual Map* CreateInitializedMap( const IntVector2& size, const std::string& name );
																				// This should just bring back a map that's
																				// ready to run for this generator
																				// This should make a map that is 100% stone
	virtual bool GenerateStep( Map* map, int currentStepNumber ) = 0;

	static EnvironmentGenerationProcess* CreateEnvironmentGenerationProcess( const XMLNode& node ) { return new EnvironmentGenerationProcess( node ); };

	static void FinalizeMap( Map* map );
	static IntVector2 FindValidStartingLocation( Map* map );

private:
	static void PopulateMap( Map* map );
	static void SpawnNPCs( Map* map );
	static void SpawnItems( Map* map );
	static bool SetNPCStartPosition( NPC* npc );
	static bool SetItemStartPosition( Item* item );

protected:
	std::string m_name;
};


//-----------------------------------------------------------------------------------------------
typedef Generator* ( GeneratorCreationFunc )( const std::string& name );
typedef EnvironmentGenerationProcess* ( EnvironmentGenerationProcessCreationFunc ) ( const XMLNode& node );
class GeneratorRegistration
{
public:
	std::map< std::string, GeneratorRegistration* >* GetGeneratorRegistrations() { return s_generatorRegistrationMap; };
	GeneratorRegistration( const std::string& name, GeneratorCreationFunc* creationFunc, EnvironmentGenerationProcessCreationFunc*  environmentGenerationProcessCreationFunc );
	static Generator* CreateGeneratorByName( const std::string& name );
	static EnvironmentGenerationProcess* CreateEnvironmentGenerationProcessByName( const std::string& name, const XMLNode& node );

protected:
	static std::map< std::string, GeneratorRegistration* >* s_generatorRegistrationMap;

private:
	std::string m_name;
	GeneratorCreationFunc* m_creationFunc;
	EnvironmentGenerationProcessCreationFunc* m_environmentGenerationProcessCreationFunc;
};