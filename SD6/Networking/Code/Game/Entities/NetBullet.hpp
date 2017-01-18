#pragma once

#include <cstdint>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/Entities/NetObject.hpp"


//-----------------------------------------------------------------------------------------------
const float NET_BULLET_SPEED = 480.0f;


//-----------------------------------------------------------------------------------------------
class NetBullet : public NetObject
{
public:
	NetBullet( uint8_t ownerConnectionIndex, uint16_t netID, const Vector2& position, 
		float xboxRightStickPositionPolarThetaRadians );
	void Update( float deltaSeconds );
	void Render() const;
};