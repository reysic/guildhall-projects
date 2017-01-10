#include "Engine/Input/DeveloperConsole.hpp"
#include "Game/Core/TheApp.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
TheApp* g_theApp = nullptr;


//-----------------------------------------------------------------------------------------------
TheApp::TheApp()
	: m_canCloseGame( false )
{
}


//-----------------------------------------------------------------------------------------------
void TheApp::Update( float deltaSeconds )
{
	g_theGame->Update( deltaSeconds );

	if ( g_theDeveloperConsole->m_exitCommandIssued )
	{
		m_canCloseGame = true;
	}
}


//-----------------------------------------------------------------------------------------------
void TheApp::Render() const
{
	g_theGame->Render();
}