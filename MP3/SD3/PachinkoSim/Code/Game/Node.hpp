#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector3Double.hpp"
#include "Game/Spring.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/IntVector2.hpp"
#include <vector>
#pragma once


static const Vector3 GRAVITY_DIRECTION = Vector3( 0.0f, -.3f, -.7f ).GetNormalized();


class Node {
public:

	struct Impulse {
	public:
		Vector3Double force;
		float time;

		Impulse( const Vector3Double& inputForce, const float& inputTime ) {
			force = inputForce;
			time = inputTime;
		}

	private:
	};

	Node();
	Node( Vector3Double position, double mass, Vector2 texCoords = Vector2(1.f,1.f), IntVector2 coordinates = IntVector2(0,0) );

	void Update( float deltaSeconds );
	void Render() const;

	void AddImpulse();

	void IncreaseMass();

	void DecreaseMass();



	void AddSpring(Node* other, Spring::SPRING_TYPE type, float unstretchedLength, float stiffness, float dampingCoefficient);
	void AddSpring( Node* other, Spring* springPtr );
	Vector3Double	m_position;
	double m_mass;
	Vector3Double m_prevPos;
	Vector3Double m_velocity;
	Vector3Double m_prevVelocity;
	bool m_isFixed;

	Vector2 m_texCoords;
	IntVector2 m_coordinates;
	int m_scoreValue;

	std::vector<Impulse> m_impulses;

	std::vector<Spring*> m_springs;
	std::vector<Spring*> m_ASprings;
	std::vector<Spring*> m_BSprings;

private:
};