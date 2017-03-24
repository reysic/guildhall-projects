#pragma once


//-----------------------------------------------------------------------------------------------
enum GameState
{
	STATE_MAIN_MENU,
	STATE_PLAYING,
	NUM_GAME_STATES
};


//-----------------------------------------------------------------------------------------------
const int MAX_TILE_COUNT = 54;
const int MAX_POWER_UP_COUNT = 54;
const std::string STICK_LEFT_TEXTURE_FILE = "Data/Images/StickLeft.png";
const std::string BUTTON_A_TEXTURE_FILE = "Data/Images/ButtonA.png";


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
class PlayerBall;
class PowerUp;
class Tile;


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
	void Update( float deltaSeconds );
	void Render() const;
	void DrawGrid() const;
	void SpawnTiles();
	void SpawnPowerUps();
	void CheckTilesForCollisions();
	void CheckForTileDestroy( Tile* tile );
	void CheckPowerUpsForCollisions();
	void DestroyPowerUp( PowerUp* powerUp );

	// Accessors
	GameState GetGameState() const { return m_currentState; }

	// Mutators
	bool SetGameState( GameState newState );

public:
	int				m_turnNumber;

private:
	PlayerBall*		m_playerBall;
	Tile*			m_tiles[ MAX_TILE_COUNT ];
	PowerUp*		m_powerUps[ MAX_POWER_UP_COUNT ];
	GameState		m_currentState;
};