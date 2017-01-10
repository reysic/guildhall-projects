//-----------------------------------------------------------------------------------------------
// IntVector2.hpp
//
#pragma once
#ifndef include_IntVector2
#define include_IntVector2

//-----------------------------------------------------------------------------------------------
#include "Engine/MathUtilities.hpp"


//-----------------------------------------------------------------------------------------------
class Vector2;


/////////////////////////////////////////////////////////////////////////////////////////////////
class IntVector2
{
public:
	int x;
	int y;

public:
	static const IntVector2 ZERO;
	static const IntVector2 ONE;

public:
	// Constructors
	~IntVector2() {}
	IntVector2() {}
	IntVector2( const IntVector2& copy );
	explicit IntVector2( int initialX, int initialY );
	explicit IntVector2( const char* asText );
	explicit IntVector2( const Vector2& fromVec2 );

	// Mutators
	void SetFromText( const char* asText );
	void SetXY( int newX, int newY );

	// Accessors
	void GetXY( int& out_x, int& out_y ) const;
	const int* GetAsIntArray() const { return &x; }
	int* GetAsIntArray() { return &x; }
	float CalcAspect() const;

	// Operators
	const bool operator == ( const IntVector2& comparison ) const;
	const bool operator != ( const IntVector2& comparison ) const;
	const IntVector2 operator + ( const IntVector2& vectorToAdd ) const;
	const IntVector2 operator - ( const IntVector2& vectorToSubtract ) const;
	const IntVector2 operator * ( const int scale ) const;
	const IntVector2 operator / ( const int divisor ) const;
	void operator += ( const IntVector2& vectorToAdd );
	void operator -= ( const IntVector2& vectorToSubtract );
};


//-----------------------------------------------------------------------------------------------
inline IntVector2::IntVector2( const IntVector2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
inline IntVector2::IntVector2( const int initialX, const int initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
inline void IntVector2::SetXY( int newX, int newY )
{
	x = newX;
	y = newY;
}


//-----------------------------------------------------------------------------------------------
inline float IntVector2::CalcAspect() const
{
	if( y == 0 )
		return 1.f;
	else
	{
		return static_cast< float >( x ) / static_cast< float >( y );
	}
}


//-----------------------------------------------------------------------------------------------
inline const bool IntVector2::operator == ( const IntVector2& comparison ) const
{
	return( x == comparison.x && y == comparison.y );
}


//-----------------------------------------------------------------------------------------------
inline const bool IntVector2::operator != ( const IntVector2& comparison ) const
{
	return( x != comparison.x || y != comparison.y );
}


//-----------------------------------------------------------------------------------------------
inline const IntVector2 IntVector2::operator + ( const IntVector2& vectorToAdd ) const
{
	return IntVector2( x + vectorToAdd.x, y + vectorToAdd.y );
}


//-----------------------------------------------------------------------------------------------
inline const IntVector2 IntVector2::operator - ( const IntVector2& vectorToSubtract ) const
{
	return IntVector2( x - vectorToSubtract.x, y - vectorToSubtract.y );
}


//-----------------------------------------------------------------------------------------------
inline const IntVector2 IntVector2::operator * ( const int scale ) const
{
	IntVector2 scaled;
	scaled.x = x * scale;
	scaled.y = x * scale;
	return scaled;
}


//-----------------------------------------------------------------------------------------------
inline const IntVector2 IntVector2::operator / ( const int divisor ) const
{
	IntVector2 scaled;
	scaled.x = x / divisor;
	scaled.y = x / divisor;
	return scaled;
}


//-----------------------------------------------------------------------------------------------
inline void IntVector2::operator += ( const IntVector2& vectorToAdd )
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}


//-----------------------------------------------------------------------------------------------
inline void IntVector2::operator -= ( const IntVector2& vectorToSubtract )
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}




#endif // include_IntVector2
