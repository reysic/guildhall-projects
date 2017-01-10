#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <math.h>
#include <stdlib.h>

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/Ship.hpp"


//-----------------------------------------------------------------------------------------------
Ship::Ship()
{
	m_position.x = 800.f;
	m_position.y = 450.f;

	m_velocity.x = 0.f;
	m_velocity.y = 0.f;

	m_orientationDegrees = 90.f;
	m_angularVelocityDegreesPerSecond = 0.f;
	m_physicsRadius = 20.f;
	m_cosmeticRadius = 30.f;

	m_ageAtDeath = 0.0f;

	m_isThrusting = false;
	m_stopDeathAnim = false;

	m_bombCount = 1;
}


// #Eiserloh: Here is a quick simple example implementation of some basic movement logic for your ship.
// #Eiserloh: While this code works fine, there are plenty of further improvements that can be made.
// #Eiserloh: However, remember that your goal is always to keep code clean, simple, obvious, robust, and (of course) correct.
// #Eiserloh: For example, it may be inconsistent to have the "if W is down" check in Update, but "if A is down" in Turn(), etc.
// #Eiserloh: Also, some of these things (for example, the 'W' and 'A' and 'D' keys) would need to be generalized to support multiplayer - IF we cared.


//-----------------------------------------------------------------------------------------------
void Ship::Update( float deltaSeconds )
{
	Entity::Update( deltaSeconds );

	// #Eiserloh: ALL of your ship movement, turning, and acceleration should happen in this function!!!
	// #Eiserloh: Or, more specifically, all of the functions that Move, Turn, and Accelerate your ship should get called from this function.

	Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();

	if ( g_theApp->GetKeyDownStatus( 'W' ) || g_theApp->GetKeyDownStatus( VK_UP ) || ( xboxLeftStickPosition.Length() > 0 ) && !m_isDead )
	{
		ThrustForward( deltaSeconds );
		m_isThrusting = true;
	}
	else
	{
		m_isThrusting = false;
	}

	Turn( deltaSeconds );

	if (!m_isDead)
		m_stopDeathAnim = false;
}


//-----------------------------------------------------------------------------------------------
void Ship::ThrustForward( float deltaSeconds )
{
	// #Eiserloh: Which way is forward?
	float orientationRadians = ConvertDegreesToRadians( m_orientationDegrees );
	Vector2 forwardDirection( cos( orientationRadians ), sin( orientationRadians ) );

	Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();

	if ( xboxLeftStickPosition.Length() > 0  )
	{
		m_velocity += forwardDirection * xboxLeftStickPosition.Length() * SHIP_THRUST_STRENGTH * deltaSeconds;
	}
	else
	{
		m_velocity += forwardDirection * SHIP_THRUST_STRENGTH * deltaSeconds;
	}
}


//-----------------------------------------------------------------------------------------------
void Ship::Turn( float deltaSeconds )
{
	float degreesToTurnThisFrame = SHIP_DEGREES_TO_TURN_PER_SECOND * deltaSeconds;

	float turnDirection = 0.f; // #Eiserloh: 1.0 means "turn counter-clockwise" (positive direction), -1.0 means "turn clockwise" (negative direction)

	Vector2 xboxLeftStickPosition = g_theApp->GetXboxLeftStickPosition();

	if ( xboxLeftStickPosition.Length() > 0 )
	{
		m_orientationDegrees = ConvertRadiansToDegrees( atan2( xboxLeftStickPosition.y, xboxLeftStickPosition.x ) );
		return;
	}
	else if (g_theApp->GetKeyDownStatus( 'A' ) || g_theApp->GetKeyDownStatus( VK_LEFT ) && !m_isDead)
		turnDirection += 1.0f;
	else if (g_theApp->GetKeyDownStatus( 'D' ) || g_theApp->GetKeyDownStatus( VK_RIGHT ) && !m_isDead)
		turnDirection += -1.0f;

	degreesToTurnThisFrame *= turnDirection; // May be zero if not turning
	m_orientationDegrees += degreesToTurnThisFrame;
}


//-----------------------------------------------------------------------------------------------
void Ship::Render() const
{
	if ( m_isDead )
	{
		RenderDeathAnim();
		return;
	}

	int randomBool = rand() % 2;

	// Save current "graph paper"
	g_theRenderer->PushView();

	if ( g_theGame->GetDebugModeStatus() )
	{
		// Draw physics radius
		g_theRenderer->SetLineWidth( 0.25f );
		g_theRenderer->SetDrawColor( Rgba::RED );
		g_theRenderer->DrawHollowPolygon( 20, m_position.x, m_position.y, m_physicsRadius );

		// Draw cosmetic radius
		g_theRenderer->SetLineWidth( 0.25f );
		g_theRenderer->SetDrawColor( Rgba::GREEN );
		g_theRenderer->DrawHollowPolygon( 20, m_position.x, m_position.y, m_cosmeticRadius );
	}

	// Set up the "graph paper"
	g_theRenderer->TranslateView( Vector3( m_position.x, m_position.y, 0.0f ) );
	g_theRenderer->ScaleView( m_cosmeticRadius );
	g_theRenderer->RotateView( m_orientationDegrees, Vector3( 0.0f, 0.0f, 1.0f ) );

	// Draw the ship facing EAST (or 0 degrees)
	g_theRenderer->SetLineWidth( 1.5f );
	g_theRenderer->SetDrawColor( Rgba::WHITE );
	g_theRenderer->DrawLine3D( Vector3( -0.5f, 0.5f, 0.0f ), Vector3( 1.0f, 0.0f, 0.0f ), Rgba::WHITE, 1.5f );
	g_theRenderer->DrawLine3D( Vector3( -0.5f, -0.5f, 0.0f ), Vector3( 1.0f, 0.0f, 0.0f ), Rgba::WHITE, 1.5f );
	g_theRenderer->DrawLine3D( Vector3( -0.25f, -0.4f, 0.0f ), Vector3( -0.25f, 0.4f, 0.0f ), Rgba::WHITE, 1.5f );

	// Draw thruster if thrusting
	if ( randomBool == 0 && m_isThrusting )
	{
		g_theRenderer->DrawLine3D( Vector3( -0.25f, 0.2f, 0.0f ), Vector3( -0.7f, 0.0f, 0.0f ), Rgba::WHITE, 1.5f );
		g_theRenderer->DrawLine3D( Vector3( -0.7f, 0.0f, 0.0f ), Vector3( -0.25f, -0.2f, 0.0f ), Rgba::WHITE, 1.5f );
	}

	// Restore the "graph paper"
	g_theRenderer->PopView();
}


//-----------------------------------------------------------------------------------------------
void Ship::RenderDeathAnim() const
{
	float secondsSinceDeath = m_ageInSeconds - m_ageAtDeath;
	float fractionComplete = secondsSinceDeath / SHIP_DEATH_ANIM_SECONDS;
	fractionComplete = ClampFloatZeroToOne( fractionComplete );

	if ( m_stopDeathAnim )
		return;

	if ( fractionComplete >= 1.0f )
		return;

	// Save current "graph paper"
	g_theRenderer->PushView();

	// Set up the "graph paper"
	g_theRenderer->TranslateView( Vector3( m_position.x, m_position.y, 0.0f ) );
	g_theRenderer->ScaleView( m_cosmeticRadius );
	g_theRenderer->RotateView( m_orientationDegrees, Vector3( 0.0f, 0.0f, 1.0f ) );

	// Draw the ship facing EAST (or 0 degrees)
	g_theRenderer->SetLineWidth( 1.5f );
	g_theRenderer->SetDrawColor( Rgba( ( unsigned char ) 255, ( unsigned char ) 255, ( unsigned char ) 255, ( unsigned char ) ( 255 - ( int ) ( fractionComplete * ( float ) 255 ) ) ) ); //#FIXME

	Vector3 firstVertex = InterpolateBetweenVertices( Vector3( -0.5f, 0.5f, 0.0f ), Vector3( -0.5f, 1.2f, 0.0f ), fractionComplete );
	Vector3 secondVertex = InterpolateBetweenVertices( Vector3( 1.0f, 0.0f, 0.0f ), Vector3( 1.0f, 0.7f, 0.0f ), fractionComplete );

	g_theRenderer->DrawLine3D( firstVertex, secondVertex, Rgba::WHITE, 1.5f );

	firstVertex = InterpolateBetweenVertices( Vector3( -0.5f, -0.5f, 0.0f ), Vector3( -0.5f, -1.2f, 0.0f ), fractionComplete );
	secondVertex = InterpolateBetweenVertices( Vector3( 1.0f, 0.0f, 0.0f ), Vector3( 1.0f, -0.7f, 0.0f ), fractionComplete );

	g_theRenderer->DrawLine3D( firstVertex, secondVertex, Rgba::WHITE, 1.5f );

	firstVertex = InterpolateBetweenVertices( Vector3( -0.25f, 0.4f, 0.0f ), Vector3( -0.95f, 0.4f, 0.0f ), fractionComplete );
	secondVertex = InterpolateBetweenVertices( Vector3( -0.25f, -0.4f, 0.0f ), Vector3( -0.95f, -0.4f, 0.0f ), fractionComplete );

	g_theRenderer->DrawLine3D( firstVertex, secondVertex, Rgba::WHITE, 1.5f );

	// Restore the "graph paper"
	g_theRenderer->PopView();
}


//-----------------------------------------------------------------------------------------------
void Ship::DestroyShip()
{
	m_ageAtDeath = m_ageInSeconds;
	m_isDead = true;
}


//-----------------------------------------------------------------------------------------------
void Ship::Reset()
{
	m_position.x = 800.f;
	m_position.y = 450.f;

	m_velocity.x = 0.f;
	m_velocity.y = 0.f;

	m_orientationDegrees = 90.f;
	m_angularVelocityDegreesPerSecond = 0.f;
	m_physicsRadius = 20.f;
	m_cosmeticRadius = 30.f;

	m_isThrusting = false;
	m_isDead = false;

	m_stopDeathAnim = true;

	m_bombCount = 1;
}


//-----------------------------------------------------------------------------------------------
float Ship::GetFractionDeathAnimComplete() const
{
	if (!m_isDead)
		return 1.0f;

	float secondsSinceDeath = m_ageInSeconds - m_ageAtDeath;
	float fractionComplete = secondsSinceDeath / SHIP_DEATH_ANIM_SECONDS;
	fractionComplete = ClampFloatZeroToOne( fractionComplete );

	return fractionComplete;
}


//-----------------------------------------------------------------------------------------------
int Ship::GetBombCount() const
{
	return m_bombCount;
}


//-----------------------------------------------------------------------------------------------
void Ship::SetBombCount( int newBombCount)
{
	m_bombCount = newBombCount;
}