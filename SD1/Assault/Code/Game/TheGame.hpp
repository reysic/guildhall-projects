#pragma once


#include "Engine/Audio/AudioSystem.hpp"


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
class Map; // Forward declaration of Map


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
const float PAUSE_MODE_COUNTDOWN_LENGTH_IN_SECONDS = 5.0f;
const std::string PAUSE_MODE_INITIATE_SOUND_FILE = "Data/Audio/Magic_AuraHit1.wav";
const std::string PAUSE_MODE_END_SOUND_FILE = "Data/Audio/Magic_AuraHit2.wav";
const std::string ATTRACT_SCREEN_TEXTURE_FILE = "Data/Images/Attract.png";


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	// Constructors & Destructors
	TheGame();
	~TheGame();

	// Methods
	void Input();
	void Update( float deltaSeconds );
	void Render() const;
	bool GetDebugModeStatus() const;
	bool GetPauseModeStatus() const ;
	void StartPauseModeCountdown();
	void MakeMap();
	void DestroyMap();

	bool m_disableFiring;

private:
	bool m_debugModeOn;
	bool m_waitToToggleDebugMode;
	bool m_pauseModeOn;
	bool m_waitToTogglePauseMode;
	bool m_pauseModeCountdownInitiated;
	float m_timeWhenPlayerTankDied;
	bool m_respawnPlayerTank;
	bool m_attractModeOn;
};