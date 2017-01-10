//-----------------------------------------------------------------------------------------------
// IntVector3.hpp
//
#pragma once
#ifndef include_IntVector3
#define include_IntVector3

//-----------------------------------------------------------------------------------------------
#include "Engine/MathUtilities.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////
class IntVector3
{
public:
	int x;
	int y;
	int z;

public:
	static const IntVector3 ZERO;
	static const IntVector3 ONE;

public:
	// Constructors
	~IntVector3() {}
	IntVector3() {}
	IntVector3( const IntVector3& copy );
	explicit IntVector3( int initialX, int initialY, int initialZ );
	explicit IntVector3( const char* asText );

	// Mutators
	void SetFromText( const char* asText );
	void SetXYZ( int newX, int newY, int newZ );

	// Accessors
	void GetXYZ( int& out_x, int& out_y, int& out_z ) const;
	const int* GetAsIntArray() const { return &x; }
	int* GetAsIntArray() { return &x; }

	// Operators
	const bool operator == ( const IntVector3& comparison ) const;
	const bool operator != ( const IntVector3& comparison ) const;
	const IntVector3 operator + ( const IntVector3& vectorToAdd ) const;
	const IntVector3 operator - ( const IntVector3& vectorToSubtract ) const;
};


//-----------------------------------------------------------------------------------------------
inline IntVector3::IntVector3( const IntVector3& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{
}


//-----------------------------------------------------------------------------------------------
inline IntVector3::IntVector3( const int initialX, const int initialY, const int initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}


//-----------------------------------------------------------------------------------------------
inline void IntVector3::SetXYZ( int newX, int newY, int newZ )
{
	x = newX;
	y = newY;
	z = newZ;
}


//-----------------------------------------------------------------------------------------------
inline const bool IntVector3::operator == ( const IntVector3& comparison ) const
{
	return( (x == comparison.x) && (y == comparison.y) && (z == comparison.z) );
}


//-----------------------------------------------------------------------------------------------
inline const bool IntVector3::operator != ( const IntVector3& comparison ) const
{
	return( (x != comparison.x) || (y != comparison.y) || (z != comparison.z) );
}


//-----------------------------------------------------------------------------------------------
inline const IntVector3 IntVector3::operator + ( const IntVector3& vectorToAdd ) const
{
	return IntVector3( x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z );
}


//-----------------------------------------------------------------------------------------------
inline const IntVector3 IntVector3::operator - ( const IntVector3& vectorToSubtract ) const
{
	return IntVector3( x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z );
}



#endif // include_IntVector3
