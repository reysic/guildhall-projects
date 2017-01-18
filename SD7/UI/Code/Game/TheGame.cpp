#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
bool g_waitForTildeKeyUp = false;


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_canCloseGame( false )
{
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	deltaSeconds;

	if ( g_theInputSystem->IsKeyDown( 0xC0 ) && !g_waitForTildeKeyUp )
	{
		g_theDeveloperConsole->m_isOpen = !g_theDeveloperConsole->m_isOpen;
		g_waitForTildeKeyUp = true;
	}

	if ( !( g_theInputSystem->IsKeyDown( 0xC0 ) ) )
	{
		g_waitForTildeKeyUp = false;
	}

	if ( g_theDeveloperConsole->m_exitCommandIssued )
	{
		m_canCloseGame = true;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
 	g_theRenderer->ClearScreen( Rgba::GRAY );
 	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );
}