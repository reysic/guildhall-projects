#pragma once


//-----------------------------------------------------------------------------------------------
const int NUM_KEYS = 256;


//-----------------------------------------------------------------------------------------------
class TheApp;


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

private:
	bool		m_isKeyDown[ NUM_KEYS ];
};