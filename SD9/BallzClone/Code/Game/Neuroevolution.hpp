#pragma once


//-----------------------------------------------------------------------------------------------
class Generations;
class NeuralNetwork;


//-----------------------------------------------------------------------------------------------
class Neuroevolution
{
public:
	// Constructors & Destructors
	Neuroevolution();

	// Methods
	static float Neuroevolution::ComputeActivation( float input );
	void Restart();
	std::vector< NeuralNetwork* > CreateNextGeneration();
	void NetworkScore( float score, NeuralNetwork* network );

public:
	Generations* m_generations;
};