#pragma once

#include "Game/BallState.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/ClothCollisionHelper.hpp"

// static const Vector3 GRAVITY_DIRECTION_BALL = Vector3( 0.0f, -.3f, -.7f ).GetNormalized();


class Node;
//-------------------------------------------------------------------------------------------------
class Ball
{
public:
	float m_mass;
	float m_radius;
	BallState m_state;
	BallState m_prevState;
	bool m_collided;
public:
	Ball(float mass, float radius, BallState const &state);
	~Ball();
	Vector3 GetPosition() const;
	void Update(float deltaSeconds);
	void BackToPrevious();
	void CollideWithNode(const Node* node, float restitution, float deltaSeconds);
};


//-------------------------------------------------------------------------------------------------
inline float IsColliding(Ball const &ball1, Ball const &ball2)
{
	Vector3 x0 = ball2.m_prevState.m_position - ball1.m_prevState.m_position;
	Vector3 e = ((ball2.m_state.m_position - ball1.m_state.m_position) - (x0));
	float R = ball1.m_radius + ball2.m_radius;
	float Dover4 = (DotProduct(x0, e) * DotProduct(x0, e)) - DotProduct(e, e) * (DotProduct(x0, x0) - R*R);
	if (Dover4 > 0)
	{
		float root1 = (-DotProduct(x0, e) - sqrt(Dover4)) / (DotProduct(e, e));
		float root2 = (-DotProduct(x0, e) + sqrt(Dover4)) / (DotProduct(e, e));
		if (root1 <= 1 && root2 >= 0)
		{
			root1 = root1 > 0.f ? root1 : 0.f;
			root2 = root2 < 1.f ? root2 : 1.f;

			return root1; //TRUE
		}
	}
	return -1.f; //FALSE
}

inline float IsColliding(Ball const &ball1, Vector3Double const &pos, float radius)
{
	Vector3Double vecPrevPos = Vector3Double(ball1.m_prevState.m_position.x, ball1.m_prevState.m_position.y, ball1.m_prevState.m_position.z);
	Vector3Double vecPos = Vector3Double(ball1.m_state.m_position.x, ball1.m_state.m_position.y, ball1.m_state.m_position.z);

	Vector3Double x0 = pos - vecPrevPos;
	Vector3Double e = ((pos - vecPos) - (x0));
	float R = ball1.m_radius + radius;
	double d1 = DotProduct(x0, e);
	double d2 = DotProduct(e, e);
	double d3 = DotProduct(x0, x0);

	float Dover4 = ( float ) ( (d1 * d1) - d2 * (d3 - R*R) );
	if (Dover4 > 0)
	{
		float root1 = ( float ) ( (-DotProduct(x0, e) - sqrt(Dover4)) / (DotProduct(e, e)) );
		float root2 = ( float ) ( (-DotProduct(x0, e) + sqrt(Dover4)) / (DotProduct(e, e)) );
		if (root1 <= 1 && root2 >= 0)
		{
			root1 = root1 > 0.f ? root1 : 0.f;
			root2 = root2 < 1.f ? root2 : 1.f;

			return root1; //TRUE
		}
	}
	return -1.f; //FALSE
}


//-------------------------------------------------------------------------------------------------
inline void CollideAndBounce(Ball *ball1, Ball *ball2, float restitution, float deltaSeconds)
{
	float enterTime = IsColliding(*ball1, *ball2);
	if (enterTime >= 0.00001f)
	{
		ball1->m_collided = true;
		ball2->m_collided = true;

		//Back up
		ball1->BackToPrevious();
		ball2->BackToPrevious();

		//Move to right before collide
		float timeUntilCollide = enterTime * deltaSeconds;
		ball1->Update(timeUntilCollide);
		ball2->Update(timeUntilCollide);

		//BOUNCE TIME
		Vector3 lineOfImpact = ball2->m_state.m_position - ball1->m_state.m_position;
		lineOfImpact.SetNormalized();

		float Va = DotProduct(ball1->m_state.m_velocity, lineOfImpact);
		float Vb = DotProduct(ball2->m_state.m_velocity, lineOfImpact);

		float oneOverMasses = 1.f / (ball1->m_mass + ball2->m_mass);
		float massA = ball1->m_mass;
		float massB = ball2->m_mass;
		float VaPrime = oneOverMasses * (massA * Va + massB * Vb - massB * restitution * (Va - Vb));
		float VbPrime = oneOverMasses * (massA * Va + massB * Vb + massA * restitution * (Va - Vb));

		//BallState velocity reset	     NEW PARALLEL                 NEW PERPENDICULAR = (old perpindiclar)                       
		ball1->m_state.m_velocity = (VaPrime * lineOfImpact) + (ball1->m_state.m_velocity - Va * lineOfImpact);
		ball2->m_state.m_velocity = (VbPrime * lineOfImpact) + (ball2->m_state.m_velocity - Vb * lineOfImpact);

		float restOfTime = deltaSeconds - timeUntilCollide;
		ball1->Update(restOfTime);
		ball2->Update(restOfTime);
		//ASSERT_OR_DIE( IsColliding( *ball1, *ball2 ) < 0.f, "You are colliding, and you shouldn't be." )
	}
}