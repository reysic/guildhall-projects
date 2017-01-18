#include "Node.hpp"
#include "Engine/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/Rgba.hpp"


const float Z_FLOOR = 1.1f;

Node::Node()
{

}

Node::Node( Vector3Double position, double mass, Vector2 texCoords, IntVector2 coordinates)
{
    m_velocity = Vector3Double::ZERO;
    m_prevVelocity = Vector3Double::ZERO;
    m_mass = mass;
    m_position = position;
    m_prevPos = position;
    m_isFixed = false;
	m_texCoords = texCoords;
	m_coordinates = coordinates;
	m_scoreValue = 0;
}

void Node::Update( float deltaSeconds )
{

    if (m_isFixed)
    {
        m_position = m_prevPos;
        m_velocity = Vector3Double::ZERO;
    }
    else
    {
        Vector3Double dVelocity = Vector3Double::ZERO;
        Vector3Double dPosition = m_prevVelocity;

        for (int i = 0; ( unsigned int ) i < m_ASprings.size(); ++i)
        {
            // TODO
            // mutate m_velocity

            Vector3Double envDamping = -DAMPING_DEFAULT * m_prevVelocity;
            Vector3Double velocity;


            Vector3Double myPos = m_prevPos;
            Vector3Double otherPos = m_ASprings[i]->m_nodeA->m_position;
            Vector3Double e = (otherPos - myPos);
            float stretch = ( float ) e.GetMagnitude();
            e.SetNormalized();

            Vector3Double forceK = m_ASprings[i]->m_stiffness * (stretch - m_ASprings[i]->m_unstretchedLength) * e;
            Vector3Double forceC = m_ASprings[i]->m_dampingCoefficient * (DotProduct( -1.f * m_prevVelocity, e )) * e;
            //Vector3 forceG = Vector3( 0.f, 0.f, -m_mass * 9.8f );

            Vector3Double force = forceK + forceC + envDamping;
// 			Vector3Double force = forceK + forceC; // //+ envDamping;
            dVelocity += ((1.f / m_mass) * force);

        }

        for (int i = 0; ( unsigned int ) i < m_BSprings.size(); ++i)
        {
            // TODO
            // mutate m_velocity

            Vector3Double envDamping = -DAMPING_DEFAULT * m_prevVelocity;
            Vector3Double velocity;


            Vector3Double myPos = m_prevPos;
            Vector3Double otherPos = m_BSprings[i]->m_nodeB->m_position;
            Vector3Double e = ( otherPos - myPos);
            float stretch = ( float ) e.GetMagnitude();
            e.SetNormalized();

            Vector3Double forceK = m_BSprings[i]->m_stiffness * (stretch - m_BSprings[i]->m_unstretchedLength) * e;
            Vector3Double forceC = m_BSprings[i]->m_dampingCoefficient * (DotProduct( -1.f * m_prevVelocity, e )) * e;
            //Vector3 forceG = Vector3( 0.f, 0.f, -m_mass * 9.8f );

            Vector3Double force = forceK + forceC + envDamping;
            dVelocity += ((1.f / m_mass) * force);

        }


		Vector3Double forceG = (GRAVITY_DIRECTION * m_mass) * 9.8f;

		for (int i = 0; i < m_impulses.size(); ++i)
		{
			forceG += m_impulses[i].force;
		}
		auto iter = m_impulses.begin();
		for (iter; iter != m_impulses.end(); )
		{
			(*iter).time-= deltaSeconds;
			if ((*iter).time <= 0)
				iter = m_impulses.erase( iter );
			else
				++iter;
		}

        dVelocity += ((1.f / m_mass) * forceG);

        // Euler integration
        //m_position = m_prevPos + deltaSeconds * dPosition;
        //m_velocity = m_prevVelocity + (deltaSeconds * dVelocity);

        // Velocity Verlet
        m_prevVelocity = m_velocity;
        m_velocity = m_velocity + ( deltaSeconds * dVelocity );
        m_position = m_position + ( m_prevVelocity + m_velocity ) * 0.5f * deltaSeconds;

		if (m_position.z < Z_FLOOR)
		{
			m_position.z = Z_FLOOR;
			m_velocity.z = 0.f;
		}


        //m_prevVelocity = m_velocity;
        m_prevPos = m_position;
    }
}

void Node::Render() const
{
    Vector3 tmpPos = Vector3( m_position.x, m_position.y, m_position.z );
    g_theRenderer->AddDebugSphere( tmpPos, .2f, Rgba::BLACK, 0.f, RenderCommand::XRAY );
}

void Node::AddImpulse()
{

}

void Node::IncreaseMass()
{

}

void Node::DecreaseMass()
{

}

void Node::AddSpring(Node* other, Spring::SPRING_TYPE type, float unstretchedLength, float stiffness, float dampingCoefficient)
{
    Spring* newSpring = new Spring(this, other, type, unstretchedLength, stiffness, dampingCoefficient);
    m_BSprings.push_back(newSpring);
    other->m_ASprings.push_back(newSpring);
}

void Node::AddSpring( Node* other, Spring* springPtr )
{
    m_BSprings.push_back( springPtr );
    other->m_ASprings.push_back( springPtr );
}

