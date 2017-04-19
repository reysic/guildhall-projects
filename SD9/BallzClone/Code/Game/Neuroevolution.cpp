#include <cmath>

#include "Game/Neuroevolution.hpp"


//-----------------------------------------------------------------------------------------------
#define STATIC // Do-nothing indicator that method/member is static in class definition


//-----------------------------------------------------------------------------------------------
STATIC float Neuroevolution::ComputeActivation( float input )
{
	//#TODO: Confirm meaning of ap
	float ap = ( -input ) / 1.0f;
	return ( 1.0f / ( 1.0f + exp( ap ) ) );
}