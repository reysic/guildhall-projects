#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/Environments/EnvironmentBlueprint.hpp"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
std::map< std::string, EnvironmentBlueprint* >* EnvironmentBlueprint::s_environmentBlueprintRegistrationMap = nullptr;


//-----------------------------------------------------------------------------------------------
EnvironmentBlueprint::EnvironmentBlueprint( const std::string& path )
	: m_currentProcessIndex( 0 )
	, m_currentProcessStepsElapsed( 0 )
	, m_currentGenerator( nullptr )
{
	std::string environmentName = ExtractTokenFromString( path, "Data/Environments/", "." );
	m_name = environmentName;

	XMLNode node = XMLNode::openFileHelper( path.c_str() );
	XMLNode rootNode = node.getChildNode( 0 );

	if ( !rootNode.IsContentEmpty() )
	{
		if ( rootNode.getAttribute( "size" ) )
		{
			std::string sizeStr = rootNode.getAttribute( "size" );
			std::string sizeXStr = ExtractTokenFromString( sizeStr, "", "," );
			std::string sizeYStr = ExtractTokenFromString( sizeStr, ( sizeXStr + "," ), "" );
			m_size = IntVector2( stoi( sizeXStr ), stoi( sizeYStr ) );
		}
		else
		{
			m_size = IntVector2( 4, 4 );
		}

		int processCount = rootNode.nChildNode();

		for ( int i = 0; i < processCount; i++ )
		{
			XMLNode thisNode = rootNode.getChildNode( i );

			EnvironmentGenerationProcess* newProcess = new EnvironmentGenerationProcess( thisNode );

			m_environmentGenerationProcesses.push_back( newProcess );
		}
	}
	else
	{ 
		ASSERT_OR_DIE( false, "XML file empty!" );
	}
}


//-----------------------------------------------------------------------------------------------
void EnvironmentBlueprint::LoadEnvironmentBlueprints()
{
	if ( s_environmentBlueprintRegistrationMap == nullptr )
	{
		s_environmentBlueprintRegistrationMap = new std::map<std::string, EnvironmentBlueprint*>;
	}

	std::vector< std::string > environmentFiles = EnumerateFilesInFolder( "Data/Environments",
		"*.Environments.xml" );
	
	for each ( std::string str in environmentFiles )
	{
		std::string environmentName = ExtractTokenFromString( str, "Data/Environments/", "." );

		EnvironmentBlueprint* newEnvironment = new EnvironmentBlueprint( str );

		s_environmentBlueprintRegistrationMap->insert( std::pair< std::string, EnvironmentBlueprint* >( environmentName, newEnvironment ) );
	}
}


//-----------------------------------------------------------------------------------------------
Map* EnvironmentBlueprint::CreateInitializedMap()
{
	ASSERT_OR_DIE( m_environmentGenerationProcesses.size() != 0, "No generation processes registered!" );

	unsigned int processNumber = 0;
	while ( m_currentGenerator == nullptr && processNumber < m_environmentGenerationProcesses.size() )
	{
		std::string generatorName = m_environmentGenerationProcesses[ processNumber ]->m_generatorName;
		m_currentGenerator = GeneratorRegistration::CreateGeneratorByName( generatorName );
		processNumber++;
	}

	if ( m_currentGenerator != nullptr )
	{
		return m_currentGenerator->CreateInitializedMap( m_size, m_name );
	}
	else
	{
		return nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
bool EnvironmentBlueprint::GenerateStep( Map* map, int currentStepNumber )
{
	UNUSED( currentStepNumber );

	// Ensure blueprint generator has been created
	ASSERT_OR_DIE( m_currentGenerator != nullptr, "Must initialize map before calling GenerateStep()!" );
	if ( m_currentProcessIndex >= m_environmentGenerationProcesses.size() )
	{
		return false;
	}

	// Store total number of steps in current process
	unsigned int processSteps = m_environmentGenerationProcesses[ m_currentProcessIndex ]->m_steps;

	if ( m_currentProcessStepsElapsed == processSteps )
	{
		// Time to move on to the next process
		m_currentProcessIndex++;

		// Does another process exist?
		if ( m_currentProcessIndex < m_environmentGenerationProcesses.size() )
		{
			// There is another generation process

			// Set m_currentGenerator based upon next generation process
			std::string generatorName = m_environmentGenerationProcesses[ m_currentProcessIndex ]->m_generatorName;
			m_currentGenerator = GeneratorRegistration::CreateGeneratorByName( generatorName );

			// Reset elapsed steps
			m_currentProcessStepsElapsed = 0;

			// Call generate step on 'new' current generator
			if ( !m_currentGenerator->GenerateStep( map, m_currentProcessStepsElapsed ) )
			{
				m_currentProcessIndex++;
			}
			return true;
		}
		else
		{
			// There is not another generation process
			DebuggerPrintf( "Invalid process call in EnvironmentBlueprint.cpp!" );
			return false;
		}
	}
	else
	{
		// Haven't reached the end of the current process
		m_currentProcessStepsElapsed++;
		m_currentGenerator->GenerateStep( map, m_currentProcessStepsElapsed );
		return true;
	}
}


//-----------------------------------------------------------------------------------------------
void EnvironmentBlueprint::ClearBlueprint()
{
	m_currentProcessIndex = 0;
	m_currentProcessStepsElapsed = 0;
	delete m_currentGenerator;
	m_currentGenerator = nullptr;
}
