#include "Game/TheApp.hpp"
#include "Engine/Core/Console.hpp"


TheApp* g_theApp = nullptr;



TheApp::TheApp(int screenWidth, int screenHeight, void* windowHandle)
	: m_canCloseGame(false)
	, m_inputSystem(windowHandle)
	, WINDOW_PHYSICAL_WIDTH(screenWidth)
	, WINDOW_PHYSICAL_HEIGHT(screenHeight)
{
}

void TheApp::Update()
{
	m_inputSystem.Update();
	if (g_theConsole->m_canExit)
	{
		m_canCloseGame = true;
	}
	//GET_WHEEL_DELTA_WPARAM(wParam);
}
