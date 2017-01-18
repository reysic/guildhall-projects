//-----------------------------------------------------------------------------------------------
// MathUtilities.hpp
//
#pragma once
#ifndef include_MathUtilities
#define include_MathUtilities

//-----------------------------------------------------------------------------------------------
#include "Engine/EngineBase.hpp"


//-----------------------------------------------------------------------------------------------
class Vector2;


//-----------------------------------------------------------------------------------------------
// Constants
//
constexpr double TWO_PI			= 6.283185307179586476925286766559;
constexpr double PI				= 3.1415926535897932384626433832795;
constexpr double HALF_PI		= 1.5707963267948966192313216916398;
constexpr double QUARTER_PI		= 0.78539816339744830961566084581988;
constexpr double SQRT_2			= 1.4142135623730950488016887242097;
constexpr double SQRT_3			= 1.7320508075688772935274463415059;
constexpr double SQRT_2_OVER_2	= 0.70710678118654752440084436210485;
constexpr double SQRT_3_OVER_2	= 0.86602540378443864676372317075294;
constexpr double SQRT_3_OVER_3	= 0.57735026918962576450914878050196;
constexpr double SQRT_3_OVER_6	= 0.28867513459481288225457439025098;

constexpr float fTWO_PI			= 6.283185307179586476925286766559f;
constexpr float fPI				= 3.1415926535897932384626433832795f;
constexpr float fHALF_PI		= 1.5707963267948966192313216916398f;
constexpr float fQUARTER_PI		= 0.78539816339744830961566084581988f;
constexpr float fSQRT_2			= 1.4142135623730950488016887242097f;
constexpr float fSQRT_3			= 1.7320508075688772935274463415059f;
constexpr float fSQRT_2_OVER_2	= 0.70710678118654752440084436210485f;
constexpr float fSQRT_3_OVER_2	= 0.86602540378443864676372317075294f;
constexpr float fSQRT_3_OVER_3	= 0.57735026918962576450914878050196f;
constexpr float fSQRT_3_OVER_6	= 0.28867513459481288225457439025098f;


//-----------------------------------------------------------------------------------------------
// Lookup tables
extern const float g_sinValuesForByteAngles[ 256 ];


//-----------------------------------------------------------------------------------------------
// Basic math utilities
//
template< typename T > const T& GetMin( const T& a, const T& b );
float GetMinFloat( const float a, const float b );
float GetMinFloat( const float a, const float b, const float c );
float GetMinFloat( const float a, const float b, const float c, const float d );
float GetMaxFloat( const float a, const float b );
float GetMaxFloat( const float a, const float b, const float c );
float GetMaxFloat( const float a, const float b, const float c, const float d );
template< typename T > T Clamp( const T& originalValue, const T& minValue, const T& maxValue );
float ClampZeroToOne( float value );
float ClampNegOneToOne( float value );
float RangeMapFloat( const float inputValue, const float inputRangeStart, const float inputRangeEnd, const float outputRangeStart, const float outputRangeEnd );
const int CalcLogBase2( int x );
const bool IsPowerOfTwo( int x );


//-----------------------------------------------------------------------------------------------
// Angle and Trig utilities
// 
float ConvertRadiansToDegrees( float radians );
float ConvertDegreesToRadians( float degrees );
double ConvertRadiansToDegrees( double radians );
double ConvertDegreesToRadians( double degrees );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float Atan2Degrees( float y, float x );
float Atan2Degrees( const Vector2& displacement );
double CosDegrees( double degrees );
double SinDegrees( double degrees );
double Atan2Degrees( double y, double x );
float CalcShortestAngularDeltaDegrees( float startDegrees, float endDegrees );


//-----------------------------------------------------------------------------------------------
// Interpolation/easing/tweening utilities
//
float Interpolate( float fromValue, float toValue, float fractionComplete );
float SmoothStep3( float inputZeroToOne );
float SmoothStep5( float inputZeroToOne );
float SmoothStart2( float inputZeroToOne );
float SmoothStop2( float inputZeroToOne );


//-----------------------------------------------------------------------------------------------
// Random number utilities
//
int GetRandomIntInRange( int minValueInclusive, int maxValueInclusive );
int GetRandomIntLessThan( int maxValueNotInclusive );
float GetRandomFloatZeroToOne();
float GetRandomFloatInRange( float minimumInclusive, float maximumInclusive );
bool GetRandomChance( float probabilityOfReturningTrue );


/////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------
template< typename T >
const T& GetMin( const T& a, const T& b )
{
	return a < b ? a : b;
}


//-----------------------------------------------------------------------------------------------
inline float GetMinFloat( const float a, const float b )
{
	return a < b ? a : b;
}


//-----------------------------------------------------------------------------------------------
inline float GetMinFloat( const float a, const float b, const float c )
{
	if( a < b )
	{
		return a < c ? a : c;
	}
	else
	{
		return b < c ? b : c;
	}
}


//-----------------------------------------------------------------------------------------------
inline float GetMinFloat( const float a, const float b, const float c, const float d )
{
	if( a < b )
	{
		if( a < c )
		{
			return a < d ? a : d;
		}
		else
		{
			return c < d ? c : d;
		}
	}
	else
	{
		if( b < c )
		{
			return b < d ? b : d;
		}
		else
		{
			return c < d ? c : d;
		}
	}
}


//-----------------------------------------------------------------------------------------------
inline float GetMaxFloat( const float a, const float b )
{
	return a > b ? a : b;
}


//-----------------------------------------------------------------------------------------------
inline float GetMaxFloat( const float a, const float b, const float c )
{
	if( a > b )
	{
		return a > c ? a : c;
	}
	else
	{
		return b > c ? b : c;
	}
}


//-----------------------------------------------------------------------------------------------
inline float GetMaxFloat( const float a, const float b, const float c, const float d )
{
	if( a > b )
	{
		if( a > c )
		{
			return a > d ? a : d;
		}
		else
		{
			return c > d ? c : d;
		}
	}
	else
	{
		if( b > c )
		{
			return b > d ? b : d;
		}
		else
		{
			return c > d ? c : d;
		}
	}
}


//-----------------------------------------------------------------------------------------------
template< typename T >
T Clamp( const T& originalValue, const T& minValue, const T& maxValue )
{
	T clampedValue = originalValue;
	if( clampedValue < minValue )
		clampedValue = minValue;
	else if( clampedValue > maxValue )
		clampedValue = maxValue;

	return clampedValue;
}


//-----------------------------------------------------------------------------------------------
inline float ClampZeroToOne( float value )
{
	if( value < 0.f )
		return 0.f;
	else if( value > 1.f )
		return 1.f;
	else
		return value;
}


//-----------------------------------------------------------------------------------------------
inline float ClampNegOneToOne( float value )
{
	if( value < -1.f )
		return -1.f;
	else if( value > 1.f )
		return 1.f;
	else
		return value;
}


//-----------------------------------------------------------------------------------------------
inline float RangeMapFloat( const float inputValue, const float inputRangeStart, const float inputRangeEnd, const float outputRangeStart, const float outputRangeEnd )
{
	float inputRangeWidth = inputRangeEnd - inputRangeStart;
	if( inputRangeWidth != 0.f )
	{
		float fractionInRange = (inputValue - inputRangeStart) / inputRangeWidth;
		return outputRangeStart + (fractionInRange * (outputRangeEnd - outputRangeStart));
	}
	else
	{
		return 0.5f * (outputRangeStart + outputRangeEnd);
	}
}


//-----------------------------------------------------------------------------------------------
inline float ConvertRadiansToDegrees( float radians )
{
	return radians * (180.f / fPI);
}


//-----------------------------------------------------------------------------------------------
inline double ConvertRadiansToDegrees( double radians )
{
	return radians * (180.0 / PI);
}


//-----------------------------------------------------------------------------------------------
inline float ConvertDegreesToRadians( float degrees )
{
	return degrees * (fPI / 180.f);
}


//-----------------------------------------------------------------------------------------------
inline double ConvertDegreesToRadians( double degrees )
{
	return degrees * (PI / 180.0);
}


//-----------------------------------------------------------------------------------------------
inline float CalcShortestAngularDeltaDegrees( float startDegrees, float endDegrees )
{
	float angularDisplacementDegrees = endDegrees - startDegrees;
	while( angularDisplacementDegrees > 180.f )
		angularDisplacementDegrees -= 360.f;
	while( angularDisplacementDegrees < -180.f )
		angularDisplacementDegrees += 360.f;

	return angularDisplacementDegrees;
}


//-----------------------------------------------------------------------------------------------
// For a given positive power-of-two x, find L = CalcLogBase2( x ) such that 2^L = X.
//
// For example, CalcLogBase2( 32 ) = 5, since 2^5 = 32.
//
inline const int CalcLogBase2( int x )
{
	int numBitShifts = 0;
	while( x )
	{
		x >>= 1;
		++ numBitShifts;
	}

	return numBitShifts-1; // e.g. if we shifted 6 times right before zeroing out, we were 00100000 (32), which is 2^5 (or 1<<5).
}


//-----------------------------------------------------------------------------------------------
// Returns true if x is a positive power of two (e.g. 1, 2, 4, 8, 16, 32, 64, 128, 256, 512...)
//
inline const bool IsPowerOfTwo( int x )
{
	return x && !(x & (x - 1));
}

//-----------------------------------------------------------------------------------------------
inline float Interpolate( float fromValue, float toValue, float fractionComplete )
{
	return fromValue + (toValue - fromValue) * fractionComplete;
}


//-----------------------------------------------------------------------------------------------
inline float SmoothStep3( float inputZeroToOne )
{
	return inputZeroToOne * inputZeroToOne * (3.f - (2.f * inputZeroToOne));
}	


//-----------------------------------------------------------------------------------------------
inline float SmoothStep5( float inputZeroToOne )
{
	const float& t = inputZeroToOne;
	return t * t * t * ( t * (t * 6.f - 15.f) + 10.f );
}	


//-----------------------------------------------------------------------------------------------
inline float SmoothStart2( float inputZeroToOne )
{
	return (inputZeroToOne * inputZeroToOne);
}	


//-----------------------------------------------------------------------------------------------
inline float SmoothStop2( float inputZeroToOne )
{
	float oneMinusInput = 1.f - inputZeroToOne;
	return 1.f - (oneMinusInput * oneMinusInput);
}	


//-----------------------------------------------------------------------------------------------
inline float GetRandomFloatInRange( float minimumInclusive, float maximumInclusive )
{
	const float randomZeroToOne = GetRandomFloatZeroToOne();
	return minimumInclusive + ( randomZeroToOne * (maximumInclusive - minimumInclusive) );
}


//-----------------------------------------------------------------------------------------------
inline bool GetRandomChance( float probabilityOfReturningTrue )
{
	if( probabilityOfReturningTrue >= 1.f )
		return true;

	return GetRandomFloatZeroToOne() < probabilityOfReturningTrue;
}



#endif // include_MathUtilities
