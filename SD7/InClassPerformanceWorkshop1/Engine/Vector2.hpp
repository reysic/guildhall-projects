//-----------------------------------------------------------------------------------------------
// Vector2.hpp
//
#pragma once

//-----------------------------------------------------------------------------------------------
#include "Engine/EngineBase.hpp"
#include "Engine/MathUtilities.hpp"
#include <math.h>


//-----------------------------------------------------------------------------------------------
class IntVector2;


/////////////////////////////////////////////////////////////////////////////////////////////////
class Vector2
{
public:
	float x;
	float y;

public:
	static const Vector2 ZERO;
	static const Vector2 ONE;

public:
	// Constructors
	~Vector2() {}
	Vector2() {} // Empty, do-nothing for ultra speed.  Newborn Vector2s are uninitialized!
	Vector2( const Vector2& copy );
	explicit Vector2( float initialX, float initialY );
	explicit Vector2( const char* asText );
	explicit Vector2( const IntVector2& fromIntVec2 );

	// Factory functions
	static const Vector2 CreateDirectionVectorDegrees( float yawDegrees );

	// Mutators
	void SetFromText( const char* asText );
	void SetXY( float newX, float newY );
	float Normalize();
	void SetUnitLengthAndYawDegrees( float yawDegrees );
	void SetUnitLengthAndYawRadians( float yawRadians );
	float SetLength( float newLength );
	void SetLengthAndYawDegrees( float newLength, float yawDegrees );
	void SetLengthAndYawRadians( float newLength, float yawRadians );
	void ScaleUniform( float scale );
	void ScaleNonUniform( const Vector2& perAxisScaleFactors );
	void InverseScaleNonUniform( const Vector2& perAxisDivisors );

	// Accessors
	void GetXY( float& out_x, float& out_y ) const;
	const float* GetAsFloatArray() const { return &x; }
	float* GetAsFloatArray() { return &x; }
	float CalcLength() const;
	float CalcLengthSquared() const;
	float CalcYawDegrees() const;
	float CalcYawRadians() const;
	const Vector2 GetRotated90Degrees() const;
	const Vector2 GetRotatedMinus90Degrees() const;
	const Vector2 GetNormalized() const;
	const Vector2 GetScaledNonUniform( const Vector2& perAxisScaleFactors ) const;
	const Vector2 GetInverseScaledNonUniform( const Vector2& perAxisDivisors ) const;
	const float CalcAspect() const;

	// Operators
	const bool operator == ( const Vector2& vectorToEqual ) const;
	const bool operator != ( const Vector2& vectorToNotEqual ) const;
	const Vector2 operator + ( const Vector2& vectorToAdd ) const;
	const Vector2 operator - ( const Vector2& vectorToSubtract ) const;
	const Vector2 operator - () const;
	const Vector2 operator * ( const float scale ) const;
	const Vector2 operator * ( const Vector2& perAxisScaleFactors ) const;
	const Vector2 operator / ( const float inverseScale ) const;
	const Vector2 operator / ( const Vector2& inverseScalesXY ) const;
	void operator *= ( const float scale );
	void operator *= ( const Vector2& perAxisScaleFactors );
	void operator += ( const Vector2& vectorToAdd );
	void operator -= ( const Vector2& vectorToSubtract );
	void operator /= ( const float inverseScale );
	void operator /= ( const Vector2& inverseScalesXY );

	// External/friend functions and utilities
	friend const float CalcDistance( const Vector2& a, const Vector2& b );
	friend const float CalcDistanceSquared( const Vector2& a, const Vector2& b );
	friend const Vector2 Interpolate( const Vector2& start, const Vector2& end, float fractionTowardEnd );
	friend const Vector2 operator * ( const float scale, const Vector2& vectorToScale );
	friend const float DotProduct( const Vector2& a, const Vector2& b );
	friend const float CrossProduct( const Vector2& a, const Vector2& b );
	friend Vector2 GetUnitVector2dForByteAngle( unsigned char byteAngle );
};


/////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------------------------
inline Vector2::Vector2( const char* asText )
	: x( 0.f )
	, y( 0.f )
{
	SetFromText( asText );
}


//-----------------------------------------------------------------------------------------------
inline Vector2::Vector2( const Vector2& copy )
	: x( copy.x )
	, y( copy.y )
{
}


//-----------------------------------------------------------------------------------------------
inline Vector2::Vector2( float initialX, float initialY )
	: x( initialX )
	, y( initialY )
{
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::SetXY( float newX, float newY )
{
	x = newX;
	y = newY;
}


//-----------------------------------------------------------------------------------------------
// Sets the vector's length to 1.  Returns the previous length of the vector.
//	Returns 0.0 and leaves vector unchanged if it was the zero vector to begin with.
//
inline float Vector2::Normalize()
{
	float lengthSquared = (x * x) + (y * y);
	if( lengthSquared > 0.f )
	{
		float previousLength = sqrtf( lengthSquared );
		float invLength = 1.f / previousLength;
		x *= invLength;
		y *= invLength;
		return previousLength;
	}
	else
	{
		return 0.f;
	}
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::SetUnitLengthAndYawDegrees( float yawDegrees )
{
	// TODO: replace with intrinsic(s) and/or hardcoded assembly for simultaneous cos/sin
	// TODO: provide also (or replace entirely with?) cos/sin table lookup version
	float yawRadians = ConvertDegreesToRadians( yawDegrees );
	x = cosf( yawRadians );
	y = sinf( yawRadians );
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::SetUnitLengthAndYawRadians( float yawRadians )
{
	// TODO: replace with intrinsic(s) and/or hardcoded assembly for simultaneous cos/sin
	// TODO: provide also (or replace entirely with?) cos/sin table lookup version
	x = cosf( yawRadians );
	y = sinf( yawRadians );
}


//-----------------------------------------------------------------------------------------------
inline float Vector2::SetLength( float newLength )
{
	float oldLengthSquared = (x * x) + (y * y);
	if( oldLengthSquared == 0.f )
		return 0.f;

	float oldLength = sqrtf( oldLengthSquared );
	float scale = newLength / oldLength;
	x *= scale;
	y *= scale;
	return oldLength;
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::SetLengthAndYawDegrees( float newLength, float yawDegrees )
{
	// TODO: replace with intrinsic(s) and/or hardcoded assembly for simultaneous cos/sin
	// TODO: provide also (or replace entirely with?) cos/sin table lookup version
	float yawRadians = ConvertDegreesToRadians( yawDegrees );
	x = newLength * cosf( yawRadians );
	y = newLength * sinf( yawRadians );
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::SetLengthAndYawRadians( float newLength, float yawRadians )
{
	// TODO: replace with intrinsic(s) and/or hardcoded assembly for simultaneous cos/sin
	// TODO: provide also (or replace entirely with?) cos/sin table lookup version
	x = newLength * cosf( yawRadians );
	y = newLength * sinf( yawRadians );
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::ScaleUniform( float scale )
{
	x *= scale;
	y *= scale;
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::ScaleNonUniform( const Vector2& perAxisScaleFactors )
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::InverseScaleNonUniform( const Vector2& perAxisDivisors )
{
	if( perAxisDivisors.x != 0.f )
		x /= perAxisDivisors.x;

	if( perAxisDivisors.y != 0.f )
		y /= perAxisDivisors.y;
}


//-----------------------------------------------------------------------------------------------
inline float Vector2::CalcLength() const
{
	float lengthSquared = CalcLengthSquared();
	if( lengthSquared > 0.f )
	{
		return sqrtf( lengthSquared );
	}
	else
	{
		return 0.f;
	}
}


//-----------------------------------------------------------------------------------------------
inline float Vector2::CalcLengthSquared() const
{
	return (x * x) + (y * y);
}


//-----------------------------------------------------------------------------------------------
inline float Vector2::CalcYawDegrees() const
{
	return ConvertRadiansToDegrees( atan2f( y, x ) );
}


//-----------------------------------------------------------------------------------------------
inline float Vector2::CalcYawRadians() const
{
	return atan2f( y, x );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::GetRotated90Degrees() const
{
	return Vector2( -y, x );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::GetRotatedMinus90Degrees() const
{
	return Vector2( y, -x );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::GetScaledNonUniform( const Vector2& perAxisScaleFactors ) const
{
	return Vector2( x * perAxisScaleFactors.x, y * perAxisScaleFactors.y );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::GetInverseScaledNonUniform( const Vector2& perAxisDivisors ) const
{
	Vector2 scaled( 0.f, 0.f );

	if( perAxisDivisors.x != 0.f )
		scaled.x = x / perAxisDivisors.x;

	if( perAxisDivisors.y != 0.f )
		scaled.y = y / perAxisDivisors.y;

	return scaled;
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 CalcLargestRectThatFitsWithinArea( float rectAspect, const Vector2& containingAreaSize )
{
	if( containingAreaSize.x < containingAreaSize.y * rectAspect )
	{
		// Containing area is taller/skinnier than aspect
		return Vector2( containingAreaSize.x, containingAreaSize.x / rectAspect );
	}
	else
	{
		// Containing area is shorter/fatter than aspect (or equal)
		return Vector2( containingAreaSize.y * rectAspect, containingAreaSize.y );
	}
}


//-----------------------------------------------------------------------------------------------
inline const float Vector2::CalcAspect() const
{
	if( y == 0.f )
		return 1.f;
	else
		return x / y;
}


//-----------------------------------------------------------------------------------------------
inline const bool Vector2::operator == ( const Vector2& vectorToEqual ) const
{
	return (x == vectorToEqual.x) && (y == vectorToEqual.y);
}


//-----------------------------------------------------------------------------------------------
inline const bool Vector2::operator != ( const Vector2& vectorToNotEqual ) const
{
	return (x != vectorToNotEqual.x) || (y != vectorToNotEqual.y);
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::operator + ( const Vector2& vectorToAdd ) const
{
	return Vector2( x + vectorToAdd.x, y + vectorToAdd.y );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::operator - ( const Vector2& vectorToSubtract ) const
{
	return Vector2( x - vectorToSubtract.x, y - vectorToSubtract.y );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::operator - () const
{
	return Vector2( -x, -y );
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::operator /= ( const float inverseScale )
{
	float scale = (1.f / inverseScale);
	x *= scale;
	y *= scale;
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::operator /= ( const Vector2& inverseScalesXY )
{
	x /= inverseScalesXY.x;
	y /= inverseScalesXY.y;
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::operator * ( const float scale ) const
{
	return Vector2( x * scale, y * scale );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::operator * ( const Vector2& perAxisScaleFactors ) const
{
	return Vector2( x * perAxisScaleFactors.x, y * perAxisScaleFactors.y );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::operator / ( const float inverseScale ) const
{
	float scale = (1.f / inverseScale);
	return Vector2( x * scale, y * scale );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Vector2::operator / ( const Vector2& inverseScalesXY ) const
{
	return Vector2( x / inverseScalesXY.x, y / inverseScalesXY.y );
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::operator *= ( const float scale )
{
	x *= scale;
	y *= scale;
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::operator *= ( const Vector2& perAxisScaleFactors )
{
	x *= perAxisScaleFactors.x;
	y *= perAxisScaleFactors.y;
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::operator += ( const Vector2& vectorToAdd )
{
	x += vectorToAdd.x;
	y += vectorToAdd.y;
}


//-----------------------------------------------------------------------------------------------
inline void Vector2::operator -= ( const Vector2& vectorToSubtract )
{
	x -= vectorToSubtract.x;
	y -= vectorToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
inline const float CalcDistance( const Vector2& a, const Vector2& b )
{
	const float xDisp = a.x - b.x;
	const float yDisp = a.y - b.y;
	return sqrtf( (xDisp * xDisp) + (yDisp * yDisp) );
}


//-----------------------------------------------------------------------------------------------
inline const float CalcDistanceSquared( const Vector2& a, const Vector2& b )
{
	const float xDisp = a.x - b.x;
	const float yDisp = a.y - b.y;
	return (xDisp * xDisp) + (yDisp * yDisp);
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 Interpolate( const Vector2& start, const Vector2& end, float fractionTowardEnd )
{
	return Vector2( start.x + fractionTowardEnd * (end.x - start.x),
					start.y + fractionTowardEnd * (end.y - start.y) );
}


//-----------------------------------------------------------------------------------------------
inline const Vector2 operator * ( const float scale, const Vector2& vectorToScale )
{
	return Vector2( scale * vectorToScale.x, scale * vectorToScale.y );
}


//-----------------------------------------------------------------------------------------------
inline const float DotProduct( const Vector2& a, const Vector2& b )
{
	return (a.x * b.x) + (a.y * b.y);
}


//-----------------------------------------------------------------------------------------------
// We choose to define a 2D cross product as returning a scalar, which is the magnitude of the
//	"z" component of the actual 3D cross product.  As in:
//
// [Ax,Ay,0] X [Bx,By,0] = [0,0,Cz]   <--- this function returns Cz
//
// This is still useful as a measure of the area of a parallelogram between the vectors, or
//	as the effective DotProduct of A vs. B's perpendicular.
//
inline const float CrossProduct( const Vector2& a, const Vector2& b )
{
	return (a.x * b.y) - (a.y * b.x);
}



