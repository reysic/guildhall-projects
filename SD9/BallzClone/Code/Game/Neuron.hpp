#pragma once

#include <vector>


//-----------------------------------------------------------------------------------------------
class Neuron
{
public:
	// Constructors & Destructors
	Neuron();

	// Methods
	void Populate( int numWeightsToInitialize );

public:
	float m_value;
	std::vector< float > m_weights;
};