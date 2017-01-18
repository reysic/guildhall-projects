#pragma once
#include "Vector3.hpp"
#include <math.h>

class Quaternion
{
public:
	Quaternion() : m_q0(0.f), m_q1(0.f), m_q2(0.f), m_q3(0.f)
	{

	}
	Quaternion(float angle, Vector3 axis)
	{
		//angle should be in radians
		m_q0 = cos(angle) / 2.f;
		axis.SetNormalized();

		m_q1 = sin(angle / 2.f) * axis.x;
		m_q2 = sin(angle / 2.f) * axis.y;
		m_q3 = sin(angle / 2.f) * axis.z;
	}
	Quaternion(float q0, float q1, float q2, float q3) : m_q0(q0), m_q1(q1), m_q2(q2), m_q3(q3)
	{

	}

	friend Quaternion operator*(const Quaternion& q, float scalar)
	{
		return Quaternion(q.m_q0 * scalar, q.m_q1 * scalar, q.m_q2 * scalar, q.m_q3 * scalar);
	}

	friend Quaternion operator*( float scalar, const Quaternion& q)
	{
		return Quaternion(q.m_q0 * scalar, q.m_q1 * scalar, q.m_q2 * scalar, q.m_q3 * scalar);
	}

	friend Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
	{
		return Quaternion(q1.m_q0 + q2.m_q0, q1.m_q1 + q2.m_q1, q1.m_q2 + q2.m_q2, q1.m_q3 + q2.m_q3);
	}

	float DotProduct(const Quaternion& other) const {
		return (m_q0 * other.m_q0 + m_q1 * other.m_q1 + m_q2 * other.m_q2 + m_q3 * other.m_q3);
	}
	void ToRotationMatrix(float* m)
	{
		float qx2 = m_q1 * m_q1;
		float qy2 = m_q2 * m_q2;
		float qz2 = m_q3 * m_q3;

		float qw = m_q0;
		float qx = m_q1;
		float qy = m_q2;
		float qz = m_q3;

		m[0] = 1 - 2 * qx2 - 2 * qy2;
		m[1] = 2 * qx*qy + 2 * qz*qw;
		m[2] = 2 * qx*qz - 2 * qy*qw;
		m[3] = 0.f;

		m[4] = 2 - qx*qy - 2 * qz*qw;
		m[5] = 1-2 * qx2 - 2 * qz2;
		m[6] = 2 * qy*qz + 2 * qx*qw;
		m[7] = 0.f;


		m[8] = 2 - qx*qz - 2 * qy*qw;
		m[9] =  2 * qy*qz - 2 * qx*qw;
		m[10] = 1 - 2 * qx2 - 2 * qy2;
		m[11] = 0.f;

		m[12] = 0.f;
		m[13] = 0.f;
		m[14] = 0.f;
		m[15] = 0.f;
	}
private:
	float m_q0;
	float m_q1;
	float m_q2;
	float m_q3;
};