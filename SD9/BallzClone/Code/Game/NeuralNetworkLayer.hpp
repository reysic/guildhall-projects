#pragma once

#include <vector>


//-----------------------------------------------------------------------------------------------
class Neuron;


//-----------------------------------------------------------------------------------------------
class NeuralNetworkLayer
{
public:
	// Constructors & Destructors
	NeuralNetworkLayer( int layerID );

	// Methods
	void Populate( int numNeuronsInLayer, int numWeightsPerNeuron );

public:
	int m_id;
	std::vector< Neuron* > m_neurons;
};