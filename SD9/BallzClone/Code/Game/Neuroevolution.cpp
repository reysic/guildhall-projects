#include <cmath>

#include "Game/Neuroevolution.hpp"
#include "Game/NeuralNetwork.hpp"
#include "Game/Generations.hpp"
#include "Game/Genome.hpp"


//-----------------------------------------------------------------------------------------------
#define STATIC // Do-nothing indicator that method/member is static in class definition


//-----------------------------------------------------------------------------------------------
Neuroevolution::Neuroevolution()
	: m_generations( new Generations )
{
}


//-----------------------------------------------------------------------------------------------
STATIC float Neuroevolution::ComputeActivation( float input )
{
	// #TODO: Confirm meaning of ap
	float ap = ( -input ) / 1.0f;
	return ( 1.0f / ( 1.0f + exp( ap ) ) );
}


//-----------------------------------------------------------------------------------------------
void Neuroevolution::Restart()
{
	m_generations = new Generations();
}


//-----------------------------------------------------------------------------------------------
std::vector< NeuralNetwork* > Neuroevolution::CreateNextGeneration()
{
	// Vector of data to construct neural networks for this generation
	std::vector< NetworkData* > networkDataForThisGeneration;

	if ( m_generations->m_generations.size() == 0 )
	{
		// Create first generation
		networkDataForThisGeneration = m_generations->CreateFirstGeneration();
	}
	else
	{
		// Create next generation
		networkDataForThisGeneration = m_generations->CreateNextGeneration();
	}

	// Vector of neural networks for this generation, returned at end
	std::vector< NeuralNetwork* > neuralNetworksForThisGeneration;
	for ( NetworkData* thisNetworkData : networkDataForThisGeneration )
	{
		NeuralNetwork* thisNeuralNetwork = new NeuralNetwork();
		thisNeuralNetwork->SetNetworkData( networkDataForThisGeneration );
	}
}


//-----------------------------------------------------------------------------------------------
void Neuroevolution::NetworkScore( float score, NeuralNetwork* network )
{
	m_generations->AddGenome( new Genome( score, network ) );
}