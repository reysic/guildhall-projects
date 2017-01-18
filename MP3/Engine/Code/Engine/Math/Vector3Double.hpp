#pragma once
#include <math.h>
#include "Engine/Math/Vector3.hpp"
class Vector3Double
{
public:
	Vector3Double();
	Vector3Double( double initialX, double initialY, double initialZ );
	Vector3Double( const Vector3Double& v );
	Vector3Double( const Vector3& v );
	void SetXYZ( const double& newX, const double& newY, const double& newZ );
	double GetMagnitude() const;
	void SetNormalized();
	double DistanceToSquared( const Vector3Double& otherVec );
	float DistanceToSquared( const Vector3& otherVec );
	Vector3Double GetNormalized() const;
	Vector3Double Cross( const Vector3Double& B );
	double Dot( const Vector3Double& B );

	const Vector3Double Vector3Double::operator+( const Vector3Double other ) const;
	const Vector3Double Vector3Double::operator-( const Vector3Double other ) const;
	Vector3Double& Vector3Double::operator=( const Vector3Double &rhs );
	Vector3Double& Vector3Double::operator+=( const Vector3Double other );
	Vector3Double Vector3Double::operator-=( const Vector3Double other );
	Vector3Double operator* ( double scalar );
	const Vector3Double operator*( const double& scalar ) const;
	friend Vector3Double operator*( double scalar, const Vector3Double& vec );
	const Vector3Double operator/( const double& scalar ) const;
	friend bool operator==( const Vector3Double& lhs, const Vector3Double& rhs );
	friend bool operator!=( const Vector3Double& lhs, const Vector3Double& rhs );
	const Vector3Double operator-() const;

	double x;
	double y;
	double z;

	static Vector3Double ZERO;
	static Vector3Double ONE;
};

