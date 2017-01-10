//-----------------------------------------------------------------------------------------------
// Map.hpp
// 2D tile-based map for in-game level


//-----------------------------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------------------------
#include <vector>
#include <set>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/PlayerTank.hpp"
#include "Game/Camera2D.hpp"
#include "Game/Tile.hpp"
#include "Game/Explosion.hpp"


//-----------------------------------------------------------------------------------------------
class SpriteSheet;


//-----------------------------------------------------------------------------------------------
typedef IntVector2 TileCoords;
typedef Vector2 WorldCoords;
typedef int TileIndex;


//-----------------------------------------------------------------------------------------------
extern Map* g_theMap;


//-----------------------------------------------------------------------------------------------
const std::string TILE_SPRITE_SHEET_FILE = "Data/Images/Tiles.png";
const std::string PAUSE_SCREEN_TEXTURE_FILE = "Data/Images/Pause.png";
const int MAX_NUM_INTERIOR_STONE_TILES = 200;
const int MAX_NUM_TURRETS = 10;
const int MAX_NUM_ENEMY_TANKS = 10;
const int NUM_PADDING_TILES = 10;
const WorldCoords PLAYER_TANK_START_POSITION = WorldCoords( 10.5f, 10.5f );
const float PLAYER_TANK_FIRING_RATE_BULLETS_PER_SECOND = 8.0f;
const std::string PLAYER_TANK_SHOOT_SOUND_FILE = "Data/Audio/GrenadeLauncher_Shot01.wav";
const std::string EXPLOSION_SOUND_FILE = "Data/Audio/Explo_Grenade1.wav";


//-----------------------------------------------------------------------------------------------
class Map
{
public:
	// Constructors & Destructors
	Map( const TileCoords& mapSizeInTiles );
	~Map();

	// Methods
	void Input();
	void Update( float deltaSeconds );
	void Render();
	void SpawnTurretBullet( Entity* turret);
	void SpawnEnemyTankBullet( Entity* enemyTank );
	void RespawnPlayerTank();
	bool HasLineOfSight( const WorldCoords& start, const WorldCoords& end );
	PlayerTank* GetPlayerTank() const;
	void SpawnExplosion( Vector2 explosionLocation, float sizeScalingFactor, float durationInSeconds );

	// Members
	std::vector< Tile > m_tiles;
	std::set< Entity* > m_entities;
	std::vector< Explosion* > m_explosions;

private:
	// Methods
	//TileCoords GetTileCoordsForTile( const Tile& tile ) const;
	//Tile* GetTileAtCoords( const TileCoords& tileCoords );
	// above does a check and returns a nullptr if coordinates are 
	// out-of-bounds
	void InitializeTiles(); // Sets all tilesÅEm_tilesCoords, and initializes them all to the default tile type (e.g. GRASS).
	void PopulateMap(); // Places stone and other stuff, wherever you want (around the edge, in the middle, etc).
	void DoCorrectivePhysicsEntityVsEntity( Entity& entity );
	void DoCorrectivePhysicsEntityVsTiles( Entity& entity );
	void DoPhysicsProjectilesVsTiles( Entity& entity );
	void DoPhysicsProjectilesVsEntities( Entity& entity);
	void PushEntityOutOfTile( Entity& entity, WorldCoords entityWorldCoords, Vector2 tileCorner, float distToCorner );
	Tile& GetTileAtIndex( TileIndex tileIndex );
	// you could do the above by reference since you're specifying
	// the index
	//Tile* GetTileAtWorldCoords( const WorldCoords& worldCoords );
	TileCoords GetTileCoordsForWorldCoords( const WorldCoords& worldCoords );
	TileCoords GetTileCoordsForIndex( TileIndex index );
	TileIndex GetIndexForTileCoords( int tileX, int tileY );
	TileIndex GetIndexForTileCoords( const TileCoords& tileCoords );
	WorldCoords GetWorldCoordsForTileCenter( TileIndex index );
	Vector2 GetWorldMinsForTile( TileIndex index );
	Vector2 GetWorldMaxsForTile( TileIndex index );
	bool IsTileSolid( TileIndex index );
	void SpawnPlayerBullet();

	// Members
	TileCoords m_mapSizeInTiles;
	PlayerTank* m_playerTank;
	Camera2D* m_camera;
	int m_numTiles;
	bool m_waitToSpawnNewPlayerBullet;
	Tile m_prototypeGrassTile;
	Tile m_prototypeStoneTile;
	float m_timeBetweenPlayerBullets;
	float m_timeLastPlayerBulletFired;
	SpriteSheet* m_tileSpriteSheet;
};