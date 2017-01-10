#pragma once


//-----------------------------------------------------------------------------------------------
const int NUM_KEYS = 256;


//-----------------------------------------------------------------------------------------------
class TheApp;


//-----------------------------------------------------------------------------------------------
class Vector2; // forward declaration of Vector2


//-----------------------------------------------------------------------------------------------
extern TheApp* g_theApp;


//-----------------------------------------------------------------------------------------------
class TheApp
{
public:
	TheApp();
	void SetKeyDownStatus( unsigned char windowsKeyCode, bool isNowDown );
	bool GetKeyDownStatus( unsigned char windowsKeyCode );
	void Input();
	void Update( float deltaSeconds );
	void Render() const;
	void Shutdown();

	bool m_shutdown;

private:
	bool		m_isKeyDown[ NUM_KEYS ];
};