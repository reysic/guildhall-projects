#pragma once

#include <vector>

//-----------------------------------------------------------------------------------------------
//#TODO: Move to Neuroevolution class
static const float CROSSOVER_FACTOR = 0.5f;
static const float MUTATION_RATE = 0.1f;
static const float MUTATION_RANGE = 0.5f;
static const float ELITISM = 0.2f;
static const float RANDOM_BEHAVIOR = 0.2f;
static const float POPULATION = 50.0f;
static const float NUM_CHILDREN = 1;


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
	Generation* GenerateNextGeneration();

public:
	std::vector< Genome* > m_genomes;
};