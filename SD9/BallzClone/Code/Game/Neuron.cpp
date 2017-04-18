#include "Engine/Math/MathUtils.hpp"

#include "Game/Neuron.hpp"


//-----------------------------------------------------------------------------------------------
Neuron::Neuron()
	: m_value( 0 )
{
}


//-----------------------------------------------------------------------------------------------
void Neuron::Populate( int numWeightsToInitialize )
{
	m_weights.clear();

	for ( int numWeight = 0; numWeight < numWeightsToInitialize; numWeight++ )
	{
		m_weights.push_back( GetRandomFloatNegativeOneToOne() );
	}
}