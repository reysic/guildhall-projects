#pragma once

#include "Engine/Math/Vector2.hpp"


//-----------------------------------------------------------------------------------------------
enum GameState
{
	STATE_MAIN_MENU,
	STATE_PLAYING,
	NUM_GAME_STATES
};


//-----------------------------------------------------------------------------------------------
const int MAX_PLAYER_BALL_COUNT = 100;
const int MAX_TILE_COUNT = 54;
const int MAX_POWER_UP_COUNT = 54;
const int MAX_EMITTER_COUNT = 54;
const int MAX_INDICATOR_BALL_COUNT = 54;
const int NUM_STARTING_BALLS = 1;
const float PLAYER_BALL_SPEED = 1000.0f;
const float TILE_SPAWN_CHANCE = 0.5f;
const float POWER_UP_SPAWN_CHANCE = 0.1f;
const float PLAYER_BALL_SHOOT_RATE_BALLS_PER_SEC = 10.0f;
const Vector2 PLAYER_BALL_INITIAL_POSITION( 350.0f, 10.0f );


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
class PlayerBall;
class PowerUp;
class Tile;
class Emitter;
class IndicatorBall;
class AimLine;
class Neuroevolution;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	// Constructors & Destructors
	TheGame();
	~TheGame();

	// Methods
	void Input();
	void HandleKeyboardInput();
	void HandleControllerInput();
	void Update( float deltaSeconds );
	void UpdatePlayerBalls( float deltaSeconds );
	void UpdateTiles( float deltaSeconds );
	void UpdateEmitters( float deltaSeconds );
	void UpdateIndicatorBalls( float deltaSeconds );
	void UpdateAimLine();
	void DeleteDeadEmitters();
	void UpdateFPSCounter( float deltaSeconds );
	void Render() const;
	void RenderMainMenuState() const;
	void RenderPlayingState() const;
	void RenderDebugInfo() const;
	void DrawGrid() const;
	void SpawnPlayerBall();
	void SpawnMore();
	void SpawnTile( int xPosition );
	void SpawnPowerUp( int xPosition );
	void SpawnTileDestoryEmitter( Vector2 emitterLocation );
	void SpawnPowerUpIndicatorEmitter( Vector2 emitterLocation );
	void CheckTilesForCollisions();
	void BounceRight( PlayerBall* ball, Tile* tile );
	void BounceLeft( PlayerBall* ball, Tile* tile );
	void BounceTop( PlayerBall* ball, Tile* tile );
	void BounceBottom( PlayerBall* ball, Tile* tile );
	void HitTile( Tile* tile );
	void CheckForTileDestroy( Tile* tile );
	void CheckPowerUpsForCollisions();
	void DestroyPowerUp( PowerUp* powerUp );
	void SpawnPowerUpIndicatorBall( const Vector2& spawnLocation );
	void AdvanceTurn();
	void CondenseBalls();
	void SetBallsToNotShotThisTurn();
	void SpawnBallsAtEndOfTurn();
	void DestroyIndicatorBalls();
	void AdvanceTiles();
	void AdvancePowerUps();
	void EndGame();
	void Reset();
	bool AreAllPlayerBallsNotMoving() const;
	bool IsAPlayerBallNotMoving();
	void ShootPlayerBall();
	int GetNumBalls() const;
	int GetNumMovingBalls() const;
	Vector2 SetNewPlayerBallStartingPosition( const Vector2& positionOfBallAsking );

	// Accessors & Mutators
	GameState GetGameState() const { return m_currentState; }
	bool SetGameState( GameState newState );

private:
	// Methods
	void InitializeArrays();
	void DeinitializeArrays();

	// Members
	PlayerBall*		m_playerBalls[ MAX_PLAYER_BALL_COUNT ];
	Tile*			m_tiles[ MAX_TILE_COUNT ];
	PowerUp*		m_powerUps[ MAX_POWER_UP_COUNT ];
	Emitter*		m_emitters[ MAX_EMITTER_COUNT ];
	IndicatorBall*	m_indicatorBalls[ MAX_INDICATOR_BALL_COUNT ];
	AimLine*		m_aimLine;
	int				m_turnNumber;
	int				m_highestTurnNumber;
	GameState		m_currentState;
	bool			m_canDrawAimLine;
	bool			m_waitToShootPlayerBall;
	bool			m_canShootBall;
	Vector2			m_forwardDirection;
	float			m_timeBetweenBallShotsSec;
	float			m_timeLastBallShotSec;

	// Neuroevolution Members
	Neuroevolution*					m_neuroevolution;
	std::vector< NeuralNetwork* >	m_generation;
	int								m_currentGeneration;
};