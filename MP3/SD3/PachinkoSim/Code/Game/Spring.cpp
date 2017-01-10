#include "Spring.hpp"
#include "Game/Node.hpp"
#include "Engine/Renderer/Renderer.hpp"

Spring::Spring()
{
	m_nodeA = nullptr;
	m_nodeB = nullptr;
	m_type = SPRING_STRUCTURAL;
	m_unstretchedLength = UNSTRETCHED_LENGTH_DEFAULT;
	m_stiffness = STRUCTURAL_STIFFNESS;
	m_dampingCoefficient = DAMPING_DEFAULT;
}

Spring::Spring(Node* nodeA /*= nullptr*/, Node* nodeB /*= nullptr*/, SPRING_TYPE type /*= SPRING_STRUCTURAL*/, 
	float unstrLen /*= UNSTRETCHED_LENGTH_DEFAULT*/, float stiffness /*= STIFFNESS_DEFAULT*/, float damping /*= DAMPING_DEFAULT*/)
{
	m_nodeA = nodeA;
	m_nodeB = nodeB;
	m_type = type;
	m_unstretchedLength = unstrLen;
	m_stiffness = stiffness;
	m_dampingCoefficient = damping;
}

void Spring::Render()
{
	if (m_type == SPRING_STRUCTURAL)
	{
		Vector3 tmpAPos = Vector3( ( float ) m_nodeA->m_position.x, ( float ) m_nodeA->m_position.y, ( float ) m_nodeA->m_position.z );
		Vector3 tmpBPos = Vector3( ( float ) m_nodeB->m_position.x, ( float ) m_nodeB->m_position.y, ( float ) m_nodeB->m_position.z );

		Rgba color = Rgba::BLUE;
		if (m_type == SPRING_SHEAR)
			color = Rgba::GREEN;
		if (m_type == SPRING_BEND)
			color = Rgba::RED;

		g_theRenderer->AddDebugLine( tmpAPos, tmpBPos, color, 0.f, RenderCommand::XRAY );
	}
}
