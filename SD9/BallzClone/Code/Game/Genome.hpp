#pragma once


//-----------------------------------------------------------------------------------------------
class NeuralNetwork;


//-----------------------------------------------------------------------------------------------
class Genome
{
public:
	Genome( float score, NeuralNetwork* network );

public:
	float m_score;
	NeuralNetwork* m_network;
};