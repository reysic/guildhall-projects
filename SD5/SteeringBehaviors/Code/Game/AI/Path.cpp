#include "Engine/Renderer/Renderer.hpp"
#include "Game/AI/Path.hpp"


//-----------------------------------------------------------------------------------------------
Path::Path()
{
}


//-----------------------------------------------------------------------------------------------
Path::~Path()
{
	m_nodes.clear();
}


//-----------------------------------------------------------------------------------------------
void Path::Render() const
{
	if ( g_debugMode )
	{
		for ( unsigned int i = 0; i < m_nodes.size(); ++i )
		{
			// Is there another path node?
			if ( ( unsigned int ) ( i + 1 ) < m_nodes.size() )
			{
				// Draw a line connecting this path node to the next
				g_theRenderer->DrawLine3D( Vector3( m_nodes[ i ].x, m_nodes[ i ].y, 0.0f ),
					Vector3( m_nodes[ i + 1 ].x, m_nodes[ i + 1 ].y, 0.0f ), Rgba::WHITE,
					2.0f );
			}

			// Is this the first node?
			if ( i == 0 )
			{
				// Draw a line connecting this path node to the last node
				g_theRenderer->DrawLine3D( Vector3( m_nodes[ i ].x, m_nodes[ i ].y, 0.0f ),
					Vector3( m_nodes[ m_nodes.size() - 1 ].x, m_nodes[ m_nodes.size() - 1 ].y, 0.0f ), Rgba::WHITE,
					2.0f );
			}

			g_theRenderer->DrawFilledPolygon( 20, m_nodes[ i ].x, m_nodes[ i ].y, 0.2f, Rgba::RED );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Path::AddNode( const Vector2& nodePosition )
{
	m_nodes.push_back( nodePosition );
}