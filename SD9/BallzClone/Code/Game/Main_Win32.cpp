#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <cassert>
#include <crtdbg.h>

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library

#include "Engine/Core/Time.hpp"
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const int WINDOW_PHYSICAL_WIDTH = 700;
const int WINDOW_PHYSICAL_HEIGHT = 900;
const double VIEW_LEFT = 0.0;
const double VIEW_RIGHT = 700.0;
const double VIEW_BOTTOM = 0.0;
const double VIEW_TOP = VIEW_RIGHT * static_cast< double >( WINDOW_PHYSICAL_HEIGHT ) / static_cast< double >( WINDOW_PHYSICAL_WIDTH );

bool g_isQuitting = false;
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "BallzClone by Jeremy Hicks";


//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	unsigned char asKey = ( unsigned char ) wParam;

	if ( wmMessageCode == WM_CHAR && g_theDeveloperConsole->m_isOpen )
	{
		g_theDeveloperConsole->ReceiveInput( asKey );
	}

	switch ( wmMessageCode )
	{
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT:
		{
			g_isQuitting = true;
			return 0;
		}

		case WM_KEYDOWN:
		{
			g_theInputSystem->SetKeyDownStatus( asKey, true );
			if ( asKey == VK_ESCAPE )
			{
				g_isQuitting = true;
				return 0;
			}
			break;
		}

		case WM_KEYUP:
		{
			g_theInputSystem->SetKeyDownStatus( asKey, false );
			break;
		}
	}

	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow( HINSTANCE applicationInstanceHandle )
{
	// Define a window class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Assign a win32 message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );

	RECT windowRect = { OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_WIDTH, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_HEIGHT };
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( g_hWnd, SW_SHOW );
	SetForegroundWindow( g_hWnd );
	SetFocus( g_hWnd );

	g_displayDeviceContext = GetDC( g_hWnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );
}


//-----------------------------------------------------------------------------------------------
void RunMessagePump()
{
	MSG queuedMessage;
	for ( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if ( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage );
	}
}


//-----------------------------------------------------------------------------------------------
void Input()
{
	g_theGame->Input();
	RunMessagePump();
}


//-----------------------------------------------------------------------------------------------
void Update()
{
	// Time code by Squirrel Eiserloh
// 	const float MIN_SECONDS_PER_FRAME = ( 1.0f / 60.0f ) - 0.0001f;
// 
// 	static double s_timeLastFrameBegan = GetCurrentTimeSeconds(); // "static" local variables are initialized once, when first encountered, thus this function call only happens once
// 
// 	float deltaSeconds = static_cast< float >( GetCurrentTimeSeconds() - s_timeLastFrameBegan );
// 	while ( deltaSeconds < MIN_SECONDS_PER_FRAME )
// 	{
// 		deltaSeconds = ( float ) GetCurrentTimeSeconds() - ( float ) s_timeLastFrameBegan;
// 	}
// 
// 	s_timeLastFrameBegan = GetCurrentTimeSeconds();

	static double t = 0.0;
	static const double dt = 0.01;

	static double currentTime = GetCurrentTimeSeconds();
	static double accumulator = 0.0;

	double newTime = GetCurrentTimeSeconds();
	double frameTime = newTime - currentTime;
	currentTime = newTime;

	if ( g_theInputSystem->GetXboxRightTriggerPulledStatus( 0 ) )
	{
		accumulator += frameTime * 2.0f;
	}
	else if ( g_theInputSystem->GetXboxLeftTriggerPulledStatus( 0 ) )
	{
		accumulator += frameTime / 2.0f;
	}
	else
	{
		accumulator += frameTime;
	}

	while ( accumulator >= dt )
	{

		g_theGame->Update( dt );

		UpdateEngineSubsystems( dt );

		accumulator -= dt;
		t += dt;
	}
}


//-----------------------------------------------------------------------------------------------
void Render()
{
	g_theGame->Render();

	RenderEngineSubsystems();
}


//-----------------------------------------------------------------------------------------------
void RunFrame()
{
	ProfileFrameMark();

	Input();
	Update();
	Render();

	SwapBuffers( g_displayDeviceContext );
}


//-----------------------------------------------------------------------------------------------
void Initialize( HINSTANCE applicationInstanceHandle )
{
	CreateOpenGLWindow( applicationInstanceHandle );

	InitializeEngineSubsystems();

	g_theGame = new TheGame();

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	ShutdownEngineSubsystems();

	delete g_theGame;
	g_theGame = nullptr;
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	InitializeEngineCommon();

	UNUSED( commandLineString );
	Initialize( applicationInstanceHandle );

	// Initialize random seed
	srand( ( unsigned int ) time( nullptr ) );

	while ( !g_isQuitting )
	{
		RunFrame();
	}

	Shutdown();

	ShutdownEngineCommon();

	return 0;
}