#include "Game/Ball.hpp"

#include "Engine/Core/Time.hpp"
#include "Game/Node.hpp"

const float Z_FLOOR = 1.4f;
//-------------------------------------------------------------------------------------------------

Ball::Ball(float mass, float radius, BallState const &state)
	: m_mass(mass)
	, m_radius(radius)
	, m_state(state)
	, m_prevState(state)
	, m_collided(false)
{

}


//-------------------------------------------------------------------------------------------------
Ball::~Ball()
{

}


//-------------------------------------------------------------------------------------------------
Vector3 Ball::GetPosition() const
{
	return m_state.m_position;
}


//-------------------------------------------------------------------------------------------------
void Ball::Update(float deltaSeconds)
{
	m_prevState = m_state;
	m_state.m_position += m_prevState.m_velocity * deltaSeconds;
	m_state.m_velocity += Vector3::ZERO; // ???? how is this working lol

	m_state.m_velocity += GRAVITY_DIRECTION * 9.8f * deltaSeconds;

	if (m_state.m_position.z < Z_FLOOR)
	{
		m_state.m_position.z = m_prevState.m_position.z;
		m_state.m_velocity.z = 0.f;
	}

}


//-------------------------------------------------------------------------------------------------
void Ball::BackToPrevious()
{
	m_state = m_prevState;
}

void Ball::CollideWithNode(const Node* node, float restitution, float deltaSeconds)
{
	float enterTime = IsColliding(*this, node->m_position, .2f);
	if (enterTime >= 0.00001f)
	{
		m_collided = true;
		Vector3Double nodePos = node->m_position;
		Vector3Double thisPos = Vector3Double(m_state.m_position.x, m_state.m_position.y, m_state.m_position.z);
		Vector3Double thisVel = Vector3Double(m_state.m_velocity.x, m_state.m_velocity.y, m_state.m_velocity.z);

		//Back up
		BackToPrevious();
		

		//Move to right before collide
		float timeUntilCollide = enterTime * deltaSeconds;
		Update(timeUntilCollide);
		

		//BOUNCE TIME
		Vector3Double lineOfImpact = nodePos - thisPos;
		lineOfImpact.SetNormalized();

		float Va = ( float ) DotProduct(thisVel, lineOfImpact);
		float Vb = 0.f;//DotProduct(thisVel, lineOfImpact);

		float oneOverMasses = 1.f / (m_mass + 9999.f);
		float massA = m_mass;
		float massB = 9999.f;
		float VaPrime = oneOverMasses * (massA * Va + massB * Vb - massB * restitution * (Va - Vb));
		float VbPrime = oneOverMasses * (massA * Va + massB * Vb + massA * restitution * (Va - Vb));
		VbPrime;

		//BallState velocity reset	     NEW PARALLEL                 NEW PERPENDICULAR = (old perpindiclar)                       
		thisVel = (VaPrime * lineOfImpact) + (thisVel - Va * lineOfImpact);
		m_state.m_velocity = Vector3( ( float ) thisVel.x, ( float ) thisVel.y, ( float ) thisVel.z);
		//ball2->m_state.m_velocity = (VbPrime * lineOfImpact) + (ball2->m_state.m_velocity - Vb * lineOfImpact);

		float restOfTime = deltaSeconds - timeUntilCollide;
		Update(restOfTime);
		//ball2->Update(restOfTime);
		//ASSERT_OR_DIE( IsColliding( *ball1, *ball2 ) < 0.f, "You are colliding, and you shouldn't be." )
	}
}
