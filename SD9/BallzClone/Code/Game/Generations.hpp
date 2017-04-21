#pragma once

#include <vector>


//-----------------------------------------------------------------------------------------------
struct NetworkData;


//-----------------------------------------------------------------------------------------------
class Generation;


//-----------------------------------------------------------------------------------------------
class Generations
{
public:
	// Constructors & Destructors
	Generations();

	// Methods
	std::vector< NetworkData* > CreateFirstGeneration();
	Generation* CreateNextGeneration();
	void AddGenome( Genome* genome );

public:
	std::vector< Generation* > m_generations;
	Generation* m_currentGeneration;
};