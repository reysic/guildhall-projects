#pragma once

#include <cstdint>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"


//-----------------------------------------------------------------------------------------------
class NetObject
{
public:
	NetObject();
	NetObject( uint8_t ownerConnectionIndex, uint16_t netID, const Vector2& position );
	virtual void Update( float deltaSeconds );
	virtual void Render() const {}

public:
	uint8_t m_ownerConnectionIndex;
	uint16_t m_netID;
	Vector2 m_position;
	Vector2 m_velocity;
	bool m_isDead;
	float m_ageInSeconds;
	float m_cosmeticRadius;
};