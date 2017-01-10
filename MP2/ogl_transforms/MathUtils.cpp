#include "MathUtils.hpp"

float DotProduct(const Vector3& a, const Vector3& b)
{
	float result;
	result = (a.x * b.x + a.y * b.y + a.z * b.z);
	return result;
}

Vector3 CrossProduct(const Vector3& v1, const Vector3& v2) {
	//a X b = v1x v2z - a3b2, a3b1 - a1b3, a1b2 - a2b1
	//a X b = v1x v2z - v1z v2y, v1z v2x - v1x v2z, v1x v2y - v2y v1x
	return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}