#pragma once

#include <vector>


//-----------------------------------------------------------------------------------------------
struct NetworkData;


//-----------------------------------------------------------------------------------------------
class Generation;
class Genome;


//-----------------------------------------------------------------------------------------------
class Generations
{
public:
	// Constructors & Destructors
	Generations();

	// Methods
	std::vector< NetworkData* > CreateFirstGeneration();
	std::vector< NetworkData* > CreateNextGeneration();
	void AddGenome( Genome* genome );

public:
	std::vector< Generation* > m_generations;
	Generation* m_currentGeneration;
};