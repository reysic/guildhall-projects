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

	m_shutdown = false;
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
void TheApp::Input()
{
	g_theGame->Input();
}


//-----------------------------------------------------------------------------------------------
void TheApp::Update( float deltaSeconds )
{
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
	m_shutdown = true;
}
