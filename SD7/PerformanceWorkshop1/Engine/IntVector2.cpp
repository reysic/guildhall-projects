//-----------------------------------------------------------------------------------------------
// IntVector2.cpp
//
#include "Engine/IntVector2.hpp"
#include "Engine/Vector2.hpp"
#include "Engine/ErrorWarningAssert.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
STATIC const IntVector2 IntVector2::ZERO	( 0, 0 );
STATIC const IntVector2 IntVector2::ONE		( 1, 1 );


//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( const char* asText )
	: x( 0 )
	, y( 0 )
{
	SetFromText( asText );
}


//-----------------------------------------------------------------------------------------------
IntVector2::IntVector2( const Vector2& fromVec2 )
	: x( static_cast< int >( floor( 0.5f + fromVec2.x ) ) )
	, y( static_cast< int >( floor( 0.5f + fromVec2.y ) ) )
{
}


//-----------------------------------------------------------------------------------------------
void IntVector2::SetFromText( const char* asText )
{
	const int MAX_BUFFER_LENGTH = 1024;
	char writeBuffer[ MAX_BUFFER_LENGTH ];
	const char* readScan = asText;
	char* writeScan = writeBuffer;
	char* startOfSecondValue = writeBuffer;
	char* endOfTempBuffer = &writeBuffer[ MAX_BUFFER_LENGTH - 1 ];
	
	// Replace the interior ',' with a null terminator (and note start of second value)
	while( *readScan != '\0' && writeScan != endOfTempBuffer )
	{
		if( *readScan == ',' )
		{
			*writeScan = '\0';
			startOfSecondValue = writeScan + 1;
		}
		else
		{
			*writeScan = *readScan;
		}

		++ readScan;
		++ writeScan;
	}

	*writeScan = '\0';

	// Now that we've converted this into two [contiguous] null-terminated strings, just call atoi() on each
	x = atoi( writeBuffer );
	y = atoi( startOfSecondValue );
}

