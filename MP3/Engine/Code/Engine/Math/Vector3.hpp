#pragma once
#include <math.h>
class Vector3Double;
class Vector3
{
public:
	Vector3();
	Vector3(float initialX, float initialY, float initialZ);
	Vector3(const Vector3& v);
	void SetXYZ(float newX, float newY, float newZ);
	float GetMagnitude() const;
	void SetNormalized();
	Vector3 GetNormalized() const;

	const Vector3 Vector3::operator+(const Vector3 other) const;
	const Vector3 Vector3::operator-(const Vector3 other) const;
	Vector3& Vector3::operator=(const Vector3 &rhs);
	Vector3& Vector3::operator=(const Vector3Double &rhs);
	Vector3& Vector3::operator+=(const Vector3 other);
	Vector3 Vector3::operator-=(const Vector3 other);
	Vector3 operator* (float scalar);
	const Vector3 operator*(const float& scalar) const;
	friend Vector3 operator*(float scalar, const Vector3& vec);
	const Vector3 operator/(const float& scalar) const;
	friend bool operator==(const Vector3& lhs, const Vector3& rhs);
	friend bool operator!=(const Vector3& lhs, const Vector3& rhs);
	const Vector3 operator-() const;

	float x;
	float y;
	float z;

	static Vector3 ZERO;
	static Vector3 ONE;
};

