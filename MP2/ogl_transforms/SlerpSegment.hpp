#pragma once

#include "Quaternion.hpp"

class SlerpSegment {
public:
	SlerpSegment() {}
	SlerpSegment(Quaternion q_a, Quaternion q_b)
		: m_q_A(q_a)
		, m_q_B(q_b)
	{
		float cosTheta = m_q_A.DotProduct(m_q_B);

		if (cosTheta < 0.f) {
			m_q_B = -1.f * m_q_B;
			cosTheta = -1.f * cosTheta;
		}

		m_theta = acos(cosTheta);
	}

	//SlerpSegment foo;
	//Quaternion q = foo(.5);
	Quaternion operator()(float t) const {
		Quaternion intermediateQuaternion = (m_q_A * sin((1.f - t) * m_theta) + m_q_B * sin(t * m_theta));
		intermediateQuaternion = intermediateQuaternion * (1 / sin(m_theta));
		return intermediateQuaternion;
	}
private:
	Quaternion m_q_A;
	Quaternion m_q_B;

	float m_theta;
};