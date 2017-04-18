#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/AimLine.hpp"


//-----------------------------------------------------------------------------------------------
AimLine::AimLine()
{
}


//-----------------------------------------------------------------------------------------------
void AimLine::Render() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	Vector2 lineEndPosition( m_lineStartPosition.x + m_forwardDirection.x * 300.0f,
		m_lineStartPosition.y + m_forwardDirection.y * 300.0f );
	Vector2 aimLineVector = lineEndPosition - m_lineStartPosition;
	aimLineVector.Normalize();

	PolarCoordinates xboxLeftStickPositionPolar = g_theInputSystem->GetXboxLeftStickPositionPolar( 0 );
	float distanceIncrement = RangeMap( xboxLeftStickPositionPolar.radius, 0.5f, 1.0f, 10.0f, 75.0f );
	float lineBallRadius = RangeMap( xboxLeftStickPositionPolar.radius, 0.5f, 1.0f, 2.0f, 5.0f );

	for ( float distanceAlongAimLine = distanceIncrement; distanceAlongAimLine <= ( distanceIncrement * 10.0f ); distanceAlongAimLine += distanceIncrement )
	{
		float aimLineDotXPos = m_lineStartPosition.x + distanceAlongAimLine * aimLineVector.x;
		float aimLineDotYPos = m_lineStartPosition.y + distanceAlongAimLine * aimLineVector.y;
		if ( aimLineDotXPos - lineBallRadius < 0.0f )
		{
			aimLineDotXPos = -aimLineDotXPos;
		}
		else if ( aimLineDotXPos + lineBallRadius > 700.0f )
		{
			aimLineDotXPos = 700.0f - ( aimLineDotXPos - 700.0f );
		}
		g_theRenderer->DrawFilledPolygon( 20, aimLineDotXPos, aimLineDotYPos, lineBallRadius, Rgba::WHITE );
	}
}