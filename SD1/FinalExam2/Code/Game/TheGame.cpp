#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Snake.hpp"


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_snake( nullptr )
{
	m_snake = new Snake();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Input()
{
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	// Call Update() for snake
	m_snake->Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{

	m_snake->Render();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Shutdown()
{
	g_theApp->Shutdown();
}