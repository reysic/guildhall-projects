#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/PolarCoordinates.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/DeveloperConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Sprites/SpriteSheet.hpp"
#include "Engine/Renderer/Fonts/BitmapFont.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/Core/GameCommon.hpp"
#include "Game/Core/TheApp.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Environments/EnvironmentBlueprint.hpp"
#include "Game/NPCs/NPCFactory.hpp"
#include "Game/Core/Faction.hpp"
#include "Game/Entities/ItemFactory.hpp"
#include "Game/Entities/FeatureFactory.hpp"


//-----------------------------------------------------------------------------------------------
bool g_cursorSetToSnapBackPos = false;
bool g_shutDownInitiated = false;
bool g_renderedPath = false;
float g_updateLoopStart = 0.0f;
float g_updateLoopEnd = 0.0f;
float g_renderLoopStart = 0.0f;
float g_renderLoopEnd = 0.0f;


//-----------------------------------------------------------------------------------------------
TheGame* g_theGame = nullptr;
float g_deltaSeconds = 0.0f;
float g_timePassed = 0.0f;
float g_fps = 0.0f;
float g_shutDownTime = 0.0f;


//-----------------------------------------------------------------------------------------------
TheGame::TheGame()
	: m_age( 0.0f )
	, m_state( STATE_START_UP )
	, m_autoGenerate( true )
	, m_godModeEnabled( false )
	, m_ageOfNextGeneration( 0.0f )
	, m_currentStep( 0 )
	, m_currentEnvironmentNumber( 0 )
	, m_simulationClock( 0.0f )
	, m_player( nullptr )
	, m_playerTurns( 0 )
	, m_playerKills( 0 )
{
	EnvironmentBlueprint::LoadEnvironmentBlueprints();
	EnvironmentBlueprint::s_environmentBlueprintRegistrationMap;
	for ( auto envIter = EnvironmentBlueprint::s_environmentBlueprintRegistrationMap->begin();
	envIter != EnvironmentBlueprint::s_environmentBlueprintRegistrationMap->end(); ++envIter )
	{
		std::string environmentName = ( envIter )->second->GetEnvironmentName();
		m_environmentBlueprintNames.push_back( environmentName );
	}

	Faction::LoadAllFactions();
	Faction::s_globalFactionMap;

	NPCFactory::LoadNPCBlueprints();
	NPCFactory::s_npcFactories;

	ItemFactory::LoadItemBlueprints();
	ItemFactory::s_itemFactories[ ITEM_TYPE_ARMOR ];

	FeatureFactory::LoadFeatureBlueprints();
	FeatureFactory::s_featureFactories;

	m_generator = GeneratorRegistration::CreateGeneratorByName( "CellularAutomata" );
}


//-----------------------------------------------------------------------------------------------
void TheGame::Update( float deltaSeconds )
{
	g_updateLoopStart = ( float ) GetCurrentTimeSeconds();
	m_age += deltaSeconds;

	if ( m_age > 0.0f && GetState() == STATE_START_UP )
	{
		SetState( STATE_MAIN_MENU );
	}

	if ( GetState() == STATE_SHUT_DOWN )
	{
		if ( !g_shutDownInitiated )
		{
			g_shutDownInitiated = true;
			g_shutDownTime = m_age;
		}
		else
		{
			if ( m_age > g_shutDownTime )
			{
				g_theApp->m_canCloseGame = true;
			}
		}
	}

	HandleMouseAndKeyboardInput();

	g_theAudioSystem->Update( deltaSeconds );

	g_deltaSeconds = deltaSeconds;

	UpdateFPSCounter( deltaSeconds );

	// Update simulation
	if ( GetState() == STATE_PLAYING )
	{
		UpdateSimulation( deltaSeconds );
	}

	g_updateLoopEnd = ( float ) GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateFPSCounter( float deltaSeconds )
{
	g_timePassed += deltaSeconds;

	static int frames = 0;
	static float startTime = 0;
	static bool first = true;

	if ( first )
	{
		frames = 0;
		startTime = g_timePassed;
		first = false;
		return;
	}

	frames++;

	if ( g_timePassed - startTime > 0.25 && frames > 10 )
	{
		g_fps = frames / ( g_timePassed - startTime );
		startTime = g_timePassed;
		frames = 0;
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::UpdateSimulation( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	bool isSimulating = true;
	bool advanceTime = true;
	float simulationDelta = 0.1f;

	while ( isSimulating )
	{
		if ( m_player == nullptr )
		{
			isSimulating = false;
			break;
		}

		TurnOrderMapIter agentIter = m_turnOrderMap.begin();
		Agent* agent = agentIter->second;
		if ( agentIter->first > m_simulationClock )
		{
			break;
		}
		if ( agent->m_type == GAME_ENTITY_PLAYER )
		{
			Player* player = ( Player* ) agent;
			player->ProcessInput();
		}

		if ( !agent->IsReadyToUpdate() )
		{
			advanceTime = false;
			break;
		}

		m_turnOrderMap.erase( agentIter );
		if ( agent->IsAlive() )
		{
			float duration = agent->Update();
			if ( agent->IsAlive() ) // Ensure agent is still alive post-update
			{
				m_turnOrderMap.insert( TurnOrderMapPair( m_simulationClock + duration, agent ) );
			}
		}
	}

	if ( advanceTime )
	{
		m_simulationClock += simulationDelta;
	}

	CleanUpDeadEntities();

	HandlePlayingKeyboardInput();
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleMouseAndKeyboardInput()
{
	const float mouseSensitivityYaw = 0.022f;
	const float mouseSensitivityPitch = 0.022f;

	const IntVector2 cursorSnapBackPos( 800, 600 );
	g_theInputSystem->SetMouseCursorSnapBackPosition( cursorSnapBackPos );
	IntVector2 cursorPosition = g_theInputSystem->GetMouseCursorPosition();
	int cursorDeltaX = cursorPosition.x - cursorSnapBackPos.x;
	int cursorDeltaY = cursorPosition.y - cursorSnapBackPos.y;

	cursorDeltaX;
	cursorDeltaY;

	// Does app have focus?
	if ( g_theInputSystem->m_appHasFocus )
	{
		// App has focus
		if ( !g_cursorSetToSnapBackPos )
		{
			g_theInputSystem->SetMouseCursorPosition( cursorSnapBackPos );
			g_cursorSetToSnapBackPos = true;
		}
		// g_theInputSystem->HideMouseCursor();

		if ( !( g_theDeveloperConsole->m_isOpen ) )
		{
			HandleStateSpecificKeyboardInput();
		}

		if ( g_theInputSystem->IsKeyPressed( 0xC0 ) ) // ~ key
		{
			g_theDeveloperConsole->m_isOpen = !g_theDeveloperConsole->m_isOpen;
		}
	}
	else // App does not have focus
	{
		g_cursorSetToSnapBackPos = false;
		g_theInputSystem->ShowMouseCursor();
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleStateSpecificKeyboardInput()
{
	if ( GetState() == STATE_MAIN_MENU && g_theInputSystem->IsKeyPressed( 'N' ) )
	{
		SetState( STATE_PICK_MAP );
	}
	if ( GetState() == STATE_MAIN_MENU && g_theInputSystem->IsKeyPressed( 'Q' ) )
	{
		SetState( STATE_SHUT_DOWN );
	}
	else if ( GetState() == STATE_PICK_MAP )
	{
		HandlePickMapKeyboardInput();
	}
	else if ( GetState() == STATE_GENERATING && ( ( g_theInputSystem->IsKeyPressed( VK_SPACE ) ) || m_autoGenerate ) )
	{
		if ( m_environmentBlueprint->GenerateStep( m_map, m_currentStep ) )
		{
			m_currentStep++;
		}
		else
		{
// 			// Validate map with A* before continuing
// 			if ( !IsMapValid( m_map ) )
// 			{
// 				delete m_map;
// 				m_map = nullptr;
// 				SetState( STATE_MAIN_MENU );
// 			}

			m_environmentBlueprint->m_currentGenerator->FinalizeMap( m_map );
			Player* player = new Player( GAME_ENTITY_PLAYER );
			player->m_map = m_map;
			ASSERT_OR_DIE( SetPlayerStartPosition( player ), "Unable to find valid player start position!" );
			m_turnOrderMap.insert( TurnOrderMapPair( m_simulationClock, player ) );
			m_entities.push_back( player );
			m_player = player;

			m_map->m_debugRender = false;
			m_godModeEnabled = false;
			SetState( STATE_PLAYING );
		}
	}
	else if ( GetState() == STATE_GENERATING && g_theInputSystem->IsKeyPressed( VK_RETURN ) )
	{
// 		// Validate map with A* before continuing
// 		if ( !IsMapValid( m_map ) )
// 		{
// 			delete m_map;
// 			m_map = nullptr;
// 			SetState( STATE_MAIN_MENU );
// 		}

		m_environmentBlueprint->m_currentGenerator->FinalizeMap( m_map );
		Player* player = new Player( GAME_ENTITY_PLAYER );
		player->m_map = m_map;
		ASSERT_OR_DIE( SetPlayerStartPosition( player ), "Unable to find valid player start position!" );
		m_turnOrderMap.insert( TurnOrderMapPair( m_simulationClock, player ) );
		m_entities.push_back( player );
		m_player = player;

		m_map->m_debugRender = false;
		m_godModeEnabled = false;
		SetState( STATE_PLAYING );
	}
	else if ( GetState() == STATE_GENERATING && g_theInputSystem->IsKeyPressed( VK_ESCAPE ) )
	{
		SetState( STATE_PICK_MAP );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandlePickMapKeyboardInput()
{
	if ( GetState() == STATE_PICK_MAP )
	{
		int size = EnvironmentBlueprint::s_environmentBlueprintRegistrationMap->size();
		int index = -1;

		if ( g_theInputSystem->IsKeyPressed( '1' ) && size >= 0 )
		{
			index = 0;
		}
		else if ( g_theInputSystem->IsKeyPressed( '2' ) && size >= 1  )
		{
			index = 1;
		}
		else if ( g_theInputSystem->IsKeyPressed( '3' ) && size >= 2 )
		{
			index = 2;
		}
		else if ( g_theInputSystem->IsKeyPressed( '4' ) && size >= 3 )
		{
			index = 3;
		}
		else if ( g_theInputSystem->IsKeyPressed( '5' ) && size >= 4 )
		{
			index = 4;
		}
		else if ( g_theInputSystem->IsKeyPressed( '6' ) && size >= 5 )
		{
			index = 5;
		}
		else if ( g_theInputSystem->IsKeyPressed( '7' ) && size >= 6 )
		{
			index = 6;
		}
		else if ( g_theInputSystem->IsKeyPressed( '8' ) && size >= 7 )
		{
			index = 7;
		}
		else if ( g_theInputSystem->IsKeyPressed( '9' ) && size >= 8 )
		{
			index = 8;
		}
		else if ( g_theInputSystem->IsKeyPressed( '0' ) && size >= 9 )
		{
			index = 9;
		}

		if ( index >= 0 )
		{
			SetState( STATE_GENERATING );
			m_environmentBlueprint = EnvironmentBlueprint::s_environmentBlueprintRegistrationMap->at( m_environmentBlueprintNames[ index ] );
			m_map = m_environmentBlueprint->CreateInitializedMap();
			m_map->m_debugRender = true;
			m_currentStep = 0;
		}

		if ( g_theInputSystem->IsKeyPressed( VK_TAB ) )
		{
			m_autoGenerate = !m_autoGenerate;
		}

		if ( g_theInputSystem->IsKeyPressed( VK_ESCAPE ) )
		{
			SetState( STATE_MAIN_MENU );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandlePlayingKeyboardInput()
{
	if ( g_theInputSystem->IsKeyPressed( 'M' ) )
	{
		if ( m_map )
		{
			m_map->m_debugRender = !m_map->m_debugRender;
		}
	}
	else if ( g_theInputSystem->IsKeyPressed( 'G' ) )
	{
		m_godModeEnabled = !m_godModeEnabled;
	}
	else if ( g_theInputSystem->IsKeyPressed( VK_F6 ) )
	{
		SaveGame();
	}
	else if ( g_theInputSystem->IsKeyPressed( 'P' ) )
	{
		if ( m_player != nullptr )
		{
			if ( m_player->m_pathfinder )
			{
				if ( m_player->m_pathfinder->m_isFinished == false )
				{
					m_player->m_pathfinder->TakeOneStep();
				}
				else
				{
					m_player->m_pathfinder = new Pathfinder( m_player->m_position, m_map->GetRandomAirCellPosition(), m_map, m_player );
				}
			}
			else
			{
				m_player->m_pathfinder = new Pathfinder( m_player->m_position, m_map->GetRandomAirCellPosition(), m_map, m_player );
			}
		}
	}
	else if ( g_theInputSystem->IsKeyPressed( VK_ESCAPE ) )
	{
		delete m_map;
		m_map = nullptr;
		m_entities.clear();
		m_turnOrderMap.clear();
		delete m_player;
		m_player = nullptr;
		m_environmentBlueprint->ClearBlueprint();
		m_currentStep = 0;
		m_messages.clear();
		SetState( STATE_PICK_MAP );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::Render() const
{
	g_renderLoopStart = ( float ) GetCurrentTimeSeconds();
	// Clear the screen (color and depths)
	g_theRenderer->ClearScreen( Rgba::BLACK ); // Affects the Back Buffer (Default FBO)

	SetUp2DView();
	DrawMap();
	DrawEntities();
	DrawHUD();
	g_renderLoopEnd = ( float ) GetCurrentTimeSeconds();
}


//-----------------------------------------------------------------------------------------------
void TheGame::SetUp2DView() const
{
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawMap() const
{
	if ( GetState() == STATE_GENERATING || GetState() == STATE_PLAYING )
	{
		m_map->Render();
	}
}

//-----------------------------------------------------------------------------------------------
void TheGame::DrawEntities() const
{
	// Iterate over entities and call Render()
	if ( m_entities.size() > 0 )
	{
		for ( GameEntity* entity : m_entities )
		{
			entity->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawHUD() const
{
	if ( g_debugMode )
	{
		DrawDebugText();
	}
	else
	{
		DrawStateSpecificText();
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawDebugText() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	static BitmapFontProportional* proportionalFont = BitmapFontProportional::CreateOrGetFont( "Data/Fonts/RuneScape.fnt" );

	std::string camPosString = Stringf( "camPos=(%.02f, %.02f, %.02f)", g_theCamera->m_position.x, g_theCamera->m_position.y, g_theCamera->m_position.z );
	std::string camOrientString = Stringf( "pitch=%.0f  roll=%.0f  yaw=%.0f", g_theCamera->m_orientation.m_pitchDegreesAboutX, g_theCamera->m_orientation.m_rollDegreesAboutY, g_theCamera->m_orientation.m_yawDegreesAboutZ );
	std::string camFwdString = Stringf( "fwd=(%.02f, %.02f, %.02f)", g_theCamera->GetForwardXYZ().x, g_theCamera->GetForwardXYZ().y, g_theCamera->GetForwardXYZ().z );
	std::string fpsString = Stringf( "FPS:%.0f", g_fps );

	g_theRenderer->DrawText2D( Vector2( 0.0f, 775.0f ), "UPDATE LOOP TIME: " + Stringf( "%.02f", ( g_updateLoopEnd - g_updateLoopStart ) ) + "sec", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 750.0f ), "RENDER LOOP TIME: " + Stringf( "%.02f", ( g_renderLoopEnd - g_renderLoopStart ) ) + "sec", 15.0f, Rgba::GREEN, fixedFont );

	g_theRenderer->DrawText2D( Vector2( 1500.0f, 875.0f ), fpsString, 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 625.0f ), "Mouse X: " + Stringf( "%i", g_theInputSystem->GetMouseCursorPosition().x )
	 			+ " Mouse Y: " + Stringf( "%i", g_theInputSystem->GetMouseCursorPosition().y ), 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 600.0f ), "Current State: " + GetGameStateString(), 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 550.0f ), Stringf( "m_entities size: %i", m_entities.size() ), 15.0f, Rgba::GREEN, fixedFont );

	g_theRenderer->DrawText2D( Vector2( 0.0f, 825.0f ), "Camera Movement: W, A, S, D, X, Spacebar, Mouse", 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 0.0f, 875.0f ), camPosString, 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 500.0f, 875.0f ), camOrientString, 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 1000.0f, 875.0f ), camFwdString, 15.0f, Rgba::GREEN, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 1500.0f, 875.0f ), fpsString, 15.0f, Rgba::GREEN, fixedFont );
}


//-----------------------------------------------------------------------------------------------
bool TheGame::SetState( GameState newState )
{
	bool didChange = false;

	if ( GetState() != newState )
	{
		m_state = newState;
		didChange = true;
	}
	else
	{
		ASSERT_OR_DIE( false, "Attempted to set new state to same state!" );
	}

	return didChange;
}


//-----------------------------------------------------------------------------------------------
std::string TheGame::GetGameStateString() const
{
	if ( GetState() == STATE_START_UP )
	{
		return Stringf( "STATE_START_UP" );
	}
	else if ( GetState() == STATE_MAIN_MENU )
	{
		return Stringf( "STATE_MAIN_MENU" );
	}
	else if ( GetState() == STATE_PICK_MAP )
	{
		return Stringf( "STATE_PICK_MAP" );
	}
	else if ( GetState() == STATE_GENERATING )
	{
		return Stringf( "STATE_GENERATING" );
	}
	else if ( GetState() == STATE_PLAYING )
	{
		return Stringf( "STATE_PLAYING" );
	}
	else if ( GetState() == STATE_SHUT_DOWN )
	{
		return Stringf( "STATE_SHUT_DOWN" );
	}
	else
	{
		ASSERT_OR_DIE( false, "Undefined state!" );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawStateSpecificText() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	static BitmapFontProportional* proportionalFont = BitmapFontProportional::CreateOrGetFont( "Data/Fonts/RuneScape.fnt" );

	if ( GetState() == STATE_START_UP )
	{
		g_theRenderer->DrawText2D( Vector2( 0.0f, 825.0f ), "Transitions to STATE_MAIN_MENU after 2 seconds...", 20.0f, Rgba::YELLOW, fixedFont );
	}
	else if ( GetState() == STATE_MAIN_MENU )
	{
		g_theRenderer->DrawTextProportional2D( Vector3( 150.0f, 700.0f, 0.0f ), "Perceptron", 10.0f, Rgba::WHITE, proportionalFont );
		g_theRenderer->DrawText2D( Vector2( 700.0f, 200.0f ), "New Game (N)", 20.0f, Rgba::YELLOW, fixedFont );
		std::vector< std::string > saveFiles = EnumerateFilesInFolder( "Data/Saves", "*.Save.xml" );
		if ( saveFiles.size() > 0 )
		{
			g_theRenderer->DrawText2D( Vector2( 700.0f, 160.0f ), "Continue (C)", 20.0f, Rgba::YELLOW, fixedFont );
			g_theRenderer->DrawText2D( Vector2( 700.0f, 120.0f ), "Quit (Q)", 20.0f, Rgba::YELLOW, fixedFont );
		}
		else
		{
			g_theRenderer->DrawText2D( Vector2( 700.0f, 160.0f ), "Quit (Q)", 20.0f, Rgba::YELLOW, fixedFont );
		}
	}
	else if ( GetState() == STATE_PICK_MAP )
	{
		g_theRenderer->DrawText2D( Vector2( 600.0f, 725.0f ), "Quest Selection:", 20.0f, Rgba::YELLOW, fixedFont );
		int numGen = 1;

		for ( auto envIter = m_environmentBlueprintNames.begin(); envIter != m_environmentBlueprintNames.end(); ++envIter )
		{
			std::string name = *envIter;
			std::string combined = Stringf( "%i : ", numGen ) + name;
			g_theRenderer->DrawText2D( Vector2( 600.0f, 725.0f - ( numGen * 25.0f ) ), combined, 20.0f, Rgba::YELLOW, fixedFont );
			numGen++;

		}
		std::string autoGenStr;
		if ( m_autoGenerate ) { autoGenStr = "Auto Generation: ON"; } else { autoGenStr = "Auto Generation: OFF"; }
		g_theRenderer->DrawText2D( Vector2( 600.0f, 400.0f ), autoGenStr, 20.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 600.0f, 375.0f ), "Press TAB to toggle auto generation.", 20.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 600.0f, 350.0f ), "Press ESC to return to Main Menu", 20.0f, Rgba::YELLOW, fixedFont );
	}
	else if ( GetState() == STATE_GENERATING )
	{
		g_theRenderer->DrawText2D( Vector2( 700.0f, 55.0f ), "Press SPACE to proceed 1 step of generation", 20.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 700.0f, 30.0f ), "Press ENTER to Play", 20.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 700.0f, 5.0f ), "Press ESC to return to Quest Selection", 20.0f, Rgba::YELLOW, fixedFont );
	}
	else if ( GetState() == STATE_PLAYING )
	{
		
		g_theRenderer->DrawText2D( Vector2( 0.0f, 875.0f ), "Press ESC to return to Quest Selection", 20.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 850.0f ), "Show Map (M)", 20.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 825.0f ), "God Mode (G)", 20.0f, Rgba::YELLOW, fixedFont );
		g_theRenderer->DrawText2D( Vector2( 0.0f, 800.0f ), "Save Game (F6)", 20.0f, Rgba::YELLOW, fixedFont );

		if ( m_player == nullptr )
		{
			g_theRenderer->DrawText2D( Vector2( 1400.0f, 850.0f ), "Game Over", 20.0f, Rgba::RED, fixedFont );
			g_theRenderer->DrawText2D( Vector2( 1400.0f, 825.0f ), Stringf( "Turns: %i", m_playerTurns ), 20.0f, Rgba::YELLOW, fixedFont );
			g_theRenderer->DrawText2D( Vector2( 1400.0f, 800.0f ), Stringf( "Kills: %i", m_playerKills ), 20.0f, Rgba::YELLOW, fixedFont );
		}

		DrawMessageBar();

	}
	else if ( GetState() == STATE_SHUT_DOWN )
	{
		g_theRenderer->DrawText2D( Vector2( 0.0f, 825.0f ), "Closes application after 2 seconds...", 20.0f, Rgba::YELLOW, fixedFont );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawMessageBar() const
{
	const float leftOffset = 900.0f;
	const float bottomYPosition = 0.0f;
	const float bottomYOffset = 20.0f;
	const float messageScale = 15.0f;
	const int maxMessageAmount = 6;
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

	int length = m_messages.size();
	Message thisMessageData;

	Message dataFifth;
	Message dataFourth;
	Message dataThird;
	Message dataSecond;
	Message dataFirst;

	dataFifth.messageStr = "";
	dataFourth.messageStr = "";
	dataThird.messageStr = "";
	dataSecond.messageStr = "";
	dataFirst.messageStr = "";

	for ( int i = 0; i < maxMessageAmount; i++ )
	{
		if ( length <= i )
		{
			break;
		}
		thisMessageData = m_messages.at( length - ( i + 1 ) );
		g_theRenderer->DrawText2D( Vector2( leftOffset, bottomYPosition + ( bottomYOffset * i ) ), 
			thisMessageData.messageStr, messageScale, thisMessageData.messageColor, fixedFont );
	}
}


//-----------------------------------------------------------------------------------------------
// Finds starting position for player, looping from the bottom to the top of the first column
// and proceeding to subsequent columns as needed to find an air cell that doesn't already
// contain an agent, item, or feature
bool TheGame::SetPlayerStartPosition( Player* player )
{
	int cellIndex = 0;

	for ( int cellX = 0; cellX < m_map->m_mapWidth; cellX++ )
	{
		for ( int cellY = 0; cellY < m_map->m_mapHeight; cellY++ )
		{
			if ( m_map->m_cells[ cellIndex ].m_currentType == CELL_TYPE_AIR )
			{
				if ( m_map->m_cells[ cellIndex ].m_containsAgent == false &&
					m_map->m_cells[ cellIndex ].m_containsItem == false &&
					m_map->m_cells[ cellIndex ].m_containsFeature == false )
				{
					player->m_position = m_map->m_cells[ cellIndex ].m_position;
					m_map->m_cells[ cellIndex ].m_containsAgent = true;
					m_map->m_cells[ cellIndex ].m_containedAgent = player;
					player->UpdateMapCellVisibilityBasedUponPlayerFoV();
					return true;
				}
			}

			cellIndex++;
		}
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
IntVector2 TheGame::GetPlayerStartPosition( Map* map )
{
	int cellIndex = 0;

	for ( int cellX = 0; cellX < map->m_mapWidth; cellX++ )
	{
		for ( int cellY = 0; cellY < map->m_mapHeight; cellY++ )
		{
			if ( map->m_cells[ cellIndex ].m_currentType == CELL_TYPE_AIR )
			{
				if ( map->m_cells[ cellIndex ].m_containsAgent == false )
				{
					return map->m_cells[ cellIndex ].m_position;
				}
			}

			cellIndex++;
		}
	}

	return IntVector2( -1, -1 );
}


//-----------------------------------------------------------------------------------------------
void TheGame::PrintMessage( const std::string& message, const Rgba& color )
{
	if ( m_player )
	{
		Message newMessage;
		newMessage.messageStr = message + " [Turn: " + std::to_string( m_player->m_numTurns ) + "]";
		newMessage.messageColor = color;
		m_messages.push_back( newMessage );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::CleanUpDeadEntities()
{
	std::vector< GameEntity* > entitiesToRemove;

	for ( auto entityIter = m_entities.begin(); entityIter != m_entities.end(); ++entityIter )
	{
		GameEntity* entity = *entityIter;
		if ( !entity->IsAlive() )
		{
			// Pull player stats before deleting player
			if ( entity->m_type == GAME_ENTITY_PLAYER )
			{
				m_playerTurns = m_player->m_numTurns;
				// m_playerKills = m_player->m_numKills; #TODO: Perhaps use static cast in Agent to set this
			}

			// Print death message
			PrintMessage( entity->m_name + " has died!" );

			// Add to entitiesToRemove
			entitiesToRemove.push_back( entity );

			// Remove from TurnOrderMap
			for ( auto agentIter = m_turnOrderMap.begin(); agentIter != m_turnOrderMap.end(); ++agentIter )
			{
				Agent* agent = agentIter->second;
				if ( agent == entity )
				{
					m_turnOrderMap.erase( agentIter );
					break;
				}
			}

			// Dereference from agents
			for ( auto agentIter = m_entities.begin(); agentIter != m_entities.end(); ++agentIter )
			{
				GameEntity* agent = *agentIter;
				UNUSED( agent );
			}

			// Remove from associated map cell
			m_map->RemoveAgentAtCellPosition( entity->m_position );

			// Remove from m_player pointer
			if ( m_player == entity )
			{
				m_player = nullptr;
			}
		}
	}

	// Drop any items held on death
	for ( GameEntity* deadEntity : entitiesToRemove )
	{
		deadEntity->DropItemsOnDeath();
	}

	// Delete dead agents
	for ( auto agentIter = m_entities.begin(); agentIter != m_entities.end(); )
	{
		GameEntity* agent = *agentIter;
		if ( !agent->IsAlive() )
		{
			delete agent;
			agent = nullptr;
			agentIter = m_entities.erase( agentIter );
		}
		else
		{
			++agentIter;
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool TheGame::IsMapValid( Map* map )
{
	IntVector2 startPosition = GetPlayerStartPosition( map );

	unsigned int numberOfPathsToGenerate = 10;

	for ( unsigned int pathNumber = 0; pathNumber < numberOfPathsToGenerate; ++pathNumber )
	{
		IntVector2 endPosition = map->GetRandomAirCellPosition();

		Pathfinder* testPathfinder = new Pathfinder( startPosition, endPosition, map, nullptr );

		bool foundPath = false;

		while ( !testPathfinder->m_isFinished )
		{
			foundPath = testPathfinder->TakeOneStep();
		}

		delete testPathfinder;
		testPathfinder = nullptr;

		if ( foundPath == false )
		{
			return false;
		}
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
void TheGame::RemoveGameEntity( GameEntity* entityToRemove )
{
	// NOTE: DO NOT delete the entity. It will delete itself on "Use".
	for ( unsigned int i = 0; i < m_entities.size(); i++ )
	{
		if ( m_entities[ i ] == entityToRemove )
		{
			m_entities.erase( m_entities.begin() + i );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::SaveGame()
{
	XMLNode rootSaveNode = XMLNode();
	rootSaveNode = rootSaveNode.createXMLTopNode( "SaveGame" );

	XMLNode mapData = m_map->SaveToXML();
	XMLNode entityData = SaveEntities();

	rootSaveNode.addChild( mapData );
	rootSaveNode.addChild( entityData );

	if ( rootSaveNode.writeToFile( "Data/Saves/Perceptron.Save.xml" ) == 0 )
	{
		g_theGame->PrintMessage( "Game saved.", Rgba::GREEN );
	}
	else
	{
		g_theGame->PrintMessage( "Game save failed.", Rgba::RED );
	}
}


//-----------------------------------------------------------------------------------------------
XMLNode TheGame::SaveEntities()
{
	XMLNode entityDataNode = XMLNode();
	entityDataNode = entityDataNode.createXMLTopNode( "EntityData" );

	for ( GameEntity* entity : m_entities )
	{
		if ( entity->IsPlayer() )
		{

			XMLNode playerDataNode = m_player->SaveToXML();

			entityDataNode.addChild( playerDataNode );
		}
	}

	return entityDataNode;
}