#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector2.hpp"

#include "Game/AIPlayer.hpp"


//-----------------------------------------------------------------------------------------------
AIPlayer::AIPlayer()
	: m_stillPlaying( true )
{
}

//-----------------------------------------------------------------------------------------------
Vector2 AIPlayer::GetShotDirection()
{
	float randomDegrees = GetRandomFloatInRange( 0.0f, 180.0f );
	float randomRadians = ConvertDegreesToRadians( randomDegrees );

	return Vector2( cos( randomRadians ), sin( randomRadians ) );
}
