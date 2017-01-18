#include "Engine/Tools/Parsers/xmlParser.h"
#include "Game/Environments/EnvironmentGenerationProcess.hpp"


//-----------------------------------------------------------------------------------------------
EnvironmentGenerationProcess::EnvironmentGenerationProcess( const XMLNode& generationProcessNode )
	: m_steps( 1 )
	, m_generatorName( "" )
{
	PopulateFromXMLNode( generationProcessNode );
}


//-----------------------------------------------------------------------------------------------
void EnvironmentGenerationProcess::PopulateFromXMLNode( const XMLNode& generationProcessNode )
{
	if ( !generationProcessNode.IsContentEmpty() )
	{

		std::string genStr = generationProcessNode.getAttribute( "generator" );
		std::string stepsStr = generationProcessNode.getAttribute( "steps" );

		m_generatorName = genStr;
		m_steps = stoi( stepsStr );

	}
}
