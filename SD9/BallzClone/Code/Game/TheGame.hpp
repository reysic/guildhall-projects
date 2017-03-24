#pragma once


//-----------------------------------------------------------------------------------------------
const int MAX_TILE_COUNT = 54;
const int MAX_POWER_UP_COUNT = 54;

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
	TheGame();
	void Input();
	void Update( float deltaSeconds );
	void Render() const;
	void DrawGrid() const;
	void SpawnTiles();
	void SpawnPowerUps();
	void CheckTilesForCollisions();
	void CheckPowerUpsForCollisions();
	void DestroyPowerUp( PowerUp* powerUp );
	bool IsPlayerDiskAlive() const;
	Vector2 GetPlayerDiskPosition() const;
	bool IsQuitting();

public:
	int				m_turnNumber;

private:
	PlayerBall*		m_playerBall;
	Tile*			m_tiles[ MAX_TILE_COUNT ];
	PowerUp*		m_powerUps[ MAX_POWER_UP_COUNT ];
};