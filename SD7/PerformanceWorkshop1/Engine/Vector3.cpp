//-----------------------------------------------------------------------------------------------
// Vector3.cpp
//
#include "Engine/Vector3.hpp"
#include "Engine/ErrorWarningAssert.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
STATIC const Vector3 Vector3::ZERO		( 0.f, 0.f, 0.f );
STATIC const Vector3 Vector3::ONE		( 1.f, 1.f, 1.f );


//-----------------------------------------------------------------------------------------------
const float CalcDistance( const Vector3& a, const Vector3& b )
{
	const float xDisp = a.x - b.x;
	const float yDisp = a.y - b.y;
	const float zDisp = a.z - b.z;
	return sqrtf( (xDisp * xDisp) + (yDisp * yDisp) + (zDisp * zDisp) );
}


