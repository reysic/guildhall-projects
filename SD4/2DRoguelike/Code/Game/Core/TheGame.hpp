#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Generators/Generator.hpp"
#include "Game/Environments/EnvironmentBlueprint.hpp"
#include "Game/Entities/Player.hpp"
#include "Game/NPCs/NPC.hpp"


//-----------------------------------------------------------------------------------------------
enum GameState
{
	STATE_START_UP,
	STATE_MAIN_MENU,
	STATE_PICK_MAP,
	STATE_GENERATING,
	STATE_PLAYING,
	STATE_SHUT_DOWN,
	NUM_GAME_STATES
};


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
struct Message
{
	std::string messageStr;
	Rgba messageColor;
};


//-----------------------------------------------------------------------------------------------
typedef std::multimap< float, Agent* > TurnOrderMap;
typedef std::multimap< float, Agent* >::iterator TurnOrderMapIter;
typedef std::pair< float, Agent* > TurnOrderMapPair;
class TheGame
{
public:
	TheGame();
	void Update( float deltaSeconds );
	void UpdateFPSCounter( float deltaSeconds );
	void UpdateSimulation( float deltaSeconds );
	void HandleMouseAndKeyboardInput();
	void HandleStateSpecificKeyboardInput();
	void HandlePickMapKeyboardInput();
	void HandlePlayingKeyboardInput();
	void Render() const;
	void SetUp2DView() const;
	void DrawMap() const;
	void DrawEntities() const;
	void DrawHUD() const;
	void DrawDebugText() const;
	GameState GetState() const { return m_state; }
	bool SetState( GameState newState );
	std::string GetGameStateString() const;
	void DrawStateSpecificText() const;
	void DrawMessageBar() const;
	bool SetPlayerStartPosition( Player* player );
	IntVector2 GetPlayerStartPosition( Map* map );
	void PrintMessage( const std::string& message, const Rgba& color = Rgba::WHITE );
	void CleanUpDeadEntities();
	bool IsMapValid( Map* map );
	void RemoveGameEntity( GameEntity* entityToRemove );
	void SaveGame();
	XMLNode SaveEntities();

public:
	float m_age;
	GameState m_state;
	Map* m_map;
	Generator* m_generator;
	bool m_autoGenerate;
	bool m_godModeEnabled;
	float m_ageOfNextGeneration;
	EnvironmentBlueprint* m_environmentBlueprint;
	std::vector< std::string > m_environmentBlueprintNames;
	int m_currentStep;
	int m_currentEnvironmentNumber;
	std::vector< GameEntity* > m_entities;
	Player* m_player;
	float m_simulationClock;
	TurnOrderMap m_turnOrderMap;
	int m_playerTurns;
	int m_playerKills;
	std::vector< Message > m_messages;
};