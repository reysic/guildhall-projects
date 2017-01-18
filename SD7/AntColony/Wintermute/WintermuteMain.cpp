#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library

#include "../Arena/include/CommonInterface.hpp"
#include "Math/ShortVector2.hpp"
#include "Entities/Ant.hpp"
#include "Environment/Map.hpp"
#include "Time/Time.hpp"

#include <vector>


//-----------------------------------------------------------------------------------------------
// Globals
AgentReports g_threadSharedAgentReports;
ObservedEntities g_threadSharedObservedEntities;
ArenaSquares g_threadSharedObservedArenaSquares;
Orders g_threadSharedOrders;

AgentReports g_internalCopyAgentReports;
ObservedEntities g_internalCopyObservedEntities;
ArenaSquares g_internalCopyObservedArenaSquares;

CRITICAL_SECTION cs_threadSharedData;


//-----------------------------------------------------------------------------------------------
// Forward declarations of functions
void ProcessNewUpdateInformation();
void DetermineAndPopulateOrders();
void DetermineAndPopulateSoldierOrder( AgentReport agentReport );
void DetermineAndPopulateScoutOrder( AgentReport agentReport );
void DetermineAndPopulateWorkerOrder( AgentReport agentReport );
void DetermineAndPopulateQueenOrder( AgentReport agentReport );
OrderCode GetRandomMovementOrder( short antPositionX, short antPositionY );
void StoreOffQueensIDAndLocateSelfOnMap();
void DebugPrintIfBlockedByRock( AgentReport agentReport );
bool DoesCurrentCellContainFood( short antPositionX, short antPositionY );
ShortVector2 GetQueensPosition();
OrderCode GetMovementOrderTowardGoalPosition( Ant* forThisAnt, const ShortVector2& goalPosition );
OrderCode GetMovementOrderTowardGoalPositionUsingComputePath( Ant* forThisAnt, const ShortVector2& goalPosition );
void AddOrderThreadSafe( AgentOrder& order );
void UpdateAntVectorContents();
void UpdateCurrentAntTypeCounts();
void UpdateUpkeepAndEstimatedTurns();
Ant* FindAntWithID( int id );
void RemoveAntWithID( int id );
void UpdateEnemyQueenLocation();
void UpdateMap();
bool DidThisAntJustDie( AgentReport agentReport );
void DrawAntPaths();
void HighlightCellsChangedToStone();
void DrawFilledSquare( const ShortVector2& squarePosition );
void AddFoodToFoodMap( const ShortVector2& foodPosition );
void RemoveFoodFromFoodMap( const ShortVector2& foodPosition );
ShortVector2 GetClosestUnclaimedFoodFromFoodMap( Ant* forThisAnt );
void UpdateFoodClaims();
void AssignGuardToQueen();
ShortVector2 GetRandomInteriorNonSolidCellCoordsWithinRadiusFromPosition( int radius, const ShortVector2& position );


//-----------------------------------------------------------------------------------------------
// Constants
const int LARGE_MAP_WIDTH_THRESHOLD = 100;
const int MAX_SCOUTS_TO_SPAWN = 0;
const int MAX_WORKERS_TO_SPAWN = 39;
const int MAX_SOLDIERS_TO_SPAWN = 99;
const int NUM_SOLDIERS_TO_SEND_TO_OPPOSITE_SIDE = 3;
const int NUM_SCOUTS_TO_SEND_TO_OPPOSITE_SIDE = 3;
const int MIN_NUTRIENT_BIRTH_THRESHOLD = 25;
const int MIN_TURNS_REMAINING_BEFORE_SPAWN_WORKER = 50;
const float PATH_RADIUS_FOR_LARGE_MAPS = 80.0f;
const bool ADD_RANDOM_OFFSET_TO_OPPOSITE_SIDE_CALC = false;
const bool SPAWN_SOLDIER_WHEN_QUEEN_IS_ATTACKED = false;
const bool GUARD_QUEEN = true;
const bool QUEEN_MOVES = false;


//-----------------------------------------------------------------------------------------------
// Globals
float g_longestPathTimeMilliseconds = 0.0f;
bool g_isQuitting = false;
bool g_printDebug = false;
bool g_spawnedOnRightSideOfMap = false;
bool g_haveAssignedScoutToOppositeSide = false;
bool g_haveAssignedSoldierToOppositeSide = false;
bool g_canSeeEnemyQueen = false;
bool g_wasQueenJustAttacked = false;
int g_turnNumber = 0;
int g_currentNutrients = 0;
int g_queenID = -1;
int g_queenGuardID = -1;
int g_currentScouts = 0;
int g_currentWorkers = 0;
int g_currentSoldiers = 0;
int g_myPlayerID = 0;
int g_upkeepForThisTurn = 0;
int g_estimatedNumberOfTurnsRemaining = 0;
int g_turnMyAntsWereLastAttacked = 0;
int g_numPlayers = 0;
int g_numSoldiersSentToOppositeSide = 0;
int g_numScoutsSentToOppositeSide = 0;
ShortVector2 g_enemyQueenLocation( -1, -1 );
ShortVector2 g_myQueenLocation( -1, -1 );

std::vector< Ant* > g_vectorOfAnts;
Map* g_theMap;


//-----------------------------------------------------------------------------------------------
int GetInterfaceVersion()
{
	return COMMON_INTERFACE_VERSION_NUMBER;
}


//-----------------------------------------------------------------------------------------------
const char* GetPlayerName()
{
	return "Wintermute 1.47";
}


//-----------------------------------------------------------------------------------------------
const char* GetAuthorName()
{
	return "Jeremy Hicks";
}


//-----------------------------------------------------------------------------------------------
void PrepareForBattle( int playerID, const ArenaInfo& arenaInfo )
{
	InitializeCriticalSection( &cs_threadSharedData );

	g_myPlayerID = playerID;
	g_numPlayers = arenaInfo.numPlayers;
	g_theMap = new Map( arenaInfo.width, arenaInfo.height );

	if ( g_printDebug )
	{
		short sizeInBytesOfMapCell = sizeof( MapCell );
		DebuggerPrintf( "%s%i%s", "sizeof( MapCell ) = ", sizeInBytesOfMapCell, " bytes\n" );
		short sizeInBytesOfCellType = sizeof( CellType );
		DebuggerPrintf( "%s%i%s", "sizeof( CellType ) = ", sizeInBytesOfCellType, " bytes\n" );
	}
}


//-----------------------------------------------------------------------------------------------
void BeginWork()
{
	while ( !g_isQuitting )
	{
		// Have your own version of the data structures here to work with
		EnterCriticalSection( &cs_threadSharedData );
		bool areNewResultsReadyToProcess = ( g_threadSharedAgentReports.m_numberAgentReports > 0 );
		LeaveCriticalSection( &cs_threadSharedData );

		if ( areNewResultsReadyToProcess )
		{
			EnterCriticalSection( &cs_threadSharedData );
			g_internalCopyAgentReports = g_threadSharedAgentReports;
			g_internalCopyObservedEntities = g_threadSharedObservedEntities;
			g_internalCopyObservedArenaSquares = g_threadSharedObservedArenaSquares;
			g_threadSharedAgentReports.m_numberAgentReports = 0;
			LeaveCriticalSection( &cs_threadSharedData );

			ProcessNewUpdateInformation();
			DetermineAndPopulateOrders();

			if ( g_printDebug )
			{
				DebuggerPrintf( "%s%i%s%.04f%s", "Turn ", g_turnNumber, " Longest ComputePath took ", g_longestPathTimeMilliseconds, " ms\n" );
			}
		}

		Sleep( 0 );
	}
}


//-----------------------------------------------------------------------------------------------
void ProcessNewUpdateInformation()
{
	// Store off Queen's ID if we haven't already
	if ( g_queenID == -1 )
	{
		StoreOffQueensIDAndLocateSelfOnMap();
	}

	UpdateAntVectorContents();
	UpdateEnemyQueenLocation();
	UpdateMap();
	UpdateFoodClaims();
	AssignGuardToQueen();
}


//-----------------------------------------------------------------------------------------------
void DetermineAndPopulateOrders()
{
	for ( int agentReportIndex = 0; agentReportIndex < g_internalCopyAgentReports.m_numberAgentReports; ++agentReportIndex )
	{
		AgentReport thisAgentReport = g_internalCopyAgentReports.m_agentReports[ agentReportIndex ];
		
		DebugPrintIfBlockedByRock( thisAgentReport );

		switch ( thisAgentReport.m_agentType )
		{
			case ENTITY_TYPE_SOLDIER:
				DetermineAndPopulateSoldierOrder( thisAgentReport );
				break;
			case ENTITY_TYPE_SCOUT:
				DetermineAndPopulateScoutOrder( thisAgentReport );
				break;
			case ENTITY_TYPE_WORKER:
				DetermineAndPopulateWorkerOrder( thisAgentReport );
				break;
			case ENTITY_TYPE_QUEEN:
				DetermineAndPopulateQueenOrder( thisAgentReport );
				break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DetermineAndPopulateSoldierOrder( AgentReport agentReport )
{
	Ant* thisAnt = FindAntWithID( agentReport.m_entityID );

	if ( thisAnt == nullptr )
	{
		return;
	}

	if ( thisAnt->m_id == g_queenGuardID && GUARD_QUEEN )
	{
		AgentOrder order;
		order.m_entityID = agentReport.m_entityID;
		order.m_orderCode = GetMovementOrderTowardGoalPosition( thisAnt, g_myQueenLocation );
		AddOrderThreadSafe( order );
		return;
	}

	if ( ( g_enemyQueenLocation != ShortVector2( -1, -1 ) ) && ( g_numPlayers == 2 ) )
	{
		// We have eyes on an enemy queen, so move toward it
		AgentOrder order;
		order.m_entityID = agentReport.m_entityID;
		order.m_orderCode = GetMovementOrderTowardGoalPosition( thisAnt, g_enemyQueenLocation );
		AddOrderThreadSafe( order );
		return;
	}

	if ( g_canSeeEnemyQueen )
	{
		// We have eyes on an enemy queen, so move toward it
		AgentOrder order;
		order.m_entityID = agentReport.m_entityID;
		order.m_orderCode = GetMovementOrderTowardGoalPosition( thisAnt, g_enemyQueenLocation );
		AddOrderThreadSafe( order );
		return;
	}

	if ( thisAnt->m_position == thisAnt->m_goalPosition )
	{
		// This ant has reached its goal position
		thisAnt->m_goalPosition = ShortVector2( -1, -1 );
	}

	if ( g_theMap->GetCellTypeForCoords( thisAnt->m_goalPosition.x, thisAnt->m_goalPosition.y ) == CELL_TYPE_STONE )
	{
		// This ant has an invalid goal position
		thisAnt->m_goalPosition = ShortVector2( -1, -1 );
	}

	if ( thisAnt->m_goalPosition == ShortVector2( -1, -1 ) )
	{
		// This ant does not currently have a goal position
		if ( ( g_numSoldiersSentToOppositeSide < NUM_SOLDIERS_TO_SEND_TO_OPPOSITE_SIDE ) && ( g_numPlayers == 2 ) )
		{
			if ( g_spawnedOnRightSideOfMap )
			{
				if ( ADD_RANDOM_OFFSET_TO_OPPOSITE_SIDE_CALC )
				{
					thisAnt->m_goalPosition = ShortVector2( 1, g_theMap->m_height / 2 + ( rand() % 11 - 5 ) );
					thisAnt->m_activePath.clear();
				}
				else
				{
					thisAnt->m_goalPosition = ShortVector2( 1, g_theMap->m_height / 2 );
					thisAnt->m_activePath.clear();
				}
			}
			else
			{
				if ( ADD_RANDOM_OFFSET_TO_OPPOSITE_SIDE_CALC )
				{
					thisAnt->m_goalPosition = ShortVector2( g_theMap->m_width - 2, ( short ) ( g_theMap->m_height / 2 ) + ( rand() % 11 - 5 ) );
					thisAnt->m_activePath.clear();
				}
				else
				{
					thisAnt->m_goalPosition = ShortVector2( g_theMap->m_width - 2, ( short ) ( g_theMap->m_height / 2 ) );
					thisAnt->m_activePath.clear();
				}
			}
			g_numSoldiersSentToOppositeSide++;
		}
		else
		{
			// Path to random locations
			if ( g_theMap->m_width >= LARGE_MAP_WIDTH_THRESHOLD )
			{
				// Large map, so avoid picking a really far away goal location to reduce
				// time to compute path
				thisAnt->m_goalPosition = GetRandomInteriorNonSolidCellCoordsWithinRadiusFromPosition( ( int ) PATH_RADIUS_FOR_LARGE_MAPS, thisAnt->m_position );
				thisAnt->m_activePath.clear();
			}
			else
			{
				thisAnt->m_goalPosition = g_theMap->GetRandomInteriorNonSolidCellCoords();
				thisAnt->m_activePath.clear();
			}
		}
	}

	AgentOrder order;
	order.m_entityID = agentReport.m_entityID;
	order.m_orderCode = GetMovementOrderTowardGoalPosition( thisAnt, thisAnt->m_goalPosition );
	AddOrderThreadSafe( order );
}


//-----------------------------------------------------------------------------------------------
void DetermineAndPopulateScoutOrder( AgentReport agentReport )
{
	Ant* thisAnt = FindAntWithID( agentReport.m_entityID );

	if ( thisAnt == nullptr )
	{
		return;
	}
	
	if ( thisAnt->m_position == thisAnt->m_goalPosition )
	{
		// This ant has reached its goal position
		thisAnt->m_goalPosition = ShortVector2( -1, -1 );
	}

	if ( g_theMap->GetCellTypeForCoords( thisAnt->m_goalPosition.x, thisAnt->m_goalPosition.y ) == CELL_TYPE_STONE )
	{
		// This ant has an invalid goal position
		thisAnt->m_goalPosition = ShortVector2( -1, -1 );
	}

	if ( thisAnt->m_goalPosition == ShortVector2( -1, -1 ) )
	{
		// This ant does not currently have a goal position
		if ( g_canSeeEnemyQueen )
		{
			// Harass the enemy queen if visible
			thisAnt->m_goalPosition = g_enemyQueenLocation;
			thisAnt->m_activePath.clear();
		}
		else
		{
			if ( ( g_numScoutsSentToOppositeSide < NUM_SCOUTS_TO_SEND_TO_OPPOSITE_SIDE ) && ( g_numPlayers == 2 ) )
			{
				if ( g_spawnedOnRightSideOfMap )
				{
					if ( ADD_RANDOM_OFFSET_TO_OPPOSITE_SIDE_CALC )
					{
						thisAnt->m_goalPosition = ShortVector2( 1, g_theMap->m_height / 2 + ( rand() % 11 - 5 ) );
						thisAnt->m_activePath.clear();
					}
					else
					{
						thisAnt->m_goalPosition = ShortVector2( 1, g_theMap->m_height / 2 );
						thisAnt->m_activePath.clear();
					}
				}
				else
				{
					if ( ADD_RANDOM_OFFSET_TO_OPPOSITE_SIDE_CALC )
					{
						thisAnt->m_goalPosition = ShortVector2( g_theMap->m_width - 2, ( short ) ( g_theMap->m_height / 2 ) + ( rand() % 11 - 5 ) );
						thisAnt->m_activePath.clear();
					}
					else
					{
						thisAnt->m_goalPosition = ShortVector2( g_theMap->m_width - 2, ( short ) g_theMap->m_height / 2 );
						thisAnt->m_activePath.clear();
					}
				}
				g_numScoutsSentToOppositeSide++;
			}
			else
			{
				// Path to random locations
				if ( g_theMap->m_width >= LARGE_MAP_WIDTH_THRESHOLD )
				{
					// Large map, so avoid picking a really far away goal location to reduce
					// time to compute path
					thisAnt->m_goalPosition = GetRandomInteriorNonSolidCellCoordsWithinRadiusFromPosition( ( int ) PATH_RADIUS_FOR_LARGE_MAPS, thisAnt->m_position );
					thisAnt->m_activePath.clear();
				}
				else
				{
					thisAnt->m_goalPosition = g_theMap->GetRandomInteriorNonSolidCellCoords();
					thisAnt->m_activePath.clear();
				}
			}
		}
	}
	
	AgentOrder order;
	order.m_entityID = agentReport.m_entityID;
	order.m_orderCode = GetMovementOrderTowardGoalPosition( thisAnt, thisAnt->m_goalPosition );
	AddOrderThreadSafe( order );
}


//-----------------------------------------------------------------------------------------------
void DetermineAndPopulateWorkerOrder( AgentReport agentReport )
{
	if ( agentReport.m_specialStatus != ENTITY_SPECIAL_STATUS_CARRYING_FOOD )
	{
		// This ant is not currently carrying any food

		// Clear any stored path
		Ant* thisAnt = FindAntWithID( agentReport.m_entityID );

		if ( thisAnt == nullptr )
		{
			return;
		}

		thisAnt->m_activePath.clear();

		// If not carrying food, check if current square contains food
		if ( DoesCurrentCellContainFood( agentReport.m_newPositionX, agentReport.m_newPositionY ) )
		{
			// Pick up the food
			AgentOrder order;
			order.m_entityID = agentReport.m_entityID;
			order.m_orderCode = ORDER_TAKE_FOOD;
			AddOrderThreadSafe( order );

			// #TODO: Confirm ant actually picked up food before removing from food map
			RemoveFoodFromFoodMap( ShortVector2( agentReport.m_newPositionX, agentReport.m_newPositionY ) );
		}
		else
		{
			ShortVector2 closestFood = GetClosestUnclaimedFoodFromFoodMap( thisAnt );
			if ( closestFood != ShortVector2( -1, -1 ) )
			{
				// Found a food to path to
				OrderCode movementOrder = GetMovementOrderTowardGoalPosition( thisAnt, closestFood );
				AgentOrder order;
				order.m_entityID = agentReport.m_entityID;
				order.m_orderCode = movementOrder;
				AddOrderThreadSafe( order );
			}
			else
			{
				// Didn't find a food to path to, so wander randomly
				AgentOrder order;
				order.m_entityID = agentReport.m_entityID;
				order.m_orderCode = GetRandomMovementOrder( agentReport.m_newPositionX, agentReport.m_newPositionY );
				AddOrderThreadSafe( order );
			}
		}
	}
	else
	{
		// This ant is currently carrying food

		// Take food to the queen
		Ant* thisAnt = FindAntWithID( agentReport.m_entityID );

		if ( thisAnt == nullptr )
		{
			return;
		}

		ShortVector2 queensPosition = GetQueensPosition();
		OrderCode tentativeMovementOrder = GetMovementOrderTowardGoalPosition( thisAnt, queensPosition );
		if ( tentativeMovementOrder != ORDER_HOLD )
		{
			AgentOrder order;
			order.m_entityID = agentReport.m_entityID;
			order.m_orderCode = tentativeMovementOrder;
			AddOrderThreadSafe( order );
		}
		else
		{
			AgentOrder order;
			order.m_entityID = agentReport.m_entityID;
			order.m_orderCode = ORDER_DROP_FOOD;
			AddOrderThreadSafe( order );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DetermineAndPopulateQueenOrder( AgentReport agentReport )
{
	if ( g_estimatedNumberOfTurnsRemaining > MIN_NUTRIENT_BIRTH_THRESHOLD )
	{
		// On a small map and have enough nutrients to birth
		if ( g_wasQueenJustAttacked && SPAWN_SOLDIER_WHEN_QUEEN_IS_ATTACKED )
		{
			AgentOrder order;
			order.m_entityID = agentReport.m_entityID;
			order.m_orderCode = ORDER_CREATE_SOLDIER;
			AddOrderThreadSafe( order );
			return;
		}
		else if ( g_currentWorkers < MAX_WORKERS_TO_SPAWN
			&& g_turnNumber < 1000
			&& g_theMap->m_foods.size() > 0
			&& g_estimatedNumberOfTurnsRemaining < MIN_TURNS_REMAINING_BEFORE_SPAWN_WORKER )
		{
			AgentOrder order;
			order.m_entityID = agentReport.m_entityID;
			order.m_orderCode = ORDER_CREATE_WORKER;
			AddOrderThreadSafe( order );
			return;
		}
		else if ( g_currentScouts < MAX_SCOUTS_TO_SPAWN )
		{
			AgentOrder order;
			order.m_entityID = agentReport.m_entityID;
			order.m_orderCode = ORDER_CREATE_SCOUT;
			AddOrderThreadSafe( order );
			return;
		}
		else if ( g_currentSoldiers < MAX_SOLDIERS_TO_SPAWN 
			&& g_theMap->m_foods.size() > 5 )
		{
			AgentOrder order;
			order.m_entityID = agentReport.m_entityID;
			order.m_orderCode = ORDER_CREATE_SOLDIER;
			AddOrderThreadSafe( order );
			return;
		}
		else
		{
			AgentOrder order;
			order.m_entityID = agentReport.m_entityID;
			if ( QUEEN_MOVES )
			{
				order.m_orderCode = GetRandomMovementOrder( agentReport.m_newPositionX, agentReport.m_newPositionY );
			}
			else
			{
				order.m_orderCode = ORDER_HOLD;
			}
			AddOrderThreadSafe( order );
			return;
		}
	}
	else
	{
		// Not enough nutrients to birth
		AgentOrder order;
		order.m_entityID = agentReport.m_entityID;
		if ( QUEEN_MOVES )
		{
			order.m_orderCode = GetRandomMovementOrder( agentReport.m_newPositionX, agentReport.m_newPositionY );
		}
		else
		{
			order.m_orderCode = ORDER_HOLD;
		}
		AddOrderThreadSafe( order );
		return;
	}
}


//-----------------------------------------------------------------------------------------------
OrderCode GetRandomMovementOrder( short antPositionX, short antPositionY )
{
	OrderCode tentativeMovementCode = NUM_ORDERS;

	while ( tentativeMovementCode == NUM_ORDERS )
	{
		int randInt0To3 = rand() % 4;
		switch ( randInt0To3 )
		{
			case 0:
				tentativeMovementCode = ORDER_MOVE_NORTH;
				break;
			case 1:
				tentativeMovementCode = ORDER_MOVE_SOUTH;
				break;
			case 2:
				tentativeMovementCode = ORDER_MOVE_EAST;
				break;
			case 3:
				tentativeMovementCode = ORDER_MOVE_WEST;
				break;
			default:
				DebuggerPrintf( "%s", "Default case hit in GetRandomMovementOrder()" );
				tentativeMovementCode = NUM_ORDERS;
				break;
		}

		switch ( tentativeMovementCode )
		{
			case ORDER_MOVE_NORTH:
				if ( g_theMap->GetCellTypeForCoords( antPositionX, antPositionY - 1 ) == CELL_TYPE_STONE )
				{
					tentativeMovementCode = NUM_ORDERS;
				}
				break;
			case ORDER_MOVE_SOUTH:
				if ( g_theMap->GetCellTypeForCoords( antPositionX, antPositionY + 1 ) == CELL_TYPE_STONE )
				{
					tentativeMovementCode = NUM_ORDERS;
				}
				break;
			case ORDER_MOVE_EAST:
				if ( g_theMap->GetCellTypeForCoords( antPositionX + 1, antPositionY ) == CELL_TYPE_STONE )
				{
					tentativeMovementCode = NUM_ORDERS;
				}
				break;
			case ORDER_MOVE_WEST:
				if ( g_theMap->GetCellTypeForCoords( antPositionX - 1, antPositionY ) == CELL_TYPE_STONE )
				{
					tentativeMovementCode = NUM_ORDERS;
				}
				break;
			default:
				DebuggerPrintf( "%s", "Default case hit in GetRandomMovementOrder()" );
				tentativeMovementCode = NUM_ORDERS;
				break;
		}
	}

	return tentativeMovementCode;
}


//-----------------------------------------------------------------------------------------------
void StoreOffQueensIDAndLocateSelfOnMap()
{
	for ( int agentReportIndex = 0; agentReportIndex < g_internalCopyAgentReports.m_numberAgentReports; ++agentReportIndex )
	{
		AgentReport thisAgentReport = g_internalCopyAgentReports.m_agentReports[ agentReportIndex ];

		if ( thisAgentReport.m_agentType == ENTITY_TYPE_QUEEN )
		{
			g_queenID = thisAgentReport.m_entityID;

			if ( thisAgentReport.m_newPositionX > ( g_theMap->m_width / 2 ) )
			{
				// Spawned on right side of map
				g_spawnedOnRightSideOfMap = true;
			}
			else
			{
				// Spawned on left side of map
				g_spawnedOnRightSideOfMap = false;
			}

			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void DebugPrintIfBlockedByRock( AgentReport agentReport )
{
	if ( agentReport.m_reportCode == REPORT_ERROR_BLOCKED_BY_ROCK && g_printDebug )
	{
		std::string antIDAsString = std::to_string( agentReport.m_entityID );
		DebuggerPrintf( "%s%s%s", "Ant ", antIDAsString.c_str(), " attempted to move into rock square.\n" );
	}
}


//-----------------------------------------------------------------------------------------------
bool DoesCurrentCellContainFood( short antPositionX, short antPositionY )
{
	if ( g_theMap->GetCellTypeForCoords( antPositionX, antPositionY ) == CELL_TYPE_FOOD )
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
ShortVector2 GetQueensPosition()
{
	Ant* foundAnt = FindAntWithID( g_queenID );

	if ( foundAnt != nullptr )
	{
		return ShortVector2( foundAnt->m_position );
	}
	else
	{
		// Queen not found, return invalid position
		return ShortVector2( -1, -1 );
	}
}


//-----------------------------------------------------------------------------------------------
OrderCode GetMovementOrderTowardGoalPosition( Ant* forThisAnt, const ShortVector2& goalPosition )
{
	if ( forThisAnt->m_position == goalPosition )
	{
		return ORDER_HOLD;
	}

	if ( forThisAnt->m_activePath.size() > 0 )
	{
		// This ant already has a path

		// Verify that path does not contain stone
		for ( unsigned int vecIndex = 0; vecIndex < forThisAnt->m_activePath.size(); ++vecIndex )
		{
			ShortVector2 pathPositionAtThisIndex = forThisAnt->m_activePath[ vecIndex ];
			CellType cellTypeAtThisPathPosition = g_theMap->GetCellTypeForCoords( pathPositionAtThisIndex.x, pathPositionAtThisIndex.y );
			if ( cellTypeAtThisPathPosition == CELL_TYPE_STONE )
			{
				return GetMovementOrderTowardGoalPositionUsingComputePath( forThisAnt, goalPosition );
			}
		}

		// Verify that the ending position of the path still matches the goal position
		if ( forThisAnt->m_activePath[ 0 ] != goalPosition )
		{
			return GetMovementOrderTowardGoalPositionUsingComputePath( forThisAnt, goalPosition );
		}

		// Find index of position along path
		int indexOfPosition = 0;
		for ( unsigned int vecIndex = 0; vecIndex < forThisAnt->m_activePath.size(); ++vecIndex )
		{
			ShortVector2 pathPositionAtThisIndex = forThisAnt->m_activePath[ vecIndex ];
			if ( forThisAnt->m_position == pathPositionAtThisIndex )
			{
				indexOfPosition = vecIndex;
				break;
			}
		}

		if ( indexOfPosition == 0 )
		{
			return ORDER_HOLD;
		}

		ShortVector2 nextPositionOnPath = forThisAnt->m_activePath[ indexOfPosition - 1 ];
		if ( forThisAnt->m_position.x < nextPositionOnPath.x )
		{
			return ORDER_MOVE_EAST;
		}
		else if ( forThisAnt->m_position.x > nextPositionOnPath.x )
		{
			return ORDER_MOVE_WEST;
		}
		else if ( forThisAnt->m_position.y < nextPositionOnPath.y )
		{
			return ORDER_MOVE_SOUTH;
		}
		else if ( forThisAnt->m_position.y > nextPositionOnPath.y )
		{
			return ORDER_MOVE_NORTH;
		}
		else
		{
			return ORDER_HOLD;
		}
	}
	else
	{
		return GetMovementOrderTowardGoalPositionUsingComputePath( forThisAnt, goalPosition );
	}
}


//-----------------------------------------------------------------------------------------------
OrderCode GetMovementOrderTowardGoalPositionUsingComputePath( Ant* forThisAnt, const ShortVector2& goalPosition )
{
	int numCells = g_theMap->m_width * g_theMap->m_height;
	CellCoordsList pathToGoal;
	pathToGoal.reserve( numCells );

	float timeAtStartOfComputePath = ( float ) GetCurrentTimeSeconds();
	g_theMap->ComputePath( forThisAnt->m_type, forThisAnt->m_position, goalPosition, pathToGoal );
	forThisAnt->m_activePath = pathToGoal;
	float timeAtEndOfComputePath = ( float ) GetCurrentTimeSeconds();
	float timeToComputePathMilliseconds = ( timeAtEndOfComputePath - timeAtStartOfComputePath ) * 1000.0f;

	if ( timeToComputePathMilliseconds > g_longestPathTimeMilliseconds )
	{
		g_longestPathTimeMilliseconds = timeToComputePathMilliseconds;
	}

	if ( g_printDebug )
	{
		switch ( forThisAnt->m_type )
		{
			case ENTITY_TYPE_SOLDIER:
			{
				DebuggerPrintf( "%s%i%s%.04f%s", "Turn ", g_turnNumber, " ComputePath for soldier took ", timeToComputePathMilliseconds, " ms\n" );
				break;
			}
			case ENTITY_TYPE_SCOUT:
			{
				DebuggerPrintf( "%s%i%s%.04f%s", "Turn ", g_turnNumber, " ComputePath for scout took ", timeToComputePathMilliseconds, " ms\n" );
				break;
			}
			case ENTITY_TYPE_WORKER:
			{
				DebuggerPrintf( "%s%i%s%.04f%s", "Turn ", g_turnNumber, " ComputePath for worker took ", timeToComputePathMilliseconds, " ms\n" );
				break;
			}
			case ENTITY_TYPE_QUEEN:
			{
				DebuggerPrintf( "%s%i%s%.04f%s", "Turn ", g_turnNumber, " ComputePath for queen took ", timeToComputePathMilliseconds, " ms\n" );
				break;
			}
			default:
			{
				DebuggerPrintf( "%s%i%s%.04f%s", "Turn ", g_turnNumber, " ComputePath for unknown any type took ", timeToComputePathMilliseconds, " ms\n" );
				break;
			}
		}
	}

	if ( pathToGoal.empty() )
	{
		// #TODO: Revise
		// Couldn't find a path, so mark this cell as stone
		unsigned short cellIndex = g_theMap->GetCellIndexForCoords( goalPosition.x, goalPosition.y );
		CellType currentCellType = g_theMap->GetCellTypeForIndex( cellIndex );
		if ( currentCellType == CELL_TYPE_FOOD )
		{
			RemoveFoodFromFoodMap( goalPosition );
		}
		g_theMap->ChangeCellType( cellIndex, CELL_TYPE_STONE );
		g_theMap->m_cellsChangedToStone.push_back( goalPosition );
		return ORDER_HOLD;
	}

	ShortVector2 positionOfCellToMoveTo = pathToGoal[ pathToGoal.size() - 2 ];
	if ( forThisAnt->m_position.x < positionOfCellToMoveTo.x )
	{
		return ORDER_MOVE_EAST;
	}
	else if ( forThisAnt->m_position.x > positionOfCellToMoveTo.x )
	{
		return ORDER_MOVE_WEST;
	}
	else if ( forThisAnt->m_position.y < positionOfCellToMoveTo.y )
	{
		return ORDER_MOVE_SOUTH;
	}
	else if ( forThisAnt->m_position.y > positionOfCellToMoveTo.y )
	{
		return ORDER_MOVE_NORTH;
	}
	else
	{
		return ORDER_HOLD;
	}
}


//-----------------------------------------------------------------------------------------------
void AddOrderThreadSafe( AgentOrder& order )
{
	EnterCriticalSection( &cs_threadSharedData );
	g_threadSharedOrders.AddOrder( order.m_entityID, order.m_orderCode, true );
	LeaveCriticalSection( &cs_threadSharedData );
}


//-----------------------------------------------------------------------------------------------
void UpdateAntVectorContents()
{
	for ( int agentReportIndex = 0; agentReportIndex < g_internalCopyAgentReports.m_numberAgentReports; ++agentReportIndex )
	{
		AgentReport thisAgentReport = g_internalCopyAgentReports.m_agentReports[ agentReportIndex ];

		// Check if the agent is the queen and if she was just attacked
		if ( thisAgentReport.m_entityID == g_queenID )
		{
			if ( thisAgentReport.m_reportCode == REPORT_QUEEN_WAS_ASSAULTED )
			{
				g_wasQueenJustAttacked = true;
			}

			// Update my queen's location
			g_myQueenLocation = ShortVector2( thisAgentReport.m_newPositionX, thisAgentReport.m_newPositionY );
		}

		Ant* foundAnt = FindAntWithID( thisAgentReport.m_entityID );

		if ( foundAnt == nullptr )
		{
			// This ant is not currently in the vector of ants
			foundAnt = new Ant( thisAgentReport.m_entityID );
			foundAnt->m_type = ( EntityType ) thisAgentReport.m_agentType;
			foundAnt->m_position = ShortVector2( thisAgentReport.m_newPositionX, thisAgentReport.m_newPositionY );

			// Add this ant to the vector of ants
			g_vectorOfAnts.push_back( foundAnt );
		}
		else
		{
			// This ant is already in the vector of ants

			bool didThisAntJustDie = DidThisAntJustDie( thisAgentReport );

			if ( didThisAntJustDie )
			{
				RemoveAntWithID( thisAgentReport.m_entityID );
			}
			else
			{
				foundAnt->m_position = ShortVector2( thisAgentReport.m_newPositionX, thisAgentReport.m_newPositionY );
			}
		}
	}

	UpdateCurrentAntTypeCounts();
	UpdateUpkeepAndEstimatedTurns();
}


//-----------------------------------------------------------------------------------------------
void UpdateCurrentAntTypeCounts()
{
	g_currentScouts = 0;
	g_currentWorkers = 0;
	g_currentSoldiers = 0;

	for ( unsigned int vecIndex = 0; vecIndex < g_vectorOfAnts.size(); ++vecIndex )
	{
		switch ( g_vectorOfAnts[ vecIndex ]->m_type )
		{
			case ENTITY_TYPE_SOLDIER:
			{
				++g_currentSoldiers;
				break;
			}
			case ENTITY_TYPE_SCOUT:
			{
				++g_currentScouts;
				break;
			}
			case ENTITY_TYPE_WORKER:
			{
				++g_currentWorkers;
				break;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void UpdateUpkeepAndEstimatedTurns()
{
	g_upkeepForThisTurn = 8; // For the queen
	g_upkeepForThisTurn += g_currentWorkers;
	g_upkeepForThisTurn += g_currentSoldiers * 2;

	g_estimatedNumberOfTurnsRemaining = g_currentNutrients / g_upkeepForThisTurn;
}


//-----------------------------------------------------------------------------------------------
Ant* FindAntWithID( int id )
{
	Ant* foundAnt = nullptr;

	for ( unsigned int vecIndex = 0; vecIndex < g_vectorOfAnts.size(); vecIndex++ )
	{
		if ( g_vectorOfAnts[ vecIndex ]->m_id == id )
		{
			foundAnt = g_vectorOfAnts[ vecIndex ];
			break;
		}
	}

	return foundAnt;
}


//-----------------------------------------------------------------------------------------------
void RemoveAntWithID( int id )
{
	for ( unsigned int vecIndex = 0; vecIndex < g_vectorOfAnts.size(); vecIndex++ )
	{
		if ( g_vectorOfAnts[ vecIndex ]->m_id == id )
		{
			g_vectorOfAnts.erase( g_vectorOfAnts.begin() + vecIndex );
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void UpdateEnemyQueenLocation()
{
	g_canSeeEnemyQueen = false;

	for ( int observedEntityIndex = 0; observedEntityIndex < g_internalCopyObservedEntities.m_numberOfObservedEntities; ++observedEntityIndex )
	{
		ObservedEntity observedEntity = g_internalCopyObservedEntities.m_entities[ observedEntityIndex ];

		if ( observedEntity.m_agentType == ENTITY_TYPE_QUEEN )
		{
			// This is an enemy queen
			g_enemyQueenLocation = ShortVector2( observedEntity.m_positionX, observedEntity.m_positionY );
			g_canSeeEnemyQueen = true;
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void UpdateMap()
{
	int numArenaSquares = g_theMap->m_width * g_theMap->m_height;
	for ( int arenaSquareIndex = 0; arenaSquareIndex < numArenaSquares; ++arenaSquareIndex )
	{
		switch ( g_internalCopyObservedArenaSquares.m_arenaSquareMaterials[ arenaSquareIndex ] )
		{
			case 0: // ARENA_SQUARE_TYPE_AIR
			{
				CellType currentCellType = g_theMap->GetCellTypeForIndex( ( unsigned short ) arenaSquareIndex );
				if ( currentCellType != CELL_TYPE_STONE )
				{
					g_theMap->ChangeCellType( ( unsigned short ) arenaSquareIndex, CELL_TYPE_AIR );
				}
				break;
			}
			case 1: // ARENA_SQUARE_TYPE_DIRT
			{
				CellType currentCellType = g_theMap->GetCellTypeForIndex( ( unsigned short ) arenaSquareIndex );
				if ( currentCellType != CELL_TYPE_STONE )
				{
					g_theMap->ChangeCellType( ( unsigned short ) arenaSquareIndex, CELL_TYPE_DIRT );
				}
				break;
			}
			case 2: // ARENA_SQUARE_TYPE_STONE
			{
				g_theMap->ChangeCellType( ( unsigned short ) arenaSquareIndex, CELL_TYPE_STONE );
				break;
			}
			case 3: // ARENA_SQUARE_TYPE_FOOD
			{
				CellType currentCellType = g_theMap->GetCellTypeForIndex( ( unsigned short ) arenaSquareIndex );
				if ( currentCellType != CELL_TYPE_STONE )
				{
					g_theMap->ChangeCellType( ( unsigned short ) arenaSquareIndex, CELL_TYPE_FOOD );
					AddFoodToFoodMap( g_theMap->GetCellCoordsForIndex( ( unsigned short ) arenaSquareIndex ) );
				}
				break;
			}
			case 4: // ARENA_SQUARE_TYPE_UNKNOWN
			{
//				g_theMap->ChangeCellType( arenaSquareIndex, CELL_TYPE_UNKNOWN );
				// Never change squares to the CELL_TYPE_UNKNOWN
				break;
			}
			default:
			{
				g_theMap->ChangeCellType( ( unsigned short ) arenaSquareIndex, CELL_TYPE_STONE );
				break;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool DidThisAntJustDie( AgentReport agentReport )
{
	switch ( agentReport.m_reportCode )
	{
		case REPORT_SUICIDE_SUCCESSFUL:
		case REPORT_WAS_KILLED_IN_COMBAT:
		case REPORT_STARVED_TO_DEATH:
			return true;
			break;
		default:
			return false;
			break;
	}
}


//-----------------------------------------------------------------------------------------------
void SendUpdate( int turnNumber, int currentNutrients, const AgentReports& agentReports, const ObservedEntities& observedEntities, const ArenaSquares& observedSquares )
{
	g_turnNumber = turnNumber;
	g_currentNutrients = currentNutrients;

	EnterCriticalSection( &cs_threadSharedData );
	g_threadSharedAgentReports = agentReports;
	g_threadSharedObservedEntities = observedEntities;
	g_threadSharedObservedArenaSquares = observedSquares;
	LeaveCriticalSection( &cs_threadSharedData );
}


//-----------------------------------------------------------------------------------------------
void FetchNewOrders( int turnNumber, Orders& newOrders_out )
{
	turnNumber;
	EnterCriticalSection( &cs_threadSharedData );
	newOrders_out = g_threadSharedOrders;
	g_threadSharedOrders.m_numberOfOrders = 0;
	LeaveCriticalSection( &cs_threadSharedData );
}


//-----------------------------------------------------------------------------------------------
void EndWork()
{
	g_isQuitting = true;
}


//-----------------------------------------------------------------------------------------------
void DrawDebugOverlay( short inspectSquareX, short inspectSquareY )
{
	EnterCriticalSection( &cs_threadSharedData );

	inspectSquareX;
	inspectSquareY;

	DrawAntPaths();
	HighlightCellsChangedToStone();

	LeaveCriticalSection( &cs_threadSharedData );
}


//-----------------------------------------------------------------------------------------------
void DrawAntPaths()
{
	for ( unsigned int vecIndex = 0; vecIndex < g_vectorOfAnts.size(); vecIndex++ )
	{
		if ( g_vectorOfAnts[ vecIndex ]->m_activePath.size() > 0 )
		{
			// This ant has a path stored on it
			CellCoordsList thisAntsPath = g_vectorOfAnts[ vecIndex ]->m_activePath;
			for ( unsigned int cellIndex = 0; cellIndex < thisAntsPath.size(); ++cellIndex )
			{
				glColor4f( 1.0f, 0.0f, 1.0f, 0.5f ); // Magenta
				ShortVector2 thisCellsCoords = thisAntsPath[ cellIndex ];
				DrawFilledSquare( thisCellsCoords );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void HighlightCellsChangedToStone()
{
	for ( unsigned short vecIndex = 0; vecIndex < g_theMap->m_cellsChangedToStone.size(); ++vecIndex )
	{
		glColor4f( 1.0f, 0.0f, 0.0f, 1.0f ); // Red
		ShortVector2 thisCellsCoords = g_theMap->m_cellsChangedToStone[ vecIndex ];
		DrawFilledSquare( thisCellsCoords );
	}
}


//-----------------------------------------------------------------------------------------------
void DrawFilledSquare( const ShortVector2& squarePosition )
{
	glBegin( GL_QUADS );
	{
		glVertex2f( squarePosition.x - 0.5f, squarePosition.y - 0.5f ); // Top Left
		glVertex2f( squarePosition.x + 0.5f, squarePosition.y - 0.5f ); // Top Right
		glVertex2f( squarePosition.x + 0.5f, squarePosition.y + 0.5f ); // Bottom Right
		glVertex2f( squarePosition.x - 0.5f, squarePosition.y + 0.5f ); // Bottom Left
	}
	glEnd();
}


//-----------------------------------------------------------------------------------------------
void AddFoodToFoodMap( const ShortVector2& foodPosition )
{
	for ( unsigned int vecIndex = 0; vecIndex < g_theMap->m_foods.size(); vecIndex++ )
	{
		Food foodAlreadyInMap = g_theMap->m_foods[ vecIndex ];
		if ( foodAlreadyInMap.position == foodPosition )
		{
			// This food has already been stored in the food map
			return;
		}
	}

	// Add the newly discovered food to the food map
	Food newFood;
	newFood.position = foodPosition;
	g_theMap->m_foods.push_back( newFood );
}


//-----------------------------------------------------------------------------------------------
void RemoveFoodFromFoodMap( const ShortVector2& foodPosition )
{
	for ( unsigned int vecIndex = 0; vecIndex < g_theMap->m_foods.size(); vecIndex++ )
	{
		Food foodAlreadyInMap = g_theMap->m_foods[ vecIndex ];
		if ( foodAlreadyInMap.position == foodPosition )
		{
			// Remove this food from the food map
			g_theMap->m_foods.erase( g_theMap->m_foods.begin() + vecIndex );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
ShortVector2 GetClosestUnclaimedFoodFromFoodMap( Ant* forThisAnt )
{
	// Returns ( -1, -1 ) if no food is in the food map, or a closer food is not found
	ShortVector2 closestFood( -1, -1 );
	float shortestManhattanDistance = 1000.0f;

	if ( g_theMap->m_foods.size() == 0 )
	{
		// There is no food in the food map
		return closestFood;
	}

	unsigned int indexOfClosestFood = 0;
	for ( unsigned int vecIndex = 0; vecIndex < g_theMap->m_foods.size(); vecIndex++ )
	{
		Food foodAtThisIndex = g_theMap->m_foods[ vecIndex ];
		float manhattanDistanceToThisFood = g_theMap->GuessEstimatedCost( forThisAnt->m_position, foodAtThisIndex.position );
		if ( manhattanDistanceToThisFood < shortestManhattanDistance && ( foodAtThisIndex.idOfAntWithClaim == -1 
			|| foodAtThisIndex.idOfAntWithClaim == forThisAnt->m_id ) )
		{
			// This is the closest unclaimed food thus far
			closestFood = g_theMap->m_foods[ vecIndex ].position;
			shortestManhattanDistance = manhattanDistanceToThisFood;
			indexOfClosestFood = vecIndex;
		}
	}

	// Verify before returning the position that it actually contains food
	if ( DoesCurrentCellContainFood( ( short ) closestFood.x, ( short ) closestFood.y ) )
	{
		// Claim the food
		g_theMap->m_foods[ indexOfClosestFood ].idOfAntWithClaim = forThisAnt->m_id;
		return closestFood;
	}
	else
	{
		RemoveFoodFromFoodMap( closestFood );
		closestFood = ShortVector2( -1, -1 );
		return closestFood;
	}
}


//-----------------------------------------------------------------------------------------------
void UpdateFoodClaims()
{
	for ( unsigned int vecIndex = 0; vecIndex < g_theMap->m_foods.size(); vecIndex++ )
	{
		Food foodAtThisIndex = g_theMap->m_foods[ vecIndex ];
		int idOfAntThatHasClaimedThisFood = foodAtThisIndex.idOfAntWithClaim;

		if ( idOfAntThatHasClaimedThisFood != -1 )
		{
			Ant* antThatHasClaimedThisFood = FindAntWithID( idOfAntThatHasClaimedThisFood );

			if ( antThatHasClaimedThisFood == nullptr )
			{
				// This ant has died, so remove its food claim
				g_theMap->m_foods[ vecIndex ].idOfAntWithClaim = -1;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void AssignGuardToQueen()
{
	int newQueenGuardID = -1;

	if ( g_queenGuardID == -1 || FindAntWithID( g_queenGuardID ) == nullptr )
	{
		// The queen does not currently have a guard

		for ( unsigned int vecIndex = 0; vecIndex < g_vectorOfAnts.size(); ++vecIndex )
		{
			Ant* thisAnt = g_vectorOfAnts[ vecIndex ];
			if ( thisAnt->m_type == ENTITY_TYPE_SOLDIER )
			{
				newQueenGuardID = thisAnt->m_id;
			}
		}
	}

	if ( newQueenGuardID != -1 )
	{
		g_queenGuardID = newQueenGuardID;
	}
}


//-----------------------------------------------------------------------------------------------
ShortVector2 GetRandomInteriorNonSolidCellCoordsWithinRadiusFromPosition( int radius, const ShortVector2& position )
{
	// Loop forever until we find an interior (non-edge) location which is not solid
	for ( ;; )
	{
		short cellX = ( short ) GetRandomIntInRange( position.x - radius, position.x + radius );
		short cellY = ( short ) GetRandomIntInRange( position.y - radius, position.y + radius );
		ShortVector2 cellCoords( cellX, cellY );

		float distanceFromPosition = g_theMap->GuessEstimatedCost( position, cellCoords );
		if ( distanceFromPosition < radius )
		{
			if ( !g_theMap->IsCellSolid( cellCoords ) )
				return cellCoords;
		}
	}
}