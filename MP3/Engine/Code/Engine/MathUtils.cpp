#include "Engine/MathUtils.hpp"

float DegToRad(float deg)
{
	return (deg * (pi/180.f));
}

float RadToDeg(float rad)
{
	return (rad * (180.f / pi));
}

float SinDegrees(float deg)
{
	float radians = DegToRad(deg);
	return (float)sin(radians);
}

float CosDegrees(float deg)
{
	float radians = DegToRad(deg);
	return (float)cos(radians);
}


float CalcSquaredDistanceBetweenPoints(const Vector2& pos1, const Vector2& pos2)
{
	float xDist = pos2.x - pos1.x;
	float yDist = pos2.y - pos1.y;
	return(xDist*xDist) + (yDist*yDist);
}

float CalcDistanceBetweenPoints(const Vector2& pos1, const Vector2& pos2)
{
	float xDist = pos2.x - pos1.x;
	float yDist = pos2.y - pos1.y;

	return (float)sqrt((xDist*xDist) + (yDist*yDist));
}

// Vector3 CalcPointAtDistance(const Vector3& pos1, const EulerAngles& angle, const float& dist)
// {
// 	//return dist * angle + pos;
// }

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd)
{
	float inputDiff = inValue - inStart;
	float inputRange = inEnd - inStart;
	float outputRange = outEnd - outStart;

	float result = outStart + ((inputDiff / inputRange) * outputRange);
	return result;
}

int ClampInt(int inValue, int low, int high)
{
	if (inValue < low)
		return low;
	else if (inValue > high)
		return high;
	else
		return inValue;
}

float ClampFloat(float inValue, float low, float high)
{
	if (inValue < low)
		return low;
	else if (inValue > high)
		return high;
	else
		return inValue;
}

float ClampFloatCircular(float inValue, float low, float high)
{
	float result = inValue;
	float delta = high - low;

	while (result < low)
	{
		result += delta;
	}

	while (result > high)
	{
		result -= delta;
	}

	return result;
}

float CalcShortestAngularDisplacement(float fromDegrees, float toDegrees)
{
	float angularDisplacement = toDegrees - fromDegrees;
	while (angularDisplacement > 180.f)
		angularDisplacement -= 360.f;
	while (angularDisplacement < -180.f)
		angularDisplacement += 360.f;
	return angularDisplacement;
}

Vector2 CalcDirToPoint(const Vector2& from, const Vector2& to)
{
	Vector2 result = to - from;
	result.SetNormalized();
	return result;
}

float DotProduct(const Vector2& a, const Vector2& b)
{
	float result;
	result = (a.x * b.x + a.y * b.y);
	return result;
}

float DotProduct(const Vector3& a, const Vector3& b)
{
	float result;
	result = (a.x * b.x + a.y * b.y + a.z * b.z);
	return result;
}

float DotProduct(const Vector4& a, const Vector4& b)
{
	float result;
	result = (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
	return result;
}

double DotProduct(const Vector3Double& a, const Vector3Double& b)
{
	double result;
	result = (a.x * b.x + a.y * b.y + a.z * b.z);
	return result;
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	Vector3 result;
	result.x = (a.y * b.z) - (a.z * b.y);
	result.y = (a.z * b.x) - (a.x * b.z);
	result.z = (a.x * b.y) - (a.y * b.x);
	return result;
}

float SmoothStep(float inputZeroToOne)
{
	float inputSquared = (inputZeroToOne * inputZeroToOne);
	return (3.f * inputSquared) - (2.f * inputSquared * inputZeroToOne);
}

float ClampUpToZero(float value)
{
	float result = value;
	if (result < 0.f)
	{
		result = 0.f;
	}
	return result;
}

unsigned char FloatToUChar(float input)
{
	float result = input;
	result = RangeMap((float)result, 0.f, 1.f, 0.f, 255.f);
	return (unsigned char)result;
}

bool IsColinear(const Vector3& a, const Vector3& b)
{
	float dot = DotProduct(a, b);

	float mag = a.GetMagnitude() * b.GetMagnitude();

	if (dot == mag)
	{
		return true;
	}
	return false;
}

bool DoDiscsOverlap(const Vector2& center1, float radius1, const Vector2& center2, float radius2)
{
	float distSquared = CalcSquaredDistanceBetweenPoints(center1, center2);

	float radii = radius1 + radius2;
	return(distSquared < (radii*radii));
}

bool IsPointInDisc(const Vector2& center1, float radius1, const Vector2& center2)
{
	float distSquared = CalcSquaredDistanceBetweenPoints(center1, center2);
	return (distSquared < (radius1 * radius1));
}