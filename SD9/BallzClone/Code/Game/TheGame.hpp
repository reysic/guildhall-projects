#pragma once


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
const float PLAYER_BALL_SPEED = 750.0f;
const float TILE_SPAWN_CHANCE = 0.5f;
const float POWER_UP_SPAWN_CHANCE = 0.1f;
const float PLAYER_BALL_SHOOT_RATE_BALLS_PER_SEC = 10.0f;
const Vector2 PLAYER_BALL_INITIAL_POSITION( 350.0f, 10.0f );
const std::string STICK_LEFT_TEXTURE_FILE = "Data/Images/StickLeft.png";
const std::string BUTTON_A_TEXTURE_FILE = "Data/Images/ButtonA.png";


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
class PlayerBall;
class PowerUp;
class Tile;
class Emitter;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	// Constructors & Destructors
	TheGame();

	// Methods
	void Input();
	void HandleKeyboardInput();
	void HandleControllerInput();
	void Update( float deltaSeconds );
	void UpdatePlayerBalls( float deltaSeconds );
	void UpdateTiles( float deltaSeconds );
	void UpdateEmitters( float deltaSeconds );
	void DeleteDeadEmitters();
	void UpdateFPSCounter( float deltaSeconds );
	void Render() const;
	void RenderMainMenuState() const;
	void RenderPlayingState() const;
	void RenderDebugInfo() const;
	void DrawGrid() const;
	void DrawAimLine() const;
	void SpawnPlayerBall();
	void SpawnMore();
	void SpawnTile( int xPosition );
	void SpawnPowerUp( int xPosition );
	void SpawnEmitter( Vector2 emitterLocation );
	void CheckTilesForCollisions();
	void CheckForTileDestroy( Tile* tile );
	void CheckPowerUpsForCollisions();
	void DestroyPowerUp( PowerUp* powerUp );
	void AdvanceTurn();
	void AdvanceTiles();
	void AdvancePowerUps();
	void EndGame();
	void Reset();
	bool AreAllPlayerBallsNotMoving();
	bool IsAPlayerBallNotMoving();
	void ShootPlayerBall();
	int GetNumBalls() const;

	// Accessors
	GameState GetGameState() const { return m_currentState; }

	// Mutators
	bool SetGameState( GameState newState );
	
	// Members
	int				m_turnNumber;

private:
	// Methods
	void InitializeArrays();

	// Members
	PlayerBall*		m_playerBalls[ MAX_PLAYER_BALL_COUNT ];
	Tile*			m_tiles[ MAX_TILE_COUNT ];
	PowerUp*		m_powerUps[ MAX_POWER_UP_COUNT ];
	Emitter*		m_emitters[ MAX_EMITTER_COUNT ];
	GameState		m_currentState;
	bool			m_canDrawAimLine;
	bool			m_waitToShootPlayerBall;
	bool			m_canShootBall;
	Vector2			m_forwardDirection;
	float			m_timeBetweenBallShotsSec;
	float			m_timeLastBallShotSec;
};