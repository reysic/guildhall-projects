#pragma once
#include "Vector3.hpp"

class CatmullRomSpline
{
public:
	CatmullRomSpline()
		: m_tensionFactor(1.f)
	{}
	CatmullRomSpline(Vector3 P0, Vector3 P1, Vector3 P2, Vector3 P3, float tensionFactor = 5.f)
		: P0x(P0.x)
		, P0y(P0.y)
		, P0z(P0.z)

		, P1x(P1.x)
		, P1y(P1.y)
		, P1z(P1.z)

		, P2x(P2.x)
		, P2y(P2.y)
		, P2z(P2.z)

		, P3x(P3.x)
		, P3y(P3.y)
		, P3z(P3.z)

		, m_tensionFactor(tensionFactor)
	{
		m_T0x = Ti(m_tensionFactor, P2x, P0x);
		m_T0y = Ti(m_tensionFactor, P2y, P0y);
		m_T0z = Ti(m_tensionFactor, P2z, P0z);

		m_T1x = Ti(m_tensionFactor, P3x, P1x);
		m_T1y = Ti(m_tensionFactor, P3y, P1y);
		m_T1z = Ti(m_tensionFactor, P3z, P1z);
	}

	Vector3 GetPositionOnSpline(float t) const {
		float b1 = 1.f - 3.f*t*t + 2.f*t*t*t;
		float b2 = 3.f*t*t + -2.f*t*t*t;
		float b3 = t - 2.f*t*t + t*t*t;
		float b4 = -t*t + t*t*t;

		float db1 = -6.f*t + 6.f*t*t;
		float db2 = 6.f*t - 6.f*t*t;
		float db3 = 1.f - 4.f*t + 3.f*t*t;
		float db4 = -2.f*t + 3.f*t*t;

		Vector3 result;
		result.x = P1x * b1 + P2x * b2 + m_T0x * b3 + m_T1x * b4;
		result.y = P1y * b1 + P2y * b2 + m_T0y * b3 + m_T1y * b4;
		result.z = P1z * b1 + P2z * b2 + m_T0z * b3 + m_T1z * b4;
		return result;
	}

	void FillInPositionMatrixForPositionOnSpline(float t, float* m) const {
		Vector3 position = GetPositionOnSpline(t);

		m[12] = position.x;
		m[13] = position.y;
		m[14] = position.z;
	}

	float GetNormOfTangent(float t) const {
		float db1 = -6.f*t + 6.f*t*t;
		float db2 = 6.f*t - 6.f*t*t;
		float db3 = 1.f - 4.f*t + 3.f*t*t;
		float db4 = -2.f*t + 3.f*t*t;

		Vector3 result;
		result.x = P1x * db1 + P2x * db2 + m_T0x * db3 + m_T1x * db4;
		result.y = P1y * db1 + P2y * db2 + m_T0y * db3 + m_T1y * db4;
		result.z = P1z * db1 + P2z * db2 + m_T0z * db3 + m_T1z * db4;
		return result.GetMagnitude();
	}

	Vector3 GetTangent(float t) const {
		float db1 = -6.f*t + 6.f*t*t;
		float db2 = 6.f*t - 6.f*t*t;
		float db3 = 1.f - 4.f*t + 3.f*t*t;
		float db4 = -2.f*t + 3.f*t*t;

		Vector3 result;
		result.x = P1x * db1 + P2x * db2 + m_T0x * db3 + m_T1x * db4;
		result.y = P1y * db1 + P2y * db2 + m_T0y * db3 + m_T1y * db4;
		result.z = P1z * db1 + P2z * db2 + m_T0z * db3 + m_T1z * db4;
		return result;
	}

private:
	float P0x;
	float P0y;
	float P0z;

	float P1x;
	float P1y;
	float P1z;

	float P2x;
	float P2y;
	float P2z;

	float P3x;
	float P3y;
	float P3z;

	float m_T0x;
	float m_T0y;
	float m_T0z;

	float m_T1x;
	float m_T1y;
	float m_T1z;

	float m_tensionFactor;

	float b2(float t) const
	{
		float t2 = t * t;
		float t3 = t2 * t;
		return 3 * t2 - 2 * t3;
	}

	float b3(float t) const
	{
		float t2 = t * t;
		float t3 = t2 * t;
		return t - 2 * t2 + t3;
	}

	float b4(float t) const
	{
		float t2 = t * t;
		float t3 = t2 * t;
		return -t2 + t3;
	}

	float db1(float t) const
	{
		float t2 = t * t;
		return -6 * t + 6 * t2;
	}

	float db2(float t) const
	{
		float t2 = t * t;
		return 6 * t - 6 * t2;
	}

	float db3(float t) const
	{
		float t2 = t * t;
		return 3 * t2 - 4 * t + 1;
	}

	float db4(float t) const
	{
		float t2 = t * t;
		return 3 * t2 - 2 * t;
	}

	float Ti(float s, float pNext, float pPrevious) const
	{
		return s * (pNext - pPrevious);
	}
};