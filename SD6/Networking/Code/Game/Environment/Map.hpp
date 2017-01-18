#pragma once

#include <vector>
#include <set>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Cameras/Camera.hpp"
#include "Engine/Renderer/Particles/Emitter.hpp"
#include "Game/Entities/Player.hpp"
#include "Game/Environment/Tile.hpp"
#include "Game/Effects/Explosion.hpp"
#include "Game/Entities/Enemy.hpp"
#include "Game/Entities/Star.hpp"
#include "Game/AI/Path.hpp"


//-----------------------------------------------------------------------------------------------
// Forward Declarations
class Map;
class SpriteSheet;


//-----------------------------------------------------------------------------------------------
// Externs
extern Map* g_theMap;


//-----------------------------------------------------------------------------------------------
// Constants
const int MAX_EXPLOSION_COUNT = 1024;
const int MAX_EMITTER_COUNT = 1024;
const int MAX_STAR_COUNT = 1024;


//-----------------------------------------------------------------------------------------------
typedef IntVector2 TileCoords;
typedef Vector2 WorldCoords;
typedef int TileIndex;
class Map
{
public:
	// Constructors & Destructors
	Map( const TileCoords& mapSizeInTiles );
	~Map();

	// Methods
	void Update( float deltaSeconds );
	void UpdateExplosions( float deltaSeconds );
	void UpdateEmitters( float deltaSeconds );
	void DeleteFinishedExplosions();
	void DeleteDeadEmitters();
	void UpdateStars( float deltaSeconds );
	void DeleteDeadStars();
	void Render();
	void RenderExplosions();
	void RenderEmitters();
	void RenderStars();
	void RenderEntities();
	void RenderGrid();
	void RenderTiles();
	bool HasLineOfSight( const WorldCoords& start, const WorldCoords& end );
	void DeleteDeadEntities();
	void CheckSeekEnemyCount();
	void CheckPathEnemyCount();
	void CheckWanderEnemyCount();
	void CheckPursueEnemyCount();
	void SpawnExplosion( Vector2 explosionLocation, float sizeScalingFactor, float durationInSeconds );
	void SpawnEmitter( Vector2 emitterLocation );
	void SpawnPlayerBullet( const WorldCoords& spawningPlayerPosition, float spawnOrientationDegrees,
		float spawningEnemyCosmeticRadius );
	void SpawnEnemyBullet( const WorldCoords& spawningEnemyPosition, float spawnOrientationDegrees,
		float spawningEnemyCosmeticRadius );
	void Restart();

	// Accessors
	Player* GetPlayer() const { return m_player; }
	TileCoords GetTileCoordsForWorldCoords( const WorldCoords& worldCoords );
	TileIndex GetIndexForTileCoords( int tileX, int tileY );
	TileIndex GetIndexForTileCoords( const TileCoords& tileCoords );

	// Members
	TileCoords m_mapSizeInTiles;
	std::vector< Tile > m_tiles;
	std::set< Entity* > m_entities;
	Star* m_stars[ MAX_STAR_COUNT ];
	Explosion* m_explosions[ MAX_EXPLOSION_COUNT ];
	Emitter* m_emitters[ MAX_EMITTER_COUNT ];
	Path* m_path;

private:
	// Methods
	void InitializeArrays();
	void InitializeTiles();
	void PopulateMap();
	void SpawnSeekEnemies();
	void SpawnPathEnemies();
	void SpawnWanderEnemies();
	void SpawnPursueEnemies();
	void SpawnStar();
	void DoCorrectivePhysicsEntityVsEntity( Entity& entity );
	void DoCorrectivePhysicsEntityVsTiles( Entity& entity );
	void DoPhysicsProjectilesVsTiles( Entity& entity );
	void DoPhysicsProjectilesVsEntities( Entity& entity );
	void PushEntityOutOfTile( Entity& entity, WorldCoords entityWorldCoords, Vector2 tileCorner, 
		float distToCorner );
	bool IsTileSolid( TileIndex index );

	// Accessors
	Tile& GetTileAtIndex( TileIndex tileIndex );
	TileCoords GetTileCoordsForIndex( TileIndex index );
	WorldCoords GetWorldCoordsForTileCenter( TileIndex index );
	Vector2 GetWorldMinsForTile( TileIndex index );
	Vector2 GetWorldMaxsForTile( TileIndex index );

	// Members
	Player* m_player;
	int m_numTiles;
	Tile m_prototypeGrassTile;
	Tile m_prototypeStoneTile;
	int m_numAliveSeekEnemies;
	int m_numAlivePathEnemies;
	int m_numAliveWanderEnemies;
	int m_numAlivePursueEnemies;
	int m_numAlivePickups;
};