#include "Game/Generations.hpp"
#include "Game/Generation.hpp"
#include "Game/NeuralNetwork.hpp"
#include "Game/Genome.hpp"


//-----------------------------------------------------------------------------------------------
Generations::Generations()
	: m_currentGeneration( new Generation() )
{
}


//-----------------------------------------------------------------------------------------------
std::vector< NetworkData > Generations::CreateFirstGeneration()
{
	std::vector< NetworkData > out;

	for ( int populationIndex = 0; populationIndex < POPULATION; populationIndex++ )
	{
		NeuralNetwork* network = new NeuralNetwork();
		std::vector< int > numNeuronsInHiddenLayers;
		numNeuronsInHiddenLayers.push_back( 33 ); // Mean of neurons in input and output layers
		// #TODO: Make these parameters constants
		network->GeneratePerceptron( 65, numNeuronsInHiddenLayers, 1 );
		out.push_back( network->GetNetworkData() );
	}

	m_generations.push_back( new Generation() );
	return out;
}


//-----------------------------------------------------------------------------------------------
std::vector< NetworkData > Generations::CreateNextGeneration()
{
	std::vector< NetworkData > out;

	if ( m_generations.size() == 0 )
	{
		// Need to create first generation
		// #TODO: Handle this return
		//return nullptr;
	}

	out = m_generations[ m_generations.size() - 1 ]->GenerateNextGeneration();
	// #TODO: Is this what we should be adding to the vector?
	m_generations.push_back( new Generation() );
	//return nextGeneration;

	return out;
}


//-----------------------------------------------------------------------------------------------
void Generations::AddGenome( Genome* genome )
{
	if ( m_generations.size() == 0 )
	{
		// Can't add a genome if there are no generations
		return;
	}

	m_generations[ m_generations.size() - 1 ]->AddGenome( genome );
}
