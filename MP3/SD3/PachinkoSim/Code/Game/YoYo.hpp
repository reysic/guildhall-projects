#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector3Double.hpp"

class YoYo {
public:
	YoYo();
	YoYo( Vector3Double posOfMass, Vector3Double posOfString, float length, float stiffness, float damping, float mass );

	void Update( float deltaSeconds );
	void Render() const;

	void AddImpulse();

	void IncreaseMass();

	void DecreaseMass();


	Vector3Double	m_positionOfMass;
	Vector3Double m_positionOfString;
	float m_unstretchedLength;
	float m_stiffness;
	float m_dampingCoefficient;
	float m_mass;
	Vector3Double m_prevPosOfMass;
	Vector3Double m_velocity;
	Vector3Double m_prevVelocity;


private:
};