#include "Engine/Math/Vector3Double.hpp"

Vector3Double::Vector3Double()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}


Vector3Double::Vector3Double( const Vector3Double& v ) : x( v.x ), y( v.y ), z( v.z )
{

}

Vector3Double::Vector3Double( double initialX, double initialY, double initialZ )
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

Vector3Double::Vector3Double( const Vector3& v )
{
	x = v.x;
	y = v.y;
	z = v.z;
}

void Vector3Double::SetXYZ( const double& newX, const double& newY, const double& newZ )
{
	x = newX;
	y = newY;
	z = newZ;
}

double Vector3Double::GetMagnitude() const
{
	return ( double ) sqrt( x*x + y*y + z*z );
}

void Vector3Double::SetNormalized()
{
	double magnitude = this->GetMagnitude();
	if ( magnitude == 0.0 )
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}
	else
	{
		double oneOverMagnitude = ( 1.0 / magnitude );
		x *= oneOverMagnitude;
		y *= oneOverMagnitude;
		z *= oneOverMagnitude;
	}

}

double Vector3Double::DistanceToSquared( const Vector3Double& otherVec )
{
	double a = (x - otherVec.x) * (x - otherVec.x);
	double b = (y - otherVec.y) * (y - otherVec.y);
	double c = (z - otherVec.z) * (z - otherVec.z);

	return a + b + c;
}

float Vector3Double::DistanceToSquared( const Vector3& otherVec )
{
	float a = ( float ) ( (x - otherVec.x) * (x - otherVec.x) );
	float b = ( float ) ( (y - otherVec.y) * (y - otherVec.y) );
	float c = ( float ) ( (z - otherVec.z) * (z - otherVec.z) );

	return a + b + c;
}

Vector3Double Vector3Double::GetNormalized() const
{
	double magnitude = GetMagnitude();
	if ( magnitude == 0.0 )
		return Vector3Double( 0.0, 0.0, 0.0 );

	Vector3Double result;
	double invMagnitude = ( 1.0 / magnitude );
	result.x = x * invMagnitude;
	result.y = y * invMagnitude;
	result.z = z * invMagnitude;
	return result;
}

Vector3Double Vector3Double::Cross( const Vector3Double& B )
{
	/*
	x = Ay * Bz - By * Az
	y = Az * Bx - Bz * Ax
	z = Ax * By - Bx * Ay
	*/

	double retX = y * B.z - B.y * z;
	double retY = z * B.x - B.z * x;
	double retZ = x * B.y - B.x * y;
	return Vector3Double( retX, retY, retZ );
}

double Vector3Double::Dot( const Vector3Double& B )
{
	return  (x * B.x) + (y * B.y) + (z * B.z);
}

const Vector3Double Vector3Double::operator-() const
{
	Vector3Double vec;
	vec.x = -x;
	vec.y = -y;
	vec.z = -z;
	return vec;
}

Vector3Double Vector3Double::ZERO = Vector3Double( 0.0, 0.0, 0.0 );
Vector3Double Vector3Double::ONE = Vector3Double( 1.0, 1.0, 1.0 );

Vector3Double& Vector3Double::operator+=( const Vector3Double other )
{
	this->x = x + other.x;
	this->y = y + other.y;
	this->z = z + other.z;
	return *this;
}

const Vector3Double Vector3Double::operator+( const Vector3Double other ) const
{
	Vector3Double result = *this;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	return result;
}

const Vector3Double Vector3Double::operator-( const Vector3Double other ) const
{
	Vector3Double result = *this;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	return result;
}

Vector3Double& Vector3Double::operator=( const Vector3Double &rhs )
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	return *this;
}

Vector3Double Vector3Double::operator-=( const Vector3Double other )
{
	this->x = x - other.x;
	this->y = y - other.y;
	this->z = z - other.z;
	return *this;
}

Vector3Double Vector3Double::operator*( double scalar )
{
	Vector3Double result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

const Vector3Double Vector3Double::operator*( const double& scalar ) const
{
	Vector3Double result;
	result.x = x * scalar;
	result.y = y * scalar;
	result.z = z * scalar;
	return result;
}

Vector3Double operator*( double scalar, const Vector3Double& vec )
{
	Vector3Double result;
	result.x = vec.x * scalar;
	result.y = vec.y * scalar;
	result.z = vec.z * scalar;
	return result;
}

const Vector3Double Vector3Double::operator/( const double& scalar ) const
{
	Vector3Double result;
	result.x = x / scalar;
	result.y = y / scalar;
	result.z = z / scalar;
	return result;
}

bool operator==( const Vector3Double& lhs, const Vector3Double& rhs )
{
	if ( lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z )
		return true;
	return false;
}