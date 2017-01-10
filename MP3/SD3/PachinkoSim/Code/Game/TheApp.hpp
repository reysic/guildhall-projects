#pragma once
#include "Engine/Core/InputSystem.hpp"

class TheApp;

extern TheApp* g_theApp;


class TheApp
{
public:
	TheApp(int screenWidth, int screenHeight, void* windowHandle);
	void Update();

	bool m_canCloseGame;
	InputSystem m_inputSystem;
	int WINDOW_PHYSICAL_WIDTH;
	int WINDOW_PHYSICAL_HEIGHT;

};

