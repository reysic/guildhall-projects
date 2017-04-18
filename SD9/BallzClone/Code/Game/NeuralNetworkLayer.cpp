#include "Game/NeuralNetworkLayer.hpp"
#include "Game/Neuron.hpp"


//-----------------------------------------------------------------------------------------------
NeuralNetworkLayer::NeuralNetworkLayer( int layerID )
	: m_id( layerID || 0 )
{
}


//-----------------------------------------------------------------------------------------------
void NeuralNetworkLayer::Populate( int numNeuronsInLayer, int numWeightsPerNeuron )
{
	m_neurons.clear();

	for ( int numNeuron = 0; numNeuron < numNeuronsInLayer; numNeuron++ )
	{
		Neuron* neuron = new Neuron();
		neuron->Populate( numWeightsPerNeuron );
		m_neurons.push_back( neuron );
	}
}