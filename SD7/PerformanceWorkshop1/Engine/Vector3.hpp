//-----------------------------------------------------------------------------------------------
// Vector3.hpp
//
#pragma once
#ifndef include_Vector3
#define include_Vector3

//-----------------------------------------------------------------------------------------------
#include "Engine/MathUtilities.hpp"


/////////////////////////////////////////////////////////////////////////////////////////////////
class Vector3
{
public:
	float x;
	float y;
	float z;

public:
	static const Vector3 ZERO;
	static const Vector3 ONE;

public:
	// Constructors
	~Vector3() {}
	Vector3() {}
	Vector3( const Vector3& copy );
	explicit Vector3( float initialX, float initialY, float initialZ );

	// Mutators
	void SetXYZ( float newX, float newY, float newZ );

	// Accessors
	const float* GetAsFloatArray() const { return &x; }
	float* GetAsFloatArray() { return &x; }

	// Operators
	const bool operator == ( const Vector3& vectorToEqual ) const;
	const bool operator != ( const Vector3& vectorToNotEqual ) const;
	const Vector3 operator + ( const Vector3& vectorToAdd ) const;
	const Vector3 operator - ( const Vector3& vectorToSubtract ) const;
	const Vector3 operator * ( const float scale ) const;
	const Vector3 operator / ( const float inverseScale ) const;
	void operator *= ( const float scale );
	void operator += ( const Vector3& vectorToAdd );
	const Vector3 operator - () const;

	// External/friend functions and utilities
	friend const float CalcDistance( const Vector3& a, const Vector3& b );
	friend const float CalcDistanceSquared( const Vector3& a, const Vector3& b );
	friend const Vector3 Interpolate( const Vector3& start, const Vector3& end, float fractionTowardEnd );
	friend const Vector3 operator * ( const float scale, const Vector3& vectorToScale );
	friend const float DotProduct( const Vector3& a, const Vector3& b );
};


/////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------
inline Vector3::Vector3( const Vector3& copy )
	: x( copy.x )
	, y( copy.y )
	, z( copy.z )
{
}


//-----------------------------------------------------------------------------------------------
inline Vector3::Vector3( float initialX, float initialY, float initialZ )
	: x( initialX )
	, y( initialY )
	, z( initialZ )
{
}


//-----------------------------------------------------------------------------------------------
inline void Vector3::SetXYZ( float newX, float newY, float newZ )
{
	x = newX;
	y = newY;
	z = newZ;
}



//-----------------------------------------------------------------------------------------------
inline const bool Vector3::operator == ( const Vector3& vectorToEqual ) const
{
	return (x == vectorToEqual.x) && (y == vectorToEqual.y) && (z == vectorToEqual.z);
}


//-----------------------------------------------------------------------------------------------
inline const bool Vector3::operator != ( const Vector3& vectorToNotEqual ) const
{
	return (x != vectorToNotEqual.x) || (y != vectorToNotEqual.y) || (z != vectorToNotEqual.z);
}


//-----------------------------------------------------------------------------------------------
inline const Vector3 Vector3::operator + ( const Vector3& vectorToAdd ) const
{
	return Vector3( x + vectorToAdd.x, y + vectorToAdd.y, z + vectorToAdd.z );
}


//-----------------------------------------------------------------------------------------------
inline const Vector3 Vector3::operator - ( const Vector3& vectorToSubtract ) const
{
	return Vector3( x - vectorToSubtract.x, y - vectorToSubtract.y, z - vectorToSubtract.z );
}


//-----------------------------------------------------------------------------------------------
inline const Vector3 Vector3::operator * ( const float scale ) const
{
	return Vector3( x * scale, y * scale, z * scale );
}


//-----------------------------------------------------------------------------------------------
inline const Vector3 Vector3::operator / ( const float inverseScale ) const
{
	const float scale = (1.f / inverseScale);
	return Vector3( x * scale, y * scale, z * scale );
}


//-----------------------------------------------------------------------------------------------
inline void Vector3::operator *= ( const float scale )
{
	x *= scale;
	y *= scale;
	z *= scale;
}


//-----------------------------------------------------------------------------------------------
inline void Vector3::operator += ( const Vector3& vectorToAdd )
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
	z += vectorToAdd.z;
}


//-----------------------------------------------------------------------------------------------
inline const Vector3 Vector3::operator - () const
{
	return Vector3( -x, -y, -z );
}


//-----------------------------------------------------------------------------------------------
inline const float CalcDistanceSquared( const Vector3& a, const Vector3& b )
{
	const float xDisp = a.x - b.x;
	const float yDisp = a.y - b.y;
	const float zDisp = a.z - b.z;
	return (xDisp * xDisp) + (yDisp * yDisp) + (zDisp * zDisp);
}


//-----------------------------------------------------------------------------------------------
inline const Vector3 Interpolate( const Vector3& start, const Vector3& end, float fractionTowardEnd )
{
	return Vector3( start.x + fractionTowardEnd * (end.x - start.x),
					start.y + fractionTowardEnd * (end.y - start.y),
					start.z + fractionTowardEnd * (end.z - start.z) );
}


//-----------------------------------------------------------------------------------------------
inline const Vector3 operator * ( const float scale, const Vector3& vectorToScale )
{
	return Vector3( scale * vectorToScale.x, scale * vectorToScale.y, scale * vectorToScale.z );
}


//-----------------------------------------------------------------------------------------------
inline const float DotProduct( const Vector3& a, const Vector3& b )
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}



#endif // include_Vector3
