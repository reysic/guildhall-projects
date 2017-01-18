#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <time.h>

#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const int WINDOW_PHYSICAL_WIDTH = 1600;
const int WINDOW_PHYSICAL_HEIGHT = 900;
const double VIEW_LEFT = 0.0;
const double VIEW_RIGHT = 1600.0;
const double VIEW_BOTTOM = 0.0;
const double VIEW_TOP = VIEW_RIGHT * static_cast< double >( WINDOW_PHYSICAL_HEIGHT ) / static_cast< double >( WINDOW_PHYSICAL_WIDTH );

bool g_isQuitting = false;
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "SimpleMiner by Jeremy Hicks";


//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	unsigned char asKey = ( unsigned char ) wParam;
	switch ( wmMessageCode )
	{
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT:
		g_isQuitting = true;
		return 0;

	case WM_KEYDOWN:
		g_theInputSystem->SetKeyDownStatus( asKey, true );
		if ( asKey == VK_ESCAPE )
		{
			g_isQuitting = true;
			return 0;
		}
		break;

	case WM_KEYUP:
		g_theInputSystem->SetKeyDownStatus( asKey, false );
		break;

	case WM_MOUSEWHEEL:
		if ( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 )
			g_theGame->IncrementHUDSelectedBlockType();
		else if ( GET_WHEEL_DELTA_WPARAM( wParam ) < 0 )
			g_theGame->DecrementHUDSelectedBlockType();
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
	RunMessagePump();
}


//-----------------------------------------------------------------------------------------------
void Update()
{
	const float MIN_SECONDS_PER_FRAME = ( 1.0f / 60.0f ) - 0.0001f;

	static double s_timeLastFrameBegan = GetCurrentTimeSeconds(); // "static" local variables are initialized once, when first encountered, thus this function call only happens once

	float deltaSeconds = static_cast<float>( GetCurrentTimeSeconds() - s_timeLastFrameBegan );
	while ( deltaSeconds < MIN_SECONDS_PER_FRAME )
	{
		deltaSeconds = ( float ) GetCurrentTimeSeconds() - ( float ) s_timeLastFrameBegan;
	}

	s_timeLastFrameBegan = GetCurrentTimeSeconds();

	g_theApp->Update( deltaSeconds );
	g_theInputSystem->Update( deltaSeconds );

	if ( GetFocus() == g_hWnd )
	{
		g_theInputSystem->m_appHasFocus = true;
	}
	else
	{
		g_theInputSystem->m_appHasFocus = false;
	}
}


//-----------------------------------------------------------------------------------------------
void Render()
{
	g_theApp->Render();
}


//-----------------------------------------------------------------------------------------------
void RunFrame()
{
	Input();
	Update();
	Render();

	SwapBuffers( g_displayDeviceContext );
}


//-----------------------------------------------------------------------------------------------
void Initialize( HINSTANCE applicationInstanceHandle )
{
	SetProcessDPIAware();
	CreateOpenGLWindow( applicationInstanceHandle );
	g_theApp = new TheApp();
	g_theGame = new TheGame();
	g_theRenderer = new Renderer();
	g_theAudioSystem = new AudioSystem();
	g_theInputSystem = new InputSystem();
}


//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	g_theGame->m_world->SaveActiveChunksToDisk();

	while ( !( g_theGame->m_world->m_activeChunksSavedToDisk ) )
	{
		// Wait for chunks to be saved to disk before shutting down
	}

	g_theGame->m_world->SavePlayerToDisk();

	while ( !( g_theGame->m_world->m_playerSavedToDisk ) )
	{
		// Wait to player to be saved to disk before shutting down
	}

	delete g_theInputSystem;
	g_theInputSystem = nullptr;
	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_theGame;
	g_theGame = nullptr;
	delete g_theApp;
	g_theApp = nullptr;
}

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	Initialize( applicationInstanceHandle );

	while ( !g_isQuitting )
	{
		RunFrame();
	}

	Shutdown();
	return 0;
}