#pragma once


//-----------------------------------------------------------------------------------------------
const int NUM_KEYS = 256;


//-----------------------------------------------------------------------------------------------
const int MAX_XBOX_CONTROLLERS = 4;


//-----------------------------------------------------------------------------------------------
class TheApp;


//-----------------------------------------------------------------------------------------------
class XboxController;
class Vector2;
class PolarCoordinates;


//-----------------------------------------------------------------------------------------------
extern TheApp* g_theApp;


//-----------------------------------------------------------------------------------------------
class TheApp
{
public:
	// Constructors & Destructors
	TheApp();

	// Methods
	void SetKeyDownStatus( unsigned char windowsKeyCode, bool isNowDown );
	bool GetKeyDownStatus( unsigned char windowsKeyCode );
	bool GetXboxADownStatus() const;
	bool GetXboxBDownStatus() const;
	bool GetXboxStartDownStatus() const;
	bool GetXboxRightTriggerPulledStatus() const;
	Vector2 GetXboxLeftStickPosition() const;
	Vector2 GetXboxRightStickPosition() const;
	PolarCoordinates GetXboxLeftStickPositionPolar() const;
	PolarCoordinates GetXboxRightStickPositionPolar() const;
	void Input();
	void Update( float deltaSeconds );
	void Render() const;
	void Shutdown();

	XboxController* m_controllers[ MAX_XBOX_CONTROLLERS ];

private:
	bool		m_isKeyDown[ NUM_KEYS ];
};