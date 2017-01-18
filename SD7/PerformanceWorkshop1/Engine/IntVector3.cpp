//-----------------------------------------------------------------------------------------------
// IntVector3.cpp
//
#include "Engine/IntVector3.hpp"
#include "Engine/ErrorWarningAssert.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
STATIC const IntVector3 IntVector3::ZERO	( 0, 0, 0 );
STATIC const IntVector3 IntVector3::ONE		( 1, 1, 1 );


//-----------------------------------------------------------------------------------------------
IntVector3::IntVector3( const char* asText )
	: x( 0 )
	, y( 0 )
	, z( 0 )
{
	SetFromText( asText );
}


//-----------------------------------------------------------------------------------------------
void IntVector3::SetFromText( const char* asText )
{
	UNUSED( asText );
	ERROR_AND_DIE( "IntVector3::SetFromText() not yet implemented!" );
}

