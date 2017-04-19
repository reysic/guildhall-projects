#include "Game/NeuralNetwork.hpp"
#include "Game/NeuralNetworkLayer.hpp"
#include "Game/Neuron.hpp"
#include "Game/Neuroevolution.hpp"


//-----------------------------------------------------------------------------------------------
NeuralNetwork::NeuralNetwork()
{
}


//-----------------------------------------------------------------------------------------------
void NeuralNetwork::GeneratePerceptron( int numNeuronsInInputLayer, std::vector< int > numNeuronsInHiddenLayers, int numNeuronsInOutputLayer )
{
	// Create input layer
	int layerID = 0;
	int numNeuronsInPreviousLayer = 0;
	NeuralNetworkLayer* layer = new NeuralNetworkLayer( layerID );
	layer->Populate( numNeuronsInInputLayer, numNeuronsInPreviousLayer );
	numNeuronsInPreviousLayer = numNeuronsInInputLayer;
	m_neuralNetworkLayers.push_back( layer );
	layerID++;

	// Create hidden layer(s)
	for ( int numNeuronsInThisHiddenLayer : numNeuronsInHiddenLayers )
	{
		layer = new NeuralNetworkLayer( layerID );
		layer->Populate( numNeuronsInThisHiddenLayer, numNeuronsInPreviousLayer );
		numNeuronsInPreviousLayer = numNeuronsInThisHiddenLayer;
		m_neuralNetworkLayers.push_back( layer );
		layerID++;
	}

	// Create output layer
	layer = new NeuralNetworkLayer( layerID );
	layer->Populate( numNeuronsInOutputLayer, numNeuronsInPreviousLayer );
	m_neuralNetworkLayers.push_back( layer );
}


//-----------------------------------------------------------------------------------------------
NetworkData* NeuralNetwork::GetNetworkData() const
{
	NetworkData* networkData = new NetworkData();

	for ( NeuralNetworkLayer* thisLayer : m_neuralNetworkLayers )
	{
		networkData->numNeuronsPerLayer.push_back( thisLayer->m_neurons.size() );
		for ( Neuron* thisNeuron : thisLayer->m_neurons )
		{
			for ( float thisNeuronWeight : thisNeuron->m_weights )
			{
				networkData->neuronalWeights.push_back( thisNeuronWeight );
			}
		}
	}

	return networkData;
}


//-----------------------------------------------------------------------------------------------
void NeuralNetwork::SetNetworkData( const NetworkData& networkData )
{
	m_neuralNetworkLayers.clear();

	int layerID = 0;
	int layerIDWeights = 0;
	int numNeuronsInPreviousLayer = 0;

	for ( int numNeuronsInThisLayer : networkData.numNeuronsPerLayer )
	{
		NeuralNetworkLayer* layer = new NeuralNetworkLayer( layerID );
		layer->Populate( numNeuronsInThisLayer, numNeuronsInPreviousLayer );
		for ( Neuron* thisNeuron : layer->m_neurons )
		{
			for ( float thisNeuronWeight : thisNeuron->m_weights )
			{
				thisNeuronWeight = networkData.neuronalWeights[ layerIDWeights ];

				layerIDWeights++;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
std::vector< float > NeuralNetwork::ComputeOutput( std::vector< float > networkInputs )
{
	//#TODO: Confirm this logic is correct

	for ( unsigned int inputIndex = 0; inputIndex < networkInputs.size(); inputIndex++ )
	{
		// Set value for each neuron in input layer
		if ( m_neuralNetworkLayers[ 0 ] && m_neuralNetworkLayers[ 0 ]->m_neurons[ inputIndex ] )
		{
			m_neuralNetworkLayers[ 0 ]->m_neurons[ inputIndex ]->m_value = networkInputs[ inputIndex ];
		}
	}

	NeuralNetworkLayer* previousLayer = m_neuralNetworkLayers[ 0 ]; // Previous layer is input layer
	for ( unsigned int layerIndex = 1; layerIndex < m_neuralNetworkLayers.size(); layerIndex++ )
	{
		for ( int thisNeuronIndex = 0; m_neuralNetworkLayers[ layerIndex ]->m_neurons.size(); thisNeuronIndex++ )
		{
			// For each neuron in each layer
			float sum = 0.0f;
			for ( int previousNeuronIndex = 0; previousLayer->m_neurons.size(); previousNeuronIndex++ )
			{
				// Every neuron in the previous layer is an input to the neuron in the next layer
				sum += previousLayer->m_neurons[ previousNeuronIndex ]->m_value + m_neuralNetworkLayers[ layerIndex ]->m_neurons[ thisNeuronIndex ]->m_weights[ previousNeuronIndex ];
			}

			// Compute the activation of the neuron
			m_neuralNetworkLayers[ layerIndex ]->m_neurons[ thisNeuronIndex ]->m_value = Neuroevolution::ComputeActivation( sum );
		}
		previousLayer = m_neuralNetworkLayers[ layerIndex ];
	}

	// All outputs of the network
	std::vector< float > networkOutputs;
	NeuralNetworkLayer* lastLayer = m_neuralNetworkLayers[ m_neuralNetworkLayers.size() ];
	for ( Neuron* thisNeuron : lastLayer->m_neurons )
	{
		networkOutputs.push_back( thisNeuron->m_value );
	}
	return networkOutputs;
}
