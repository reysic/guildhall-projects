#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Entity.hpp"
#include "Game/Map.hpp"


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_debugModeOn( false )
	, m_waitToToggleDebugMode( false )
	, m_pauseModeOn( false )
	, m_waitToTogglePauseMode( false )
	, m_pauseModeCountdownInitiated( false )
	, m_respawnPlayerTank( false )
	, m_disableFiring( false )
	, m_attractModeOn( true )
{
}



//-----------------------------------------------------------------------------------------------
TheGame::~TheGame()
{
	DestroyMap();
}


//-----------------------------------------------------------------------------------------------
void TheGame::Input()
{
	if ( m_attractModeOn )
	{
		if ( ( g_theApp->GetKeyDownStatus( 'P' ) || g_theApp->GetXboxStartDownStatus() ) && !m_waitToTogglePauseMode )
		{
			m_attractModeOn = false;
			MakeMap();
			m_waitToTogglePauseMode = true;
		}
	}
	else
	{
		if ( !m_pauseModeCountdownInitiated )
		{
			// Check for 'K', which toggles debug mode
			if ( g_theApp->GetKeyDownStatus( 'K' ) && !m_waitToToggleDebugMode )
			{
				m_debugModeOn = !m_debugModeOn;
				m_waitToToggleDebugMode = true;
			}

			if ( !( g_theApp->GetKeyDownStatus( 'K' ) ) && m_waitToToggleDebugMode )
			{
				m_waitToToggleDebugMode = false;
			}

			// Check for 'P' or Xbox 'Start', which toggles pause
			if ( ( g_theApp->GetKeyDownStatus( 'P' ) || g_theApp->GetXboxStartDownStatus() ) && !m_waitToTogglePauseMode )
			{
				if ( !m_pauseModeOn )
				{
					g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( PAUSE_MODE_INITIATE_SOUND_FILE ), 1.0f );
					m_pauseModeOn = true;
				}
				else
				{
					g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( PAUSE_MODE_END_SOUND_FILE ), 1.0f );
					m_pauseModeOn = false;
				}
				m_waitToTogglePauseMode = true;
			}

			if ( ( !( g_theApp->GetKeyDownStatus( 'P' ) ) && !( g_theApp->GetXboxStartDownStatus() ) ) && m_waitToTogglePauseMode )
			{
				m_waitToTogglePauseMode = false;
			}

			if ( !m_pauseModeOn )
				g_theMap->Input();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	if ( m_attractModeOn )
	{
	}
	else
	{
		if ( m_pauseModeCountdownInitiated )
		{
			if ( ( float ) GetCurrentTimeSeconds() > ( m_timeWhenPlayerTankDied + PAUSE_MODE_COUNTDOWN_LENGTH_IN_SECONDS ) )
			{
				g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( PAUSE_MODE_INITIATE_SOUND_FILE ), 1.0f );
				m_pauseModeCountdownInitiated = false;
				m_pauseModeOn = true;
				m_respawnPlayerTank = true;
			}
		}

		if ( m_pauseModeOn )
		{
			g_theMap->Update( 0.0f ); // pause the game
		}
		else
		{
			if ( m_respawnPlayerTank )
			{
				g_theMap->RespawnPlayerTank();
				m_respawnPlayerTank = false;
			}

			g_theMap->Update( deltaSeconds ); // update the map
		}

	}

	g_theAudioSystem->Update( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	if ( m_attractModeOn )
	{
		g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );
		g_theRenderer->DrawQuad( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ), Rgba( 0, 0, 0, 255 ) );
		g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) ), *Texture::CreateOrGetTexture( ATTRACT_SCREEN_TEXTURE_FILE ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ),
			Rgba( 255, 255, 255, 255 ) );
	}
	else
	{
		g_theMap->Render(); // draw the map
	}
}


//-----------------------------------------------------------------------------------------------
bool TheGame::GetDebugModeStatus() const
{
	return m_debugModeOn;
}


//-----------------------------------------------------------------------------------------------
bool TheGame::GetPauseModeStatus() const
{
	return m_pauseModeOn;
}


//-----------------------------------------------------------------------------------------------
void TheGame::StartPauseModeCountdown()
{
	if ( !m_pauseModeCountdownInitiated )
	{
		m_timeWhenPlayerTankDied = ( float ) GetCurrentTimeSeconds();
		m_pauseModeCountdownInitiated = true;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::MakeMap()
{
	g_theMap = new Map( IntVector2( 60, 60 ) );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DestroyMap()
{
	if ( g_theMap != nullptr )
	{
		delete g_theMap;
		g_theMap = nullptr;
	}
}