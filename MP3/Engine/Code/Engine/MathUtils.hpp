#pragma once
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Vector3Double.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "math.h"

const float pi = 3.141592653589793f;

float DegToRad(float deg);

float RadToDeg(float rad);

float SinDegrees(float deg);

float CosDegrees(float deg);

bool DoDiscsOverlap(const Vector2& center1, float radius1, const Vector2& center2, float radius2);

bool IsPointInDisc(const Vector2& center1, float radius1, const Vector2& center2);
float CalcSquaredDistanceBetweenPoints(const Vector2& pos1, const Vector2& pos2);

float CalcDistanceBetweenPoints(const Vector2& pos1, const Vector2& pos2);
//Vector3 CalcPointAtDistance(const Vector3& pos1, const EulerAngles& angle, const float& dist);

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);

int ClampInt(int inValue, int low, int high);

float ClampFloat(float inValue, float low, float high);

float ClampFloatCircular(float inValue, float low, float high);

float CalcShortestAngularDisplacement(float fromDegrees, float toDegrees);

Vector2 CalcDirToPoint(const Vector2& from, const Vector2& to);

float DotProduct(const Vector2& a, const Vector2& b);
float DotProduct(const Vector3& a, const Vector3& b);
double DotProduct(const Vector3Double& a, const Vector3Double& b);

float DotProduct(const Vector4& a, const Vector4& b);
Vector3 CrossProduct(const Vector3& a, const Vector3& b);

float SmoothStep(float inputZeroToOne);

float ClampUpToZero(float value);

unsigned char FloatToUChar(float input);

bool IsColinear(const Vector3& a, const Vector3& b);