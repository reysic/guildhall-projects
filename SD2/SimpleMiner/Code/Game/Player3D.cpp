#include "Game/TheGame.hpp"
#include "Game/Player3D.hpp"


//-----------------------------------------------------------------------------------------------
Player3D::Player3D()
	: m_position( PLAYER_INITIAL_POSITION )
	, m_velocity( Vector3( 0.0f, 0.0f, 0.0f ) )
	, m_orientation( PLAYER_INITIAL_ORIENTATION )
	, m_movingForward( false )
	, m_movingBackward( false )
	, m_movingLeft( false )
	, m_movingRight( false )
	, m_movingUp( false )
	, m_movingDown( false )
	, m_boundingBox( AABB3( Vector3::ZERO, Vector3::ZERO ) )
{
}
