#pragma once

#include <string>


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
//
// EnvironmentGenerationProcess
// Houses information from <EnvironmentGenerationProcess generator="CellularAutomata" steps="5"/
//
class EnvironmentGenerationProcess
{
public:
	EnvironmentGenerationProcess( const XMLNode& node );

	void PopulateFromXMLNode( const XMLNode& generationProcessNode );

public:
	int m_steps;
	std::string m_generatorName;
};