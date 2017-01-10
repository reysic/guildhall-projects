#pragma once


//-----------------------------------------------------------------------------------------------
const int NUM_KEYS = 256;


//-----------------------------------------------------------------------------------------------
const int MAX_XBOX_CONTROLLERS = 4;


//-----------------------------------------------------------------------------------------------
class TheApp;


//-----------------------------------------------------------------------------------------------
class XboxController; // forward declaration of XboxController
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
	bool GetXboxADownStatus() const;
	bool GetXboxBDownStatus() const;
	bool GetXboxStartDownStatus() const;
	bool GetXboxRightTriggerPulledStatus() const;
	bool GetXboxLeftTriggerPulledStatus() const;
	Vector2 GetXboxLeftStickPosition() const;
	Vector2 GetXboxRightStickPosition() const;
	void Input();
	void Update( float deltaSeconds );
	void Render() const;

	XboxController* m_controllers[ MAX_XBOX_CONTROLLERS ];

private:
	bool		m_isKeyDown[ NUM_KEYS ];
};