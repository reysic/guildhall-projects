#include "Engine/Math/Vector2.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
TheApp* g_theApp = nullptr;


//-----------------------------------------------------------------------------------------------
TheApp::TheApp()
{
	// Set all keys to "not down"
	for ( int keyIndex = 0; keyIndex < NUM_KEYS; ++keyIndex )
	{
		m_isKeyDown[ keyIndex ] = false;
	}

	// Set all Xbox controller pointers to nullptr
	for ( int index = 0; index < MAX_XBOX_CONTROLLERS; ++index )
	{
		m_controllers[ index ] = nullptr;
	}


	g_theGame = new TheGame();

	m_controllers[ 0 ] = new XboxController( 0 );

	g_theRenderer = new Renderer();
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );
}


//-----------------------------------------------------------------------------------------------
void TheApp::SetKeyDownStatus( unsigned char windowsKeyCode, bool isNowDown )
{
	m_isKeyDown[ windowsKeyCode ] = isNowDown;
}


//-----------------------------------------------------------------------------------------------
bool TheApp::GetKeyDownStatus( unsigned char windowsKeyCode )
{
	return m_isKeyDown[ windowsKeyCode ];
}


//-----------------------------------------------------------------------------------------------
bool TheApp::GetXboxADownStatus() const
{
	return m_controllers[ 0 ]->IsAButtonDown();
}


//-----------------------------------------------------------------------------------------------
bool TheApp::GetXboxBDownStatus() const
{
	return m_controllers[ 0 ]->IsBButtonDown();
}


//-----------------------------------------------------------------------------------------------
bool TheApp::GetXboxStartDownStatus() const
{
	return m_controllers[ 0 ]->IsStartButtonDown();
}


//-----------------------------------------------------------------------------------------------
bool TheApp::GetXboxRightTriggerPulledStatus() const
{
	return m_controllers[ 0 ]->IsRightTriggerPulled();
}


//-----------------------------------------------------------------------------------------------
Vector2 TheApp::GetXboxLeftStickPosition() const
{
	return m_controllers[ 0 ]->GetLeftStickPosition();
}


//-----------------------------------------------------------------------------------------------
Vector2 TheApp::GetXboxRightStickPosition() const
{
	return m_controllers[ 0 ]->GetRightStickPosition();
}


//-----------------------------------------------------------------------------------------------
PolarCoordinates TheApp::GetXboxLeftStickPositionPolar() const
{
	return m_controllers[ 0 ]->GetLeftStickPositionPolar();
}


//-----------------------------------------------------------------------------------------------
PolarCoordinates TheApp::GetXboxRightStickPositionPolar() const
{
	return m_controllers[ 0 ]->GetRightStickPositionPolar();
}


//-----------------------------------------------------------------------------------------------
void TheApp::Input()
{
	g_theGame->Input();
}


//-----------------------------------------------------------------------------------------------
void TheApp::Update( float deltaSeconds )
{
	m_controllers[ 0 ]->Update( deltaSeconds );
	g_theGame->Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void TheApp::Render() const
{
	g_theRenderer->ClearBuffer();

	g_theGame->Render();
}


//-----------------------------------------------------------------------------------------------
void TheApp::Shutdown()
{
	delete this;
}