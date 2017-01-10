#include "Game/YoYo.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Rgba.hpp"

//STRUCTORS
YoYo::YoYo() {}
YoYo::YoYo(Vector3Double posOfMass, Vector3Double posOfString, float length, float stiffness, float damping, float mass)
	: m_positionOfMass(posOfMass)
	, m_positionOfString(posOfString)
	, m_unstretchedLength(length)
	, m_stiffness(stiffness)
	, m_dampingCoefficient(damping)
	, m_mass(mass)
	, m_prevPosOfMass(posOfMass)
	, m_velocity(Vector3Double(0.f, 0.f, 0.f))
	, m_prevVelocity(Vector3Double(0.f, 0.f, 0.f))
{ }

//UPDATE RENDER
void YoYo::Update(float deltaSeconds) {
	static float age = 0.f;
	Vector3Double envDamping = -10.f * m_prevVelocity;
	Vector3Double velocity;
	Vector3Double dPosition = m_prevVelocity;


	Vector3Double e = (m_positionOfString - m_prevPosOfMass);
	float stretch = ( float ) e.GetMagnitude();
	e.SetNormalized();

	Vector3Double forceK = m_stiffness * (stretch - m_unstretchedLength) * e;
	Vector3Double forceC = m_dampingCoefficient * (DotProduct(-1.f * m_prevVelocity, e)) * e;
	Vector3Double forceG = Vector3Double(0.f, 0.f, -m_mass * 9.8f);

	Vector3Double force = forceK + forceC + forceG; //+ envDamping;
	Vector3Double dVelocity = ((1.f / m_mass) * force);

	m_positionOfMass = m_prevPosOfMass + deltaSeconds * dPosition;
	m_velocity = m_prevVelocity + deltaSeconds * dVelocity;

	m_prevVelocity = m_velocity;
	m_prevPosOfMass = m_positionOfMass;

// 	age += deltaSeconds;
// 	m_positionOfString = Vector3(5.f * cos(age), 5.f * sin(age), 0.f);
}
void YoYo::Render()  const{
	Vector3 tmpStringPos = Vector3( ( float ) m_positionOfString.x, ( float ) m_positionOfString.y, ( float ) m_positionOfString.z );
	Vector3 tmpMassPos = Vector3( ( float ) m_positionOfMass.x, ( float ) m_positionOfMass.y, ( float ) m_positionOfMass.z );

	g_theRenderer->AddDebugLine( tmpStringPos, tmpMassPos, Rgba::WHITE, 0.f, RenderCommand::XRAY );
	g_theRenderer->AddDebugSphere( tmpMassPos, 2.f, Rgba::WHITE, 0.f, RenderCommand::XRAY );
}

void YoYo::AddImpulse() {
	m_prevVelocity += Vector3Double(0.f, 0.f, 20.f);
}
void YoYo::IncreaseMass() {
	m_mass += 0.1f;
}
void YoYo::DecreaseMass() {
	m_mass -= 0.1f;
	if (m_mass <= 0.f)
		m_mass = 0.1f;
}