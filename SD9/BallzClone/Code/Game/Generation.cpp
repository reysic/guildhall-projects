#include "Engine/Math/MathUtils.hpp"

#include "Game/Generation.hpp"
#include "Game/Genome.hpp"
#include "Game/NeuralNetwork.hpp"


//-----------------------------------------------------------------------------------------------
Generation::Generation()
{
}


//-----------------------------------------------------------------------------------------------
// Locate position to insert genome, genomes should remain sorted in descending order
void Generation::AddGenome( Genome* genome )
{
	if ( m_genomes.size() == 0 )
	{
		// No genomes added yet, so just push back
		m_genomes.push_back( genome );
		return;
	}

	for ( unsigned int genomeIndex = 0; genomeIndex < m_genomes.size(); genomeIndex++ )
	{
		if ( genome->m_score > m_genomes[ genomeIndex ]->m_score )
		{
			// Insert before genome with lower score
			m_genomes.insert( m_genomes.begin() + genomeIndex, genome );
			return;
		}
	}

	// Score is lower than all other genomes, so just push back
	m_genomes.push_back( genome );
}


//-----------------------------------------------------------------------------------------------
// Breed genomes to give children with random crossover and mutations
std::vector< Genome* > Generation::BreedGenomes( Genome* firstGenome, Genome* secondGenome, int numChildren )
{
	// #TODO: Confirm that this should be returning a std::vector< Genome* > and not a flat array
	std::vector< Genome* > children;

	NetworkData* secondGenomeNetworkData = secondGenome->m_network->GetNetworkData();

	for ( int childIndex = 0; childIndex < numChildren; childIndex++ )
	{
		Genome* child = new Genome( firstGenome->m_score, firstGenome->m_network );
		NetworkData* childNetworkData = child->m_network->GetNetworkData();

		for ( unsigned int weightIndex = 0; weightIndex < secondGenomeNetworkData->neuronalWeights.size(); weightIndex++ )
		{
			// Genetic crossover
			if ( GetRandomChance( CROSSOVER_FACTOR ) )
			{
				childNetworkData->neuronalWeights[ weightIndex ] = secondGenomeNetworkData->neuronalWeights[ weightIndex ];
			}
		}

		for ( unsigned int weightIndex = 0; weightIndex < childNetworkData->neuronalWeights.size(); weightIndex++ )
		{
			// Mutation
			if ( GetRandomFloatZeroToOne() <= MUTATION_RATE )
			{
				childNetworkData->neuronalWeights[ weightIndex ] += GetRandomFloatZeroToOne() * MUTATION_RANGE * 2.0f - MUTATION_RANGE;
			}
		}

		children.push_back( child );
	}

	return children;
}


//-----------------------------------------------------------------------------------------------
Generation* Generation::GenerateNextGeneration()
{
	std::vector< Genome* > nextGenerationGenomes;

	for ( int genomeIndex = 0; genomeIndex < round( ELITISM * POPULATION ); genomeIndex++ )
	{
		if ( nextGenerationGenomes.size() < POPULATION )
		{
			nextGenerationGenomes.push_back( m_genomes[ genomeIndex ] );
		}
	}

	// #TODO: Are we actually iterating over genomes here?
	for ( int genomeIndex = 0; genomeIndex < round( RANDOM_BEHAVIOR * POPULATION ); genomeIndex++ )
	{
		// #TODO: Confirm we're just interested in the first genome
		for ( float thisWeight : m_genomes[ 0 ]->m_network->GetNetworkData()->neuronalWeights )
		{
			thisWeight = GetRandomFloatNegativeOneToOne();
		}
		if ( nextGenerationGenomes.size() < POPULATION )
		{
			nextGenerationGenomes.push_back( m_genomes[ 0 ] );
		}
	}

	int sentinel = 0;
	while ( true )
	{
		// #TODO: Figure out what 'i' is
		for ( int i = 0; i < sentinel; i++ )
		{
			// Create the children and push them into the nextGeneration vector
			std::vector< Genome* > children = BreedGenomes( m_genomes[ i ], m_genomes[ sentinel ], NUM_CHILDREN > 0 ? NUM_CHILDREN : 1 );
			for ( Genome* child : children )
			{
				nextGenerationGenomes.push_back( child );
				if ( nextGenerationGenomes.size() >= POPULATION )
				{
					Generation* nextGeneration;
					nextGeneration->m_genomes = nextGenerationGenomes;
					return nextGeneration;
				}
			}
		}


		sentinel++;
		if ( sentinel >= ( m_genomes.size() - 1 ) )
		{
			sentinel = 0;
		}
	}
}
