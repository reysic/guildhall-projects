#pragma once

#include <vector>


//-----------------------------------------------------------------------------------------------
class NeuralNetworkLayer;


//-----------------------------------------------------------------------------------------------
struct NetworkData
{
	std::vector< int > numNeuronsPerLayer;
	std::vector< float > neuronalWeights; // Flat array of all neuronal weights
};


//-----------------------------------------------------------------------------------------------
class NeuralNetwork
{
public:
	// Constructors & Destructors
	NeuralNetwork();

	// Methods
	void GeneratePerceptron( int numNeuronsInInputLayer, std::vector< int > numNeuronsInHiddenLayers, int numNeuronsInOutputLayer );
	NetworkData GetNetworkData() const;
	void SetNetworkData( const NetworkData& networkData );
	std::vector< float > ComputeOutput( std::vector< float > networkInputs );

public:
	std::vector< NeuralNetworkLayer* > m_neuralNetworkLayers;
};