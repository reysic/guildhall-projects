#pragma once

#include <vector>


//-----------------------------------------------------------------------------------------------
class Genome;


//-----------------------------------------------------------------------------------------------
class Generation
{
public:
	// Constructors & Destructors
	Generation();

	// Methods
	void AddGenome( Genome* genome );
	std::vector< Genome* > BreedGenomes( Genome* firstGenome, Genome* secondGenome, int numChildren );
	std::vector< Genome* > GenerateNextGeneration();

public:
	std::vector< Genome* > m_genomes;
};