#pragma once


//-----------------------------------------------------------------------------------------------
const int NUM_KEYS = 256;


//-----------------------------------------------------------------------------------------------
const int MAX_XBOX_CONTROLLERS = 4;


//-----------------------------------------------------------------------------------------------
class TheApp;


//-----------------------------------------------------------------------------------------------
class XboxController; // Forward declaration of XboxController


//-----------------------------------------------------------------------------------------------
extern TheApp* g_theApp;


//-----------------------------------------------------------------------------------------------
class TheApp
{
public:
	TheApp();
	void SetKeyDownStatus( unsigned char windowsKeyCode, bool isNowDown );
	bool GetKeyDownStatus( unsigned char windowsKeyCode );
	bool GetXboxADownStatus() const;
	bool GetXboxBDownStatus() const;
	bool GetXboxStartDownStatus() const;
	Vector2 GetXboxLeftStickPosition() const;
	bool IsPaused();
	void Input();
	void Update( float deltaSeconds );
	void Render() const;

	XboxController* m_controllers[ MAX_XBOX_CONTROLLERS ];

private:
	bool		m_isKeyDown[ NUM_KEYS ];
	bool		m_isPaused;
};