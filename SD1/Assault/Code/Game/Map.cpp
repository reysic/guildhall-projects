//-----------------------------------------------------------------------------------------------
// Map.cpp
// 2D tile-based map for in-game level


//-----------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerBullet.hpp"
#include "Game/Turret.hpp"
#include "Game/EnemyTank.hpp"
#include "Game/EnemyBullet.hpp"
#include "Game/Explosion.hpp"


//-----------------------------------------------------------------------------------------------
Map* g_theMap = nullptr;


//-----------------------------------------------------------------------------------------------
Map::Map( const TileCoords& mapSizeInTiles )
	: m_mapSizeInTiles( mapSizeInTiles )
	, m_camera( new Camera2D() )
	, m_waitToSpawnNewPlayerBullet( false )
	, m_timeBetweenPlayerBullets( 1.0f / PLAYER_TANK_FIRING_RATE_BULLETS_PER_SECOND )
	, m_timeLastPlayerBulletFired( 0.0f )
	, m_tileSpriteSheet( new SpriteSheet( TILE_SPRITE_SHEET_FILE, 2, 1 ) )
{
	InitializeTiles();
	PopulateMap();
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	delete m_tileSpriteSheet;
	m_tileSpriteSheet = nullptr;
	delete m_camera;
	m_camera = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Map::Input()
{
	// Check for 'VK_SPACE' or Xbox 'Right Trigger', which spawns another player bullet
	if ( ( g_theApp->GetKeyDownStatus( VK_SPACE ) || g_theApp->GetXboxRightTriggerPulledStatus() ) && !m_waitToSpawnNewPlayerBullet && !( m_playerTank->GetIsDead() ) && !( m_playerTank->m_isCloaked ) )
	{
		SpawnPlayerBullet();
		m_waitToSpawnNewPlayerBullet = true;
		m_timeLastPlayerBulletFired = (float) GetCurrentTimeSeconds();
	}

	float currentTime = ( float ) GetCurrentTimeSeconds();
	float timeNextBulletCanBeFired = m_timeLastPlayerBulletFired + m_timeBetweenPlayerBullets;
	if ( m_waitToSpawnNewPlayerBullet && ( currentTime > timeNextBulletCanBeFired ) )
	{
		m_waitToSpawnNewPlayerBullet = false;
	}

	// Check for Xbox 'Left Trigger', which cloaks the tank
	if ( g_theApp->GetXboxLeftTriggerPulledStatus() && !( m_playerTank->GetIsDead() ) )
	{
		m_playerTank->m_isCloaked = true;
	}
	else
	{
		m_playerTank->m_isCloaked = false;
	}
}


//-----------------------------------------------------------------------------------------------
void Map::Update( float deltaSeconds )
{
	// Iterate through and update the entities
	std::set< Entity* >::iterator entitiesIterator;
	for ( entitiesIterator = m_entities.begin(); entitiesIterator != m_entities.end(); ++entitiesIterator )
	{
		Entity* entity = *entitiesIterator;
		if ( entity )
		{
			if ( entity->GetIsDead() && ( entity->m_type != ENTITY_TYPE_PLAYER_TANK ) )
			{
				if ( ( entity->m_type == ENTITY_TYPE_PLAYER_BULLET ) || ( entity->m_type == ENTITY_TYPE_ENEMY_BULLET ) )
				{
					g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( EXPLOSION_SOUND_FILE ), 0.1f );
					SpawnExplosion( entity->GetPosition(), 0.5f, 0.25f );
				}
				else
				{
					g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( EXPLOSION_SOUND_FILE ), 0.5f );
					SpawnExplosion( entity->GetPosition(), 1.25f, 0.5f );
				}
				delete entity;
				m_entities.erase( entitiesIterator );
				break;
			}

			entity->Update( deltaSeconds );

			if ( entity->UsesCorrectivePhysics() )
			{
				DoCorrectivePhysicsEntityVsEntity( *entity );
				DoCorrectivePhysicsEntityVsTiles( *entity );
			}

			if ( entity->IsProjectile() )
			{
				DoPhysicsProjectilesVsTiles( *entity );
				DoPhysicsProjectilesVsEntities( *entity );
			}
		}
		
	}

	// Iterate through and update the explosions
	std::vector< Explosion* >::iterator explosionIterator;
	for ( explosionIterator = m_explosions.begin(); explosionIterator != m_explosions.end(); explosionIterator++ )
	{
		Explosion* explosion = *explosionIterator;
		if ( explosion )
		{
			if ( explosion->IsFinished() )
			{
				delete explosion;
				m_explosions.erase( explosionIterator );
				break;
			}

			explosion->Update( deltaSeconds );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::Render()
{
	// Set up camera to follow player tank
	float numTilesInViewHorizontally = ( 16.0f / 9.0f ) * m_camera->GetNumTilesInViewVertically();
	Vector2 screenSize( numTilesInViewHorizontally, m_camera->GetNumTilesInViewVertically() );
	Vector2 cameraCenteredOnScreenPosition( screenSize.x * 0.5f, screenSize.y * 0.2f );
	m_camera->SetWorldPosition( m_playerTank->GetPosition() );
	m_camera->SetOrientationDegrees( m_playerTank->GetOrientationDegrees() - 90.0f );

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), screenSize );
	Vector3 positionAsVector3( cameraCenteredOnScreenPosition.x, cameraCenteredOnScreenPosition.y, 0.0f );
	g_theRenderer->TranslateView( positionAsVector3 );
	g_theRenderer->RotateView( -( m_camera->GetOrientationDegrees() ), Vector3( 0.0f, 0.0f, 1.0f ) );
	Vector3 worldPositionAsVector3( m_camera->GetWorldPosition().x, m_camera->GetWorldPosition().y, 0.0f );
	g_theRenderer->TranslateView( -( worldPositionAsVector3 ) );

	// Draw the tiles
	TileIndex tileIndex = 0;
	for ( tileIndex = 0; tileIndex < m_numTiles; tileIndex++)
	{
		AABB2 bounds = AABB2( Vector2( ( float ) m_tiles[ tileIndex ].m_tileCoords.x, ( float ) m_tiles[ tileIndex ].m_tileCoords.y ),
							  Vector2( ( float ) m_tiles[ tileIndex ].m_tileCoords.x + 1, ( float ) m_tiles[ tileIndex ].m_tileCoords.y + 1 ) );

		if ( m_tiles[ tileIndex ].m_type == TILE_TYPE_GRASS )
		{
			Rgba tint = Rgba( 0, 230, 0, 255 );

			Vector2 texCoordsMins = m_tileSpriteSheet->GetTexCoordsForSpriteIndex( 0 ).mins;
			Vector2 texCoordsMaxs = m_tileSpriteSheet->GetTexCoordsForSpriteIndex( 0 ).maxs;

			g_theRenderer->DrawTexturedAABB2( bounds, *Texture::CreateOrGetTexture( TILE_SPRITE_SHEET_FILE ), texCoordsMins, texCoordsMaxs, tint );
		}
		if ( m_tiles[ tileIndex ].m_type == TILE_TYPE_STONE )
		{
			Rgba tint = Rgba( 128, 128, 128, 255 );

			Vector2 texCoordsMins = m_tileSpriteSheet->GetTexCoordsForSpriteIndex( 1 ).mins;
			Vector2 texCoordsMaxs = m_tileSpriteSheet->GetTexCoordsForSpriteIndex( 1 ).maxs;
				
			g_theRenderer->DrawTexturedAABB2( bounds, *Texture::CreateOrGetTexture( TILE_SPRITE_SHEET_FILE ), texCoordsMins, texCoordsMaxs, tint );
		}
	}

	// Draw a grid
	Rgba gridColor( 0, 0, 0, 255 );

	float gridSize = ( float ) m_mapSizeInTiles.x;

	for ( float xy = 0.0f; xy < gridSize; xy += 1.0f )
	{
		g_theRenderer->DrawLine3D( Vector3( xy, 0.0f, 0.0f ), Vector3( xy, gridSize, 0.0f ), gridColor, 2.0f );
		g_theRenderer->DrawLine3D( Vector3( 0.0f, xy, 0.0f ), Vector3( gridSize, xy, 0.0f ), gridColor, 2.0f );
	}

	// Iterate through and draw the entities
	std::set< Entity* >::iterator entitiesIterator;
	for ( entitiesIterator = m_entities.begin(); entitiesIterator != m_entities.end(); ++entitiesIterator )
	{
		Entity* entity = *entitiesIterator;
		if ( entity )
		{
			// Debug mode rendering
			if ( g_theGame->GetDebugModeStatus() )
			{
				// Draw tinted, filled polygon using physics radius
				g_theRenderer->DrawFilledPolygon( 20, entity->GetPosition().x, entity->GetPosition().y, entity->GetPhysicsRadius(), entity->GetTint() );

				// Draw tinted, hollow polygon using cosmetic radius
				Rgba white( 255, 255, 255, 255 );
				g_theRenderer->DrawHollowPolygon( 20, entity->GetPosition().x, entity->GetPosition().y, entity->GetCosmeticRadius(), white );

				// Draw entity's forward direction vector
				float orientationRadians = ConvertDegreesToRadians( entity->GetOrientationDegrees() );
				Vector2 entityForwardDirection( cos( orientationRadians ), sin( orientationRadians ) );
				Vector2 normalizedEntityForwardDirection = entityForwardDirection.Normalize();

				Rgba black( 0, 0, 0, 255 );
				Vector3 entityPositionAsVector3( entity->GetPosition().x, entity->GetPosition().y, 0.0f );
				Vector3 entityForwardDirectionAsVector3( entityForwardDirection.x, entityForwardDirection.y, 0.0f );
				g_theRenderer->DrawLine3D( Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3 * entity->GetCosmeticRadius() ), Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3 * entity->GetCosmeticRadius() * 2.0f ), black, 2.0f );
			}
			// Normal rendering
			else
			{
				entity->Render();
			}
		}
	}

	// Iterate through and draw the explosions
	std::vector< Explosion* >::iterator explosionIterator;
	for ( explosionIterator = m_explosions.begin(); explosionIterator != m_explosions.end(); explosionIterator++ )
	{
		Explosion* explosion = *explosionIterator;
		if ( explosion )
		{
			explosion->Render();
		}
	}

	// Draw health bar
	if ( !( m_playerTank->GetIsDead() ) )
	{
		g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) );

		// Draw health bar background
		g_theRenderer->DrawQuad( Vector2( 598.0f, 23.0f ), Vector2( 1002.0f, 77.0f ), Rgba( 0, 0, 0, 255 ) );

		// Draw health bar
		float greenEnd = RangeMap( ( float ) m_playerTank->GetHealth(), 0.0f, (float) PLAYER_TANK_STARTING_HEALTH, 600.0f, 1000.0f );

		g_theRenderer->DrawQuad( Vector2( 600.0f, 25.0f ), Vector2( greenEnd, 75.0f ), Rgba( 0, 255, 0, 255 ) );
		g_theRenderer->DrawQuad( Vector2( greenEnd, 25.0f ), Vector2( 1000.0f, 75.0f ), Rgba( 255, 0, 0, 255 ) );
	}

	// Render pause screen if game is paused
	if ( g_theGame->GetPauseModeStatus() )
	{
		g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2 ( 1600.0f, 900.0f) );
		g_theRenderer->DrawQuad( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ), Rgba( 0, 0, 0, 77 ) );
		g_theRenderer->DrawTexturedAABB2( AABB2( Vector2( 0.0f, 0.0f ), Vector2( 1600.0f, 900.0f ) ), *Texture::CreateOrGetTexture( PAUSE_SCREEN_TEXTURE_FILE ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ),
			Rgba( 255, 255, 255, 255 ) );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::InitializeTiles()
{
	m_numTiles = m_mapSizeInTiles.x * m_mapSizeInTiles.y;
	m_tiles.resize( m_numTiles );

	// Create prototype tiles
	m_prototypeGrassTile.m_type = TILE_TYPE_GRASS;
	m_prototypeStoneTile.m_type = TILE_TYPE_STONE;

	// Build the map out of prototype grass tiles
	TileIndex tileIndex = 0;

	for ( int tileY = 0; tileY < m_mapSizeInTiles.y; tileY++ )
	{
		for ( int tileX = 0; tileX < m_mapSizeInTiles.x; tileX++ )
		{
			m_tiles[ tileIndex ] = m_prototypeGrassTile;
			m_tiles[ tileIndex ].m_tileCoords = IntVector2( tileX, tileY );
			m_tiles[ tileIndex ].m_containsEntity = false;

			tileIndex++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::PopulateMap()
{
	// Make edge tiles out of prototype stone tiles
	TileIndex tileIndex = 0;

	for ( int tileY = 0; tileY < m_mapSizeInTiles.y; tileY++ )
	{
		for ( int tileX = 0; tileX < m_mapSizeInTiles.x; tileX++ )
		{
			if ( tileX < NUM_PADDING_TILES || tileX > m_mapSizeInTiles.x  - ( NUM_PADDING_TILES + 1 ) || tileY < NUM_PADDING_TILES || tileY > m_mapSizeInTiles.y - ( NUM_PADDING_TILES + 1 ) )
			{
				m_tiles[ tileIndex ] = m_prototypeStoneTile;
				m_tiles[ tileIndex ].m_tileCoords = IntVector2( tileX, tileY );
				m_tiles[ tileIndex ].m_containsEntity = false;
			}
	
			tileIndex++;
		}
	}

	// Make some interior tiles out of prototype stone tiles
	for ( int numInteriorStoneTilesPlaced = 0; numInteriorStoneTilesPlaced < MAX_NUM_INTERIOR_STONE_TILES; numInteriorStoneTilesPlaced++ )
	{
		int randomTileIndex = rand() % m_tiles.size();

		while ( m_tiles[ randomTileIndex ].m_type == TILE_TYPE_STONE || ( ( m_tiles[ randomTileIndex ].m_tileCoords.x == floor( PLAYER_TANK_START_POSITION.x ) ) && ( m_tiles[ randomTileIndex ].m_tileCoords.y == floor( PLAYER_TANK_START_POSITION.y ) ) ) )
		{
			randomTileIndex = rand() % m_tiles.size();
		}

		IntVector2 tileCoords = m_tiles[ randomTileIndex ].m_tileCoords;
		bool containsEntity = m_tiles[ randomTileIndex ].m_containsEntity;
		m_tiles[ randomTileIndex ]= m_prototypeStoneTile;
		m_tiles[ randomTileIndex ].m_tileCoords = tileCoords;
		m_tiles[ randomTileIndex ].m_containsEntity = containsEntity;
	}

	// Place 10 turrets in random grass tiles
	for ( int numTurretsPlaced = 0; numTurretsPlaced < MAX_NUM_TURRETS; numTurretsPlaced++ )
	{
		int randomTileIndex = rand() % m_tiles.size();

		while ( m_tiles[ randomTileIndex ].m_type == TILE_TYPE_STONE || m_tiles[ randomTileIndex ].m_containsEntity )
		{
			randomTileIndex = rand() % m_tiles.size();
		}

		m_entities.insert( new Turret( GetWorldCoordsForTileCenter( randomTileIndex ) ) );
		m_tiles[ randomTileIndex ].m_containsEntity = true;
	}

	// Place 10 enemy tanks in random grass tiles
	for ( int numEnemyTanksPlaced = 0; numEnemyTanksPlaced < MAX_NUM_ENEMY_TANKS; numEnemyTanksPlaced++ )
	{
		int randomTileIndex = rand() % m_tiles.size();

		while ( m_tiles[ randomTileIndex ].m_type == TILE_TYPE_STONE || m_tiles[ randomTileIndex ].m_containsEntity )
		{
			randomTileIndex = rand() % m_tiles.size();
		}

		m_entities.insert( new EnemyTank( GetWorldCoordsForTileCenter( randomTileIndex ) ) );
		m_tiles[ randomTileIndex ].m_containsEntity = true;
	}

	// Place the player tank
	m_playerTank = new PlayerTank( PLAYER_TANK_START_POSITION );
	m_entities.insert( m_playerTank );
}


//-----------------------------------------------------------------------------------------------
void Map::DoCorrectivePhysicsEntityVsEntity( Entity& entity )
{
	std::set< Entity* >::iterator otherEntitiesIterator;
	for ( otherEntitiesIterator = m_entities.begin(); otherEntitiesIterator != m_entities.end(); ++otherEntitiesIterator )
	{
		Entity* otherEntity = *otherEntitiesIterator;
		if ( otherEntity )
		{
			if ( !otherEntity->UsesCorrectivePhysics() )
				break;

			WorldCoords entityPosition = entity.GetPosition();
			float entityPhysicsRadius = entity.GetPhysicsRadius();
			WorldCoords otherEntityPosition = otherEntity->GetPosition();
			float otherEntityPhysicsRadius = otherEntity->GetPhysicsRadius();

			if ( DoDiscsOverlap( entityPosition, entityPhysicsRadius, otherEntityPosition, otherEntityPhysicsRadius ) )
			{
				float overlapDepth = CalculateDiscOverlapDepth( entityPosition, entityPhysicsRadius, otherEntityPosition, otherEntityPhysicsRadius );
				float halfOfOverlapDepth = overlapDepth / 2.0f;

				if ( ( entityPosition == otherEntityPosition ) && ( entity.m_type == otherEntity->m_type ) )
				{
					// Likely the same entity, so don't apply corrective physics
					break;
				}
				else
				{
					// Get vector from entity to other entity
					Vector2 vectorFromEntityToOtherEntity = otherEntityPosition - entityPosition;

					Vector2 vectorToPushEntityFromOtherEntity = vectorFromEntityToOtherEntity.Normalize().Negate() * halfOfOverlapDepth;

					// Don't push turrets since they're stationary, don't push dead player tank
					if ( entity.m_type != ENTITY_TYPE_TURRET || ( entity.m_type == ENTITY_TYPE_PLAYER_TANK && m_playerTank->GetIsDead() ) )
					{
						entity.SetPosition( entityPosition + vectorToPushEntityFromOtherEntity );
					}

					// Get vector from other entity to entity
					Vector2 vectorFromOtherEntityToEntity = entityPosition - otherEntityPosition;

					Vector2 vectorToPushOtherEntityFromEntity = vectorFromOtherEntityToEntity.Normalize().Negate() * halfOfOverlapDepth;

					// Don't push turrets since they're stationary, don't push dead player tank
					if ( otherEntity->m_type != ENTITY_TYPE_TURRET || ( entity.m_type == ENTITY_TYPE_PLAYER_TANK && m_playerTank->GetIsDead() ) )
					{
						otherEntity->SetPosition( otherEntityPosition + vectorToPushOtherEntityFromEntity );
					}

				}
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DoCorrectivePhysicsEntityVsTiles( Entity& entity )
{
	if ( entity.GetIsDead() )
		return;

	float entityRadius = entity.GetPhysicsRadius();
	WorldCoords entityWorldCoords = entity.GetPosition();
	TileCoords entityTileCoords = GetTileCoordsForWorldCoords( entityWorldCoords );

	TileIndex northTileIndex = GetIndexForTileCoords( entityTileCoords.x, entityTileCoords.y + 1 );
	TileIndex southTileIndex = GetIndexForTileCoords( entityTileCoords.x, entityTileCoords.y - 1 );
	TileIndex eastTileIndex = GetIndexForTileCoords( entityTileCoords.x + 1, entityTileCoords.y );
	TileIndex westTileIndex = GetIndexForTileCoords( entityTileCoords.x - 1, entityTileCoords.y );

	// Check North tile neighbor
	if ( IsTileSolid( northTileIndex ) )
	{
		float minTileY = ( float ) entityTileCoords.y + 1;
		float maxTankY = entity.GetPosition().y + entityRadius;
		float overlapDepth = maxTankY - minTileY;
		if ( overlapDepth > 0.0f )
			entity.SetPosition( Vector2( entity.GetPosition().x, entity.GetPosition().y - overlapDepth ) );
	}

	// Check South tile neighbor
	if ( IsTileSolid( southTileIndex ) )
	{
		float maxTileY = ( float ) entityTileCoords.y;
		float minTankY = entity.GetPosition().y - entityRadius;
		float overlapDepth = maxTileY - minTankY;
		if ( overlapDepth > 0.0f )
			entity.SetPosition( Vector2( entity.GetPosition().x, entity.GetPosition().y + overlapDepth ) );
	}

	// Check East tile neighbor
	if ( IsTileSolid( eastTileIndex ) )
	{
		float minTileX = ( float ) entityTileCoords.x + 1;
		float maxTankX = entity.GetPosition().x + entityRadius;
		float overlapDepth = maxTankX - minTileX;
		if ( overlapDepth > 0.0f )
			entity.SetPosition( Vector2( entity.GetPosition().x - overlapDepth, entity.GetPosition().y ) );
	}

	// Check West tile neighbor
	if ( IsTileSolid( westTileIndex ) )
	{
		float maxTileX = ( float ) entityTileCoords.x;
		float minTankX = entity.GetPosition().x - entityRadius;
		float overlapDepth = maxTileX - minTankX;
		if ( overlapDepth > 0.0f )
			entity.SetPosition( Vector2( entity.GetPosition().x + overlapDepth, entity.GetPosition().y ) );
	}

	TileIndex northeastTileIndex = GetIndexForTileCoords( entityTileCoords.x + 1, entityTileCoords.y + 1 );
	TileIndex northwestTileIndex = GetIndexForTileCoords( entityTileCoords.x - 1, entityTileCoords.y + 1 );
	TileIndex southeastTileIndex = GetIndexForTileCoords( entityTileCoords.x + 1, entityTileCoords.y - 1 );
	TileIndex southwestTileIndex = GetIndexForTileCoords( entityTileCoords.x - 1, entityTileCoords.y - 1 );

	// Check Northeast tile neighbor
	if ( IsTileSolid( northeastTileIndex ) )
	{
		Vector2 tileCorner = GetWorldMinsForTile( northeastTileIndex );
		float distToCorner = CalculateDistanceBetweenPoints( tileCorner, entityWorldCoords );

		if ( distToCorner < entityRadius )
			PushEntityOutOfTile( entity, entityWorldCoords, tileCorner, distToCorner );
	}

	// Check Northwest tile neighbor
	if ( IsTileSolid( northwestTileIndex ) )
	{
		Vector2 tileCorner = GetWorldMinsForTile( northwestTileIndex );
		tileCorner += Vector2( 1.0f, 0.0f );

		float distToCorner = CalculateDistanceBetweenPoints( tileCorner, entityWorldCoords );

		if ( distToCorner < entityRadius )
			PushEntityOutOfTile( entity, entityWorldCoords, tileCorner, distToCorner );
	}

	// Check Southeast tile neighbor
	if ( IsTileSolid( southeastTileIndex ) )
	{
		Vector2 tileCorner = GetWorldMinsForTile( southeastTileIndex );
		tileCorner += Vector2( 0.0f, 1.0f );

		float distToCorner = CalculateDistanceBetweenPoints( tileCorner, entityWorldCoords );

		if ( distToCorner < entityRadius )
			PushEntityOutOfTile( entity, entityWorldCoords, tileCorner, distToCorner );
	}

	// Check Southwest tile neighbor
	if ( IsTileSolid( southwestTileIndex ) )
	{
		Vector2 tileCorner = GetWorldMaxsForTile( southwestTileIndex );

		float distToCorner = CalculateDistanceBetweenPoints( tileCorner, entityWorldCoords );

		if ( distToCorner < entityRadius )
			PushEntityOutOfTile( entity, entityWorldCoords, tileCorner, distToCorner );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DoPhysicsProjectilesVsTiles( Entity& entity )
{
	if ( entity.GetIsDead() )
		return;

	WorldCoords entityWorldCoords = entity.GetPosition();
	TileCoords entityTileCoords = GetTileCoordsForWorldCoords( entityWorldCoords );

	TileIndex occupiedTileIndex = GetIndexForTileCoords( entityTileCoords.x, entityTileCoords.y );
	
	if ( m_tiles[ occupiedTileIndex ].m_type == TILE_TYPE_STONE )
	{
		entity.SetHealth( 0 );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DoPhysicsProjectilesVsEntities( Entity& entity )
{
	if ( entity.m_type == ENTITY_TYPE_PLAYER_BULLET )
	{
		std::set< Entity* >::iterator enemyTankIterator;
		for ( enemyTankIterator = m_entities.begin(); enemyTankIterator != m_entities.end(); ++enemyTankIterator )
		{
			Entity* potentialEnemyTank = *enemyTankIterator;

			if ( potentialEnemyTank->m_type == ENTITY_TYPE_ENEMY_TANK )
			{
				if ( DoDiscsOverlap( entity.GetPosition(), entity.GetPhysicsRadius(), potentialEnemyTank->GetPosition(), potentialEnemyTank->GetPhysicsRadius() ) )
				{
					if ( entity.GetHealth() > 0 && ( potentialEnemyTank->GetHealth() > 0 ) )
					{
						entity.SetHealth( 0 );
						potentialEnemyTank->SetHealth( potentialEnemyTank->GetHealth() - 1 );
					}
				}
			}

		}

		std::set< Entity* >::iterator turretIterator;
		for ( turretIterator = m_entities.begin(); turretIterator != m_entities.end(); ++turretIterator )
		{
			Entity* potentialTurret = *turretIterator;

			if ( potentialTurret->m_type == ENTITY_TYPE_TURRET )
			{
				if ( DoDiscsOverlap( entity.GetPosition(), entity.GetPhysicsRadius(), potentialTurret->GetPosition(), potentialTurret->GetPhysicsRadius() ) )
				{
					if ( entity.GetHealth() > 0 && ( potentialTurret->GetHealth() > 0 ) )
					{
						entity.SetHealth( 0 );
						potentialTurret->SetHealth( potentialTurret->GetHealth() - 1 );
					}
				}
			}
		}
	}

	if ( entity.m_type == ENTITY_TYPE_ENEMY_BULLET )
	{
		if ( DoDiscsOverlap( entity.GetPosition(), entity.GetPhysicsRadius(), m_playerTank->GetPosition(), m_playerTank->GetPhysicsRadius() ) )
		{
			if ( entity.GetHealth() > 0 && ( m_playerTank->GetHealth() > 0 ) )
			{
				entity.SetHealth( 0 );
				m_playerTank->SetHealth( m_playerTank->GetHealth() - 1 );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::PushEntityOutOfTile( Entity& entity, WorldCoords entityWorldCoords, Vector2 tileCorner, float distToCorner )
{
	Vector2 pushDir = entityWorldCoords - tileCorner;
	pushDir.Normalize();
	float pushDistance = entity.GetPhysicsRadius() - distToCorner;
	pushDir = pushDir * pushDistance;
	entity.SetPosition( entity.GetPosition() + pushDir );
}


//-----------------------------------------------------------------------------------------------
Tile& Map::GetTileAtIndex( TileIndex tileIndex )
{
	return m_tiles[ tileIndex ];
}


//-----------------------------------------------------------------------------------------------
TileCoords Map::GetTileCoordsForWorldCoords( const WorldCoords& worldCoords )
{
	TileCoords tileCoords;
	tileCoords.x = ( int ) floor( worldCoords.x );
	tileCoords.y = ( int ) floor( worldCoords.y );
	return tileCoords;
}


//-----------------------------------------------------------------------------------------------
TileCoords Map::GetTileCoordsForIndex( TileIndex tileIndex )
{
	TileCoords tileCoords;
	tileCoords.x = tileIndex % m_mapSizeInTiles.x;
	tileCoords.y = tileIndex / m_mapSizeInTiles.x;
	return tileCoords;
}


//-----------------------------------------------------------------------------------------------
TileIndex Map::GetIndexForTileCoords( int tileX, int tileY )
{
	return ( m_mapSizeInTiles.x * tileY ) + tileX;
}


//-----------------------------------------------------------------------------------------------
TileIndex Map::GetIndexForTileCoords( const TileCoords& tileCoords )
{
	return ( m_mapSizeInTiles.x * tileCoords.y ) + tileCoords.x;
}


//-----------------------------------------------------------------------------------------------
WorldCoords Map::GetWorldCoordsForTileCenter( TileIndex tileIndex )
{
	WorldCoords worldCoords;
	worldCoords.x = m_tiles[ tileIndex ].m_tileCoords.x + 0.5f;
	worldCoords.y = m_tiles[ tileIndex ].m_tileCoords.y + 0.5f;
	return worldCoords;
}


//-----------------------------------------------------------------------------------------------
bool Map::IsTileSolid( TileIndex tileIndex )
{
	if ( GetTileAtIndex( tileIndex ).m_type == TILE_TYPE_STONE )
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
Vector2 Map::GetWorldMinsForTile( TileIndex tileIndex )
{
	return Vector2( ( float ) m_tiles[ tileIndex ].m_tileCoords.x, ( float ) m_tiles[ tileIndex ].m_tileCoords.y );
}


//-----------------------------------------------------------------------------------------------
Vector2 Map::GetWorldMaxsForTile( TileIndex tileIndex )
{
	return Vector2( ( ( float ) m_tiles[ tileIndex ].m_tileCoords.x ) + 1.0f, ( ( float ) m_tiles[ tileIndex ].m_tileCoords.y ) + 1.0f );
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnPlayerBullet()
{
	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( PLAYER_TANK_SHOOT_SOUND_FILE ), 1.0f );
	m_entities.insert( new PlayerBullet( m_playerTank->GetPosition(), m_playerTank->GetOrientationDegrees(), m_playerTank->GetCosmeticRadius() ) );
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnExplosion( Vector2 explosionLocation, float sizeScalingFactor, float durationInSeconds)
{
	m_explosions.push_back( new Explosion( explosionLocation, sizeScalingFactor, durationInSeconds ) );
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnTurretBullet( Entity* turret)
{
	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( PLAYER_TANK_SHOOT_SOUND_FILE ), 0.25f );
	m_entities.insert( new EnemyBullet( turret->GetPosition(), turret->GetOrientationDegrees(), turret->GetCosmeticRadius() ) );
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnEnemyTankBullet( Entity* enemyTank )
{
	g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( PLAYER_TANK_SHOOT_SOUND_FILE ), 0.25f );
	m_entities.insert( new EnemyBullet( enemyTank->GetPosition(), enemyTank->GetOrientationDegrees(), enemyTank->GetCosmeticRadius() ) );
}


//-----------------------------------------------------------------------------------------------
void Map::RespawnPlayerTank()
{
	m_playerTank->Respawn();
}

//-----------------------------------------------------------------------------------------------
bool Map::HasLineOfSight( const WorldCoords& start, const WorldCoords& end )
{
	Vector2 displacementVector = end - start;
	Vector2 displacementVectorChunk = displacementVector * ( 1.0f / 100.0f );

	WorldCoords currentPosition = start;
	TileCoords tileCoords;
	TileIndex tileIndex;

	for ( int displacementIndex = 0; displacementIndex < 100; displacementIndex++ )
	{
		currentPosition += displacementVectorChunk;

		tileCoords = GetTileCoordsForWorldCoords( currentPosition );

		if ( !( tileCoords.x > m_mapSizeInTiles.x || tileCoords.y > m_mapSizeInTiles.y || tileCoords.x < 0 || tileCoords.y < 0 ) ) // confirm tile exists
		{
			tileIndex = GetIndexForTileCoords( tileCoords );

			if ( m_tiles[ tileIndex ].m_type == TILE_TYPE_STONE )
			{
				return false;
			}
		}

	}

	return true;
}


//-----------------------------------------------------------------------------------------------
PlayerTank* Map::GetPlayerTank() const
{
	return m_playerTank;
}