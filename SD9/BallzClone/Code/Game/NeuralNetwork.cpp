#include "Game/NeuralNetwork.hpp"
#include "Game/NeuralNetworkLayer.hpp"
#include "Game/Neuron.hpp"


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

}
