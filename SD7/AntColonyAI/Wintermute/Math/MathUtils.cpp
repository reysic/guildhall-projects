#include "MathUtils.hpp"

#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
int GetRandomIntInRange( int minValueInclusive, int maxValueInclusive )
{
	return minValueInclusive + rand() % ( 1 + maxValueInclusive - minValueInclusive );
}