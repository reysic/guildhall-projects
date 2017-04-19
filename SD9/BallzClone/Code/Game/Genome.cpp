#include "Game/Genome.hpp"

//-----------------------------------------------------------------------------------------------
Genome::Genome( float score, NeuralNetwork* network )
	: m_score( score )
	, m_network( network )
{
}