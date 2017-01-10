#pragma once

#include <cstdint>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/Entities/NetObject.hpp"


//-----------------------------------------------------------------------------------------------
const float NET_PLAYER_SPEED = 160.0f;


//-----------------------------------------------------------------------------------------------
class NetPlayer : public NetObject
{
public:
	NetPlayer( uint8_t ownerConnectionIndex, uint16_t netID, const Vector2& position );
	void Update( float deltaSeconds );
	void Render() const;
	void CheckForScreenEdge();
};