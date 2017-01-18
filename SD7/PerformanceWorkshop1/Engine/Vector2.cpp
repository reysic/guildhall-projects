//-----------------------------------------------------------------------------------------------
// Vector2.cpp
//
#include "Engine/Vector2.hpp"
#include "Engine/MathUtilities.hpp"
#include "Engine/IntVector2.hpp"
#include "Engine/ErrorWarningAssert.hpp"
#include <stdlib.h>


//-----------------------------------------------------------------------------------------------
STATIC const Vector2 Vector2::ZERO		( 0.f, 0.f );
STATIC const Vector2 Vector2::ONE		( 1.f, 1.f );


//-----------------------------------------------------------------------------------------------
Vector2::Vector2( const IntVector2& fromIntVec2 )
	: x( static_cast< float >( fromIntVec2.x ) )
	, y( static_cast< float >( fromIntVec2.y ) )
{
}


//-----------------------------------------------------------------------------------------------
STATIC const Vector2 Vector2::CreateDirectionVectorDegrees( float yawDegrees )
{
	return Vector2( CosDegrees( yawDegrees ), SinDegrees( yawDegrees ) );
}


//-----------------------------------------------------------------------------------------------
void Vector2::SetFromText( const char* asText )
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

	// Now that we've converted this into two [contiguous] null-terminated strings, just call atof() on each
	x = (float) atof( writeBuffer );
	y = (float) atof( startOfSecondValue );
}


//-----------------------------------------------------------------------------------------------
const Vector2 Vector2::GetNormalized() const
{
	float length = CalcLength();
	if( length == 0.f )
	{
		return Vector2::ZERO;
	}
	else
	{
		float scale = 1.f / length;
		return Vector2( x * scale, y * scale );
	}
}


//-----------------------------------------------------------------------------------------------
// Fast utility function for picking one of 256 unit-length 2D vectors evenly distributed around
//	unit circle, where byteAngle [0,256) maps to degrees [0,360), so 90 degrees is byteAngle 64.
//
// Exploits the fact that cosine is just 90 degrees "ahead" of sine.
//
Vector2 GetUnitVector2dForByteAngle( unsigned char byteAngle )
{
	unsigned int cosineIndex = 64 + (int) byteAngle; // 64/256 is equivalent to 90/360 degrees
	unsigned char cosineByte = (unsigned char)( cosineIndex & 0x000000FF ); // wrap 256+ to 0+
	return Vector2( g_sinValuesForByteAngles[ cosineByte ], g_sinValuesForByteAngles[ byteAngle ] );
}


