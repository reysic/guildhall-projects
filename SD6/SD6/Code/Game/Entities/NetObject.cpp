#include "Game/Entities/NetObject.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/DeveloperConsole.hpp"


//-----------------------------------------------------------------------------------------------
NetObject::NetObject()
	: m_isDead( false )
	, m_ageInSeconds( 0.0f )
{
}


//-----------------------------------------------------------------------------------------------
NetObject::NetObject( uint8_t ownerConnectionIndex, uint16_t netID, const Vector2& position )
	: m_ownerConnectionIndex( ownerConnectionIndex )
	, m_netID( netID )
	, m_position( position )
	, m_isDead( false )
	, m_ageInSeconds( 0.0f )
{
}


//-----------------------------------------------------------------------------------------------
void NetObject::Update( float deltaSeconds )
{
	m_ageInSeconds += deltaSeconds;
	m_position += m_velocity * deltaSeconds;
}