#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/Sprites/SpriteSheet.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/Core/TheApp.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Environment/Map.hpp"
#include "Game/Environment/Tile.hpp"
#include "Game/Entities/Entity.hpp"
#include "Game/Entities/PlayerBullet.hpp"
#include "Game/Entities/EnemyBullet.hpp"


//-----------------------------------------------------------------------------------------------
Map* g_theMap = nullptr;


//-----------------------------------------------------------------------------------------------
// Constants
const int MAX_NUM_INTERIOR_STONE_TILES = 0;
const int MAX_NUM_ENEMY_CARS = 2;
const int NUM_PADDING_TILES = 1;
const WorldCoords PLAYER_START_POSITION = WorldCoords( 18.0f, 2.5f );


//-----------------------------------------------------------------------------------------------
Map::Map( const TileCoords& mapSizeInTiles )
	: m_mapSizeInTiles( mapSizeInTiles )
	, m_numAliveSeekEnemies( 0 )
	, m_numAlivePathEnemies( 0 )
	, m_numAliveWanderEnemies( 0 )
	, m_numAlivePursueEnemies( 0 )
	, m_numAlivePickups( 0 )
{
	PROFILE_LOG_SCOPE( "Map()" );

	InitializeArrays();
	InitializeTiles();
	PopulateMap();

	m_path = new Path();
	m_path->AddNode( Vector2( 31.0f, 5.5f ) );
	m_path->AddNode( Vector2( 31.0f, 16.5f ) );
	m_path->AddNode( Vector2( 18.0f, 12.5f ) );
	m_path->AddNode( Vector2( 5.0f, 16.5f ) );
	m_path->AddNode( Vector2( 5.0f, 5.5f ) );
	m_path->AddNode( Vector2( 18.0f, 8.5f ) );
}


//-----------------------------------------------------------------------------------------------
Map::~Map()
{
	delete m_player;
	m_player = nullptr;
	m_tiles.clear();
	m_entities.clear();
	delete m_path;
	m_path = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Map::Update( float deltaSeconds )
{
	CheckSeekEnemyCount();
	CheckPathEnemyCount();
	CheckWanderEnemyCount();
	CheckPursueEnemyCount();
	if ( GetRandomChance( 0.1f ) ) { SpawnStar(); }

	// Iterate through and update the entities
	std::set< Entity* >::iterator entitiesIterator;
	for ( entitiesIterator = m_entities.begin(); entitiesIterator != m_entities.end(); ++entitiesIterator )
	{
		Entity* entity = *entitiesIterator;
		if ( entity )
		{
// 			if ( entity->GetIsDead() && ( entity->m_type != ENTITY_TYPE_PLAYER_CAR ) )
// 			{
// 				if ( ( entity->m_type == ENTITY_TYPE_PLAYER_BULLET ) || ( entity->m_type == ENTITY_TYPE_ENEMY_BULLET ) )
// 				{
// 					//g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( EXPLOSION_SOUND_FILE ), 0.1f );
// 					SpawnExplosion( entity->GetPosition(), 0.5f, 0.25f );
// 				}
// 				else
// 				{
// 					//g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( EXPLOSION_SOUND_FILE ), 0.5f );
// 					SpawnExplosion( entity->GetPosition(), 1.25f, 0.5f );
// 				}
// 				delete entity;
// 				m_entities.erase( entitiesIterator );
// 				break;
// 			}

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

	UpdateStars( deltaSeconds );
	UpdateExplosions( deltaSeconds );
	UpdateEmitters( deltaSeconds );

	DeleteDeadEntities();
}


//-----------------------------------------------------------------------------------------------
void Map::UpdateExplosions( float deltaSeconds )
{
	// Call Update() for each explosion
	for ( int index = 0; index < MAX_EXPLOSION_COUNT; ++index )
	{
		if ( m_explosions[ index ] != nullptr )
		{
			m_explosions[ index ]->Update( deltaSeconds );
		}
	}

	DeleteFinishedExplosions();
}


//-----------------------------------------------------------------------------------------------
void Map::UpdateEmitters( float deltaSeconds )
{
	// Call Update() for each emitter
	for ( int index = 0; index < MAX_EMITTER_COUNT; ++index )
	{
		if ( m_emitters[ index ] != nullptr )
		{
			m_emitters[ index ]->Update( deltaSeconds );
		}
	}

	DeleteDeadEmitters();
}


//-----------------------------------------------------------------------------------------------
void Map::DeleteFinishedExplosions()
{
	for ( int index = 0; index < MAX_EXPLOSION_COUNT; ++index )
	{
		if ( m_explosions[ index ] != nullptr )
		{
			if ( m_explosions[ index ]->IsFinished() )
			{
				delete m_explosions[ index ];
				m_explosions[ index ] = nullptr;
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DeleteDeadEmitters()
{
	for ( int index = 0; index < MAX_EMITTER_COUNT; ++index )
	{
		if ( m_emitters[ index ] != nullptr )
		{
			if ( m_emitters[ index ]->m_isDead )
			{
				delete m_emitters[ index ];
				m_emitters[ index ] = nullptr;
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::UpdateStars( float deltaSeconds )
{
	// Call Update() for each star
	for ( int index = 0; index < MAX_STAR_COUNT; ++index )
	{
		if ( m_stars[ index ] != nullptr )
		{
			m_stars[ index ]->Update( deltaSeconds );
		}
	}

	DeleteDeadStars();
}


//-----------------------------------------------------------------------------------------------
void Map::DeleteDeadStars()
{
	for ( int index = 0; index < MAX_STAR_COUNT; ++index )
	{
		if ( m_stars[ index ] != nullptr )
		{
			if ( m_stars[ index ]->GetIsDead() )
			{
				delete m_stars[ index ];
				m_stars[ index ] = nullptr;
				return;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::Render()
{
	// Set up camera to follow player tank
	float numTilesInViewHorizontally = ( 16.0f / 9.0f ) * g_theCamera->GetNumTilesInViewVertically();
	Vector2 screenSize( numTilesInViewHorizontally, g_theCamera->GetNumTilesInViewVertically() );
	Vector2 cameraCenteredOnScreenPosition( screenSize.x * 0.5f, screenSize.y * 0.5f );
	g_theCamera->SetWorldPosition( cameraCenteredOnScreenPosition );
	//g_theCamera->SetWorldPosition( m_playerCar->GetPosition() );
	//g_theCamera->SetOrientationDegrees( m_playerCar->GetOrientationDegrees() - 90.0f );

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), screenSize );
	Vector3 positionAsVector3( cameraCenteredOnScreenPosition.x, cameraCenteredOnScreenPosition.y, 0.0f );
	g_theRenderer->TranslateView( positionAsVector3 );
	g_theRenderer->RotateView( -( g_theCamera->GetOrientationDegrees() ), Vector3( 0.0f, 0.0f, 1.0f ) );
	Vector3 worldPositionAsVector3( g_theCamera->GetWorldPosition().x, g_theCamera->GetWorldPosition().y, 0.0f );
	g_theRenderer->TranslateView( -( worldPositionAsVector3 ) );

	// Adjust order of below render calls to change rendering order
	RenderTiles();
	RenderGrid();
	RenderStars();
	m_path->Render();
	RenderEntities();
	RenderExplosions();
	RenderEmitters();

	// Player icon on bottom of HUD
	g_theRenderer->DrawFilledPolygon( 20, 1.5f, 0.5f,
		0.4f, Rgba::BLUE );

	g_theRenderer->DrawHollowPolygon( 20, 1.5f, 0.5f,
		0.4f, Rgba::WHITE );

	// Draw entity's forward direction vector
	float orientationRadians = ConvertDegreesToRadians( 90.0f );
	Vector2 entityForwardDirection( cos( orientationRadians ), sin( orientationRadians ) );
	Vector2 normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	Vector3 entityPositionAsVector3( 1.5f, 0.5f, 0.0f );
	Vector3 entityForwardDirectionAsVector3( entityForwardDirection.x, entityForwardDirection.y, 0.0f );
	g_theRenderer->DrawLine3D( entityPositionAsVector3, Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3
		* 0.4f ), Rgba::WHITE, 2.0f );

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	g_theRenderer->DrawText2D( Vector2( 2.0f, 0.2f ), "Player", 0.5f, Rgba::WHITE, fixedFont );

	// Seek/Flee enemy icon on bottom of HUD
	g_theRenderer->DrawFilledPolygon( 20, 6.5f, 0.5f,
		0.4f, Rgba::ORANGE );

	g_theRenderer->DrawHollowPolygon( 20, 6.5f, 0.5f,
		0.4f, Rgba::WHITE );

	// Draw entity's forward direction vector
	orientationRadians = ConvertDegreesToRadians( 90.0f );
	entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
	normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	entityPositionAsVector3 = Vector3( 6.5f, 0.5f, 0.0f );
	entityForwardDirectionAsVector3 = Vector3( entityForwardDirection.x, entityForwardDirection.y, 0.0f );
	g_theRenderer->DrawLine3D( entityPositionAsVector3, Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3
		* 0.4f ), Rgba::WHITE, 2.0f );

	g_theRenderer->DrawText2D( Vector2( 7.0f, 0.2f ), "Seek/Flee", 0.5f, Rgba::WHITE, fixedFont );

	// Path enemy icon on bottom of HUD
	g_theRenderer->DrawFilledPolygon( 20, 12.5f, 0.5f,
		0.4f, Rgba::LIGHTOLIVEGREEN );

	g_theRenderer->DrawHollowPolygon( 20, 12.5f, 0.5f,
		0.4f, Rgba::WHITE );

	// Draw entity's forward direction vector
	orientationRadians = ConvertDegreesToRadians( 90.0f );
	entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
	normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	entityPositionAsVector3 = Vector3( 12.5f, 0.5f, 0.0f );
	entityForwardDirectionAsVector3 = Vector3( entityForwardDirection.x, entityForwardDirection.y, 0.0f );
	g_theRenderer->DrawLine3D( entityPositionAsVector3, Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3
		* 0.4f ), Rgba::WHITE, 2.0f );

	g_theRenderer->DrawText2D( Vector2( 13.0f, 0.2f ), "Path Follow", 0.5f, Rgba::WHITE, fixedFont );

	// Wander enemy icon on bottom of HUD
	g_theRenderer->DrawFilledPolygon( 20, 19.5f, 0.5f,
		0.4f, Rgba::PINK );

	g_theRenderer->DrawHollowPolygon( 20, 19.5f, 0.5f,
		0.4f, Rgba::WHITE );

	// Draw entity's forward direction vector
	orientationRadians = ConvertDegreesToRadians( 90.0f );
	entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
	normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	entityPositionAsVector3 = Vector3( 19.5f, 0.5f, 0.0f );
	entityForwardDirectionAsVector3 = Vector3( entityForwardDirection.x, entityForwardDirection.y, 0.0f );
	g_theRenderer->DrawLine3D( entityPositionAsVector3, Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3
		* 0.4f ), Rgba::WHITE, 2.0f );

	g_theRenderer->DrawText2D( Vector2( 20.0f, 0.2f ), "Wander", 0.5f, Rgba::WHITE, fixedFont );

	// Pursue/Evade enemy icon on bottom of HUD
	g_theRenderer->DrawFilledPolygon( 20, 24.5f, 0.5f,
		0.4f, Rgba::RED );

	g_theRenderer->DrawHollowPolygon( 20, 24.5f, 0.5f,
		0.4f, Rgba::WHITE );

	// Draw entity's forward direction vector
	orientationRadians = ConvertDegreesToRadians( 90.0f );
	entityForwardDirection = Vector2( cos( orientationRadians ), sin( orientationRadians ) );
	normalizedEntityForwardDirection = entityForwardDirection.Normalize();
	entityPositionAsVector3 = Vector3( 24.5f, 0.5f, 0.0f );
	entityForwardDirectionAsVector3 = Vector3( entityForwardDirection.x, entityForwardDirection.y, 0.0f );
	g_theRenderer->DrawLine3D( entityPositionAsVector3, Vector3( entityPositionAsVector3 + entityForwardDirectionAsVector3
		* 0.4f ), Rgba::WHITE, 2.0f );

	g_theRenderer->DrawText2D( Vector2( 25.0f, 0.2f ), "Pursue/Evade", 0.5f, Rgba::WHITE, fixedFont );
}


//-----------------------------------------------------------------------------------------------
void Map::RenderExplosions()
{
	// Call Render() for each explosion
	for ( int index = 0; index < MAX_EXPLOSION_COUNT; ++index )
	{
		if ( m_explosions[ index ] != nullptr )
		{
			m_explosions[ index ]->Render();
		}
	}
}



//-----------------------------------------------------------------------------------------------
void Map::RenderEmitters()
{
	// Call Render() for each emitter
	for ( int index = 0; index < MAX_EMITTER_COUNT; ++index )
	{
		if ( m_emitters[ index ] != nullptr )
		{
			m_emitters[ index ]->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::RenderStars()
{
	// Call Render() for each star
	for ( int index = 0; index < MAX_STAR_COUNT; ++index )
	{
		if ( m_stars[ index ] != nullptr )
		{
			m_stars[ index ]->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::RenderEntities()
{
	// Iterate through and draw the entities
	for ( Entity* entity : m_entities )
	{
		if ( entity )
		{
			entity->Render();
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::RenderGrid()
{
	// Draw a grid
	float gridSize = ( float ) m_mapSizeInTiles.x;

	for ( float xy = 0.0f; xy < gridSize; xy += 1.0f )
	{
		g_theRenderer->DrawLine3D( Vector3( xy, 0.0f, 0.0f ), Vector3( xy, gridSize, 0.0f ), Rgba( 255, 255, 255, 12 ), 2.0f );
		g_theRenderer->DrawLine3D( Vector3( 0.0f, xy, 0.0f ), Vector3( gridSize, xy, 0.0f ), Rgba( 255, 255, 255, 12 ), 2.0f );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::RenderTiles()
{
	// Draw the tiles
	for ( Tile tile : m_tiles )
	{
		AABB2 bounds = AABB2( Vector2( ( float ) tile.m_tileCoords.x, ( float ) tile.m_tileCoords.y ),
			Vector2( ( float ) tile.m_tileCoords.x + 1, ( float ) tile.m_tileCoords.y + 1 ) );

		if ( tile.m_type == TILE_TYPE_GRASS )
		{
			//g_theRenderer->DrawTexturedAABB2( bounds, *Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), Rgba::LIGHTOLIVEGREEN );
		}
		if ( tile.m_type == TILE_TYPE_STONE )
		{
			g_theRenderer->DrawTexturedAABB2( bounds, *Texture::CreateOrGetTexture( "Data/Images/PlainWhiteTexture.png" ), Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ), Rgba::GRAY );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::InitializeArrays()
{
	// Set all explosions pointers to nullptr
	for ( int index = 0; index < MAX_EXPLOSION_COUNT; ++index )
	{
		m_explosions[ index ] = nullptr;
	}

	// Set all emitter pointers to nullptr
	for ( int index = 0; index < MAX_EMITTER_COUNT; ++index )
	{
		m_emitters[ index ] = nullptr;
	}

	// Set all star pointers to nullptr
	for ( int index = 0; index < MAX_STAR_COUNT; ++index )
	{
		m_stars[ index ] = nullptr;
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
			if ( tileX < NUM_PADDING_TILES || tileX > m_mapSizeInTiles.x - ( NUM_PADDING_TILES + 1 ) || tileY < NUM_PADDING_TILES || tileY > m_mapSizeInTiles.y - ( NUM_PADDING_TILES + 1 ) )
			{
				m_tiles[ tileIndex ] = m_prototypeStoneTile;
				m_tiles[ tileIndex ].m_tileCoords = IntVector2( tileX, tileY );
				m_tiles[ tileIndex ].m_containsEntity = false;
			}

			tileIndex++;
		}
	}

	// Make some interior tiles out of prototype stone tiles
	tileIndex = 0;

	for ( int tileY = 0; tileY < m_mapSizeInTiles.y; tileY++ )
	{
		for ( int tileX = 0; tileX < m_mapSizeInTiles.x; tileX++ )
		{
			if ( tileIndex == 332 || tileIndex == 333 || tileIndex == 334 ||
				 tileIndex == 368 || tileIndex == 369 || tileIndex == 370 ||
				 tileIndex == 404 || tileIndex == 405 || tileIndex == 406 ||
				 tileIndex == 349 || tileIndex == 350 || tileIndex == 351 || 
				 tileIndex == 385 || tileIndex == 386 || tileIndex == 387 ||
				 tileIndex == 421 || tileIndex == 422 || tileIndex == 423 )
			{
				m_tiles[ tileIndex ] = m_prototypeStoneTile;
				m_tiles[ tileIndex ].m_tileCoords = IntVector2( tileX, tileY );
				m_tiles[ tileIndex ].m_containsEntity = false;
			}

			tileIndex++;
		}
	}

	// Place the player tank
	m_player = new Player( PLAYER_START_POSITION );
	m_entities.insert( m_player );
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnSeekEnemies()
{
	if ( g_theGame == nullptr )
	{
		return;
	}
	
	if ( g_theGame->GetState() != STATE_PLAYING )
	{
		return;
	}

	for ( int numEnemyCarsPlaced = 0; numEnemyCarsPlaced < MAX_NUM_ENEMY_CARS; numEnemyCarsPlaced++ )
	{
		int randomTileIndex = rand() % m_tiles.size() / 5 + ( m_tiles.size() * 4 / 5 );

		while ( m_tiles[ randomTileIndex ].m_type == TILE_TYPE_STONE || m_tiles[ randomTileIndex ].m_containsEntity )
		{
			randomTileIndex = rand() % m_tiles.size() / 5 + ( m_tiles.size() * 4 / 5 );
		}

		m_entities.insert( new Enemy( GetWorldCoordsForTileCenter( randomTileIndex ), ENTITY_TYPE_SEEK_ENEMY ) );
		++m_numAliveSeekEnemies;
		m_tiles[ randomTileIndex ].m_containsEntity = true;
	}
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnPathEnemies()
{
	if ( g_theGame == nullptr )
	{
		return;
	}

	if ( g_theGame->GetState() != STATE_PLAYING )
	{
		return;
	}

	int randomTileIndex = rand() % m_tiles.size() / 5 + ( m_tiles.size() * 4 / 5 );

	while ( m_tiles[ randomTileIndex ].m_type == TILE_TYPE_STONE || m_tiles[ randomTileIndex ].m_containsEntity )
	{
		randomTileIndex = rand() % m_tiles.size() / 5 + ( m_tiles.size() * 4 / 5 );
	}

	m_entities.insert( new Enemy( GetWorldCoordsForTileCenter( randomTileIndex ), ENTITY_TYPE_PATH_ENEMY ) );
	++m_numAlivePathEnemies;
	m_tiles[ randomTileIndex ].m_containsEntity = true;
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnWanderEnemies()
{
	if ( g_theGame == nullptr )
	{
		return;
	}

	if ( g_theGame->GetState() != STATE_PLAYING )
	{
		return;
	}

	int randomTileIndex = rand() % m_tiles.size() / 5 + ( m_tiles.size() * 4 / 5 );

	while ( m_tiles[ randomTileIndex ].m_type == TILE_TYPE_STONE || m_tiles[ randomTileIndex ].m_containsEntity )
	{
		randomTileIndex = rand() % m_tiles.size() / 5 + ( m_tiles.size() * 4 / 5 );
	}

	m_entities.insert( new Enemy( GetWorldCoordsForTileCenter( randomTileIndex ), ENTITY_TYPE_WANDER_ENEMY ) );
	++m_numAliveWanderEnemies;
	m_tiles[ randomTileIndex ].m_containsEntity = true;
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnPursueEnemies()
{
	if ( g_theGame == nullptr )
	{
		return;
	}

	if ( g_theGame->GetState() != STATE_PLAYING )
	{
		return;
	}

	int randomTileIndex = rand() % m_tiles.size() / 5 + ( m_tiles.size() * 4 / 5 );

	while ( m_tiles[ randomTileIndex ].m_type == TILE_TYPE_STONE || m_tiles[ randomTileIndex ].m_containsEntity )
	{
		randomTileIndex = rand() % m_tiles.size() / 5 + ( m_tiles.size() * 4 / 5 );
	}

	m_entities.insert( new Enemy( GetWorldCoordsForTileCenter( randomTileIndex ), ENTITY_TYPE_PURSUE_ENEMY ) );
	++m_numAlivePursueEnemies;
	m_tiles[ randomTileIndex ].m_containsEntity = true;
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnStar()
{
	if ( g_theGame == nullptr )
	{
		return;
	}

	float spawnX = GetRandomFloatInRange( 1.0f, ( float ) ( m_mapSizeInTiles.x - 1 ) );
	Vector2 spawnPosition( spawnX, 21.0f );

	for ( int index = 0; index < MAX_STAR_COUNT; ++index )
	{
		if ( m_stars[ index ] == nullptr )
		{
			m_stars[ index ] = new Star( spawnPosition );
			return;
		}
	}
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
					if ( entity.m_type == ENTITY_TYPE_PLAYER )
					{
						break; // REMOVE ME LATER
					}

					// Get vector from entity to other entity
					Vector2 vectorFromEntityToOtherEntity = otherEntityPosition - entityPosition;

					Vector2 vectorToPushEntityFromOtherEntity = vectorFromEntityToOtherEntity.Normalize().Negate() * halfOfOverlapDepth;

					// Don't push dead player tank
					if (  entity.m_type == ENTITY_TYPE_PLAYER )
					{
						entity.SetPosition( entityPosition + vectorToPushEntityFromOtherEntity );
					}

					// Get vector from other entity to entity
					Vector2 vectorFromOtherEntityToEntity = entityPosition - otherEntityPosition;

					Vector2 vectorToPushOtherEntityFromEntity = vectorFromOtherEntityToEntity.Normalize().Negate() * halfOfOverlapDepth;

					// Don't push dead player tank
					if ( entity.m_type == ENTITY_TYPE_PLAYER )
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
	float entityRadius = entity.GetPhysicsRadius();
	WorldCoords entityWorldCoords = entity.GetPosition();
	TileCoords entityTileCoords = GetTileCoordsForWorldCoords( entityWorldCoords );

	TileIndex northTileIndex = GetIndexForTileCoords( entityTileCoords.x, entityTileCoords.y + 1 );
	TileIndex southTileIndex = GetIndexForTileCoords( entityTileCoords.x, entityTileCoords.y - 1 );
	TileIndex eastTileIndex = GetIndexForTileCoords( entityTileCoords.x + 1, entityTileCoords.y );
	TileIndex westTileIndex = GetIndexForTileCoords( entityTileCoords.x - 1, entityTileCoords.y );

	// Check North tile neighbor
	if ( northTileIndex >= ( int ) m_tiles.size() || northTileIndex < 0 )
	{
		float minTileY = ( float ) entityTileCoords.y + 1;
		float maxCarY = entity.GetPosition().y + entityRadius;
		float overlapDepth = maxCarY - minTileY;
		if ( overlapDepth > 0.0f )
		{
			entity.SetPosition( Vector2( entity.GetPosition().x, entity.GetPosition().y - overlapDepth ) );
		}
	}
	else if ( IsTileSolid( northTileIndex ) )
	{
		float minTileY = ( float ) entityTileCoords.y + 1;
		float maxCarY = entity.GetPosition().y + entityRadius;
		float overlapDepth = maxCarY - minTileY;
		if ( overlapDepth > 0.0f )
		{
			entity.SetPosition( Vector2( entity.GetPosition().x, entity.GetPosition().y - overlapDepth ) );
		}
	}

	// Check South tile neighbor
	if ( southTileIndex >= ( int ) m_tiles.size() || southTileIndex < 0 )
	{
		float maxTileY = ( float ) entityTileCoords.y;
		float minCarY = entity.GetPosition().y - entityRadius;
		float overlapDepth = maxTileY - minCarY;
		if ( overlapDepth > 0.0f )
		{
			entity.SetPosition( Vector2( entity.GetPosition().x, entity.GetPosition().y + overlapDepth ) );
		}
	}
	else if ( IsTileSolid( southTileIndex ) )
	{
		float maxTileY = ( float ) entityTileCoords.y;
		float minCarY = entity.GetPosition().y - entityRadius;
		float overlapDepth = maxTileY - minCarY;
		if ( overlapDepth > 0.0f )
		{
			entity.SetPosition( Vector2( entity.GetPosition().x, entity.GetPosition().y + overlapDepth ) );
		}
	}

	// Check East tile neighbor
	if ( eastTileIndex >= ( int ) m_tiles.size() || eastTileIndex < 0 )
	{
		float minTileX = ( float ) entityTileCoords.x + 1;
		float maxCarX = entity.GetPosition().x + entityRadius;
		float overlapDepth = maxCarX - minTileX;
		if ( overlapDepth > 0.0f )
		{
			entity.SetPosition( Vector2( entity.GetPosition().x - overlapDepth, entity.GetPosition().y ) );
		}
	}
	else if ( IsTileSolid( eastTileIndex ) )
	{
		float minTileX = ( float ) entityTileCoords.x + 1;
		float maxCarX = entity.GetPosition().x + entityRadius;
		float overlapDepth = maxCarX - minTileX;
		if ( overlapDepth > 0.0f )
		{
			entity.SetPosition( Vector2( entity.GetPosition().x - overlapDepth, entity.GetPosition().y ) );
		}
	}

	// Check West tile neighbor
	if ( westTileIndex >= ( int ) m_tiles.size() || westTileIndex < 0 )
	{
		float maxTileX = ( float ) entityTileCoords.x;
		float minCarX = entity.GetPosition().x - entityRadius;
		float overlapDepth = maxTileX - minCarX;
		if ( overlapDepth > 0.0f )
		{
			entity.SetPosition( Vector2( entity.GetPosition().x + overlapDepth, entity.GetPosition().y ) );
		}
	}
	else if ( IsTileSolid( westTileIndex ) )
	{
		float maxTileX = ( float ) entityTileCoords.x;
		float minCarX = entity.GetPosition().x - entityRadius;
		float overlapDepth = maxTileX - minCarX;
		if ( overlapDepth > 0.0f )
		{
			entity.SetPosition( Vector2( entity.GetPosition().x + overlapDepth, entity.GetPosition().y ) );
		}
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

	if ( occupiedTileIndex >= ( int ) m_tiles.size() || occupiedTileIndex < 0 )
	{
		entity.SetHealth( 0 );
	}
	else if ( m_tiles[ occupiedTileIndex ].m_type == TILE_TYPE_STONE )
	{
		entity.SetHealth( 0 );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::DoPhysicsProjectilesVsEntities( Entity& entity )
{
	if ( entity.m_type == ENTITY_TYPE_PLAYER_BULLET )
	{
		std::set< Entity* >::iterator entityIterator;
		for ( entityIterator = m_entities.begin(); entityIterator != m_entities.end(); ++entityIterator )
		{
			Entity* enemy = *entityIterator;

			if ( enemy->m_type == ENTITY_TYPE_SEEK_ENEMY || enemy->m_type == ENTITY_TYPE_PATH_ENEMY ||
				 enemy->m_type == ENTITY_TYPE_WANDER_ENEMY )
			{
				if ( DoDiscsOverlap( entity.GetPosition(), entity.GetPhysicsRadius(), enemy->GetPosition(), enemy->GetPhysicsRadius() ) )
				{
					if ( entity.GetHealth() > 0 && ( enemy->GetHealth() > 0 ) )
					{
						entity.SetHealth( 0 );
						enemy->SetHealth( enemy->GetHealth() - 1 );
					}
				}
			}

		}
	}

	if ( entity.m_type == ENTITY_TYPE_ENEMY_BULLET )
	{
		if ( DoDiscsOverlap( entity.GetPosition(), entity.GetPhysicsRadius(), m_player->GetPosition(), m_player->GetPhysicsRadius() ) )
		{
			if ( entity.GetHealth() > 0 && ( m_player->GetHealth() > 0 ) )
			{
				entity.SetHealth( 0 );
				m_player->SetHealth( m_player->GetHealth() - 1 );
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
	{
		return true;
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnPlayerBullet( const WorldCoords& spawningPlayerPosition, float spawnOrientationDegrees,
	float spawningPlayerCosmeticRadius )
{
	if ( !g_theGame->m_disableShooting )
	{
		m_entities.insert( new PlayerBullet( spawningPlayerPosition, spawnOrientationDegrees,
			spawningPlayerCosmeticRadius ) );
	}
}


//-----------------------------------------------------------------------------------------------
Vector2 Map::GetWorldMinsForTile( TileIndex tileIndex )
{
	return Vector2( ( float ) m_tiles[ tileIndex ].m_tileCoords.x,
		( float ) m_tiles[ tileIndex ].m_tileCoords.y );
}


//-----------------------------------------------------------------------------------------------
Vector2 Map::GetWorldMaxsForTile( TileIndex tileIndex )
{
	return Vector2( ( ( float ) m_tiles[ tileIndex ].m_tileCoords.x ) + 1.0f,
		( ( float ) m_tiles[ tileIndex ].m_tileCoords.y ) + 1.0f );
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

		if ( !( tileCoords.x > m_mapSizeInTiles.x || tileCoords.y > m_mapSizeInTiles.y
			|| tileCoords.x < 0 || tileCoords.y < 0 ) ) // confirm tile exists
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
void Map::DeleteDeadEntities()
{
	std::set< Entity* >::iterator entityIterator = m_entities.begin();
	while ( entityIterator != m_entities.end() )
	{
		if ( ( *entityIterator )->GetIsDead() )
		{
			Entity* entity = *entityIterator;
			if ( entity->m_type == ENTITY_TYPE_SEEK_ENEMY )
			{
				g_theGame->m_score++;
				--m_numAliveSeekEnemies;
			}
			else if ( entity->m_type == ENTITY_TYPE_PATH_ENEMY )
			{
				g_theGame->m_score += 5;
				--m_numAlivePathEnemies;
			}
			else if ( entity->m_type == ENTITY_TYPE_WANDER_ENEMY )
			{
				g_theGame->m_score += 3;
				--m_numAliveWanderEnemies;
			}
			entityIterator = m_entities.erase( entityIterator );
			SpawnEmitter( entity->GetPosition() );
		}
		else
		{
			++entityIterator;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::CheckSeekEnemyCount()
{
	if ( m_numAliveSeekEnemies < 1 )
	{
		SpawnSeekEnemies();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::CheckPathEnemyCount()
{
	if ( m_numAlivePathEnemies < 1 )
	{
		SpawnPathEnemies();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::CheckWanderEnemyCount()
{
	if ( m_numAliveWanderEnemies < 1 )
	{
		SpawnWanderEnemies();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::CheckPursueEnemyCount()
{
	if ( m_numAlivePursueEnemies < 1 )
	{
		SpawnPursueEnemies();
	}
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnExplosion( Vector2 explosionLocation, float sizeScalingFactor, float durationInSeconds )
{
	for ( int index = 0; index < MAX_EXPLOSION_COUNT; ++index )
	{
		if ( m_explosions[ index ] == nullptr )
		{
			m_explosions[ index ] = new Explosion( explosionLocation, sizeScalingFactor, durationInSeconds );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnEmitter( Vector2 emitterLocation )
{
	for ( int index = 0; index < MAX_EMITTER_COUNT; ++index )
	{
		if ( m_emitters[ index ] == nullptr )
		{
			m_emitters[ index ] = new Emitter( emitterLocation, Vector2( 1.0f, 1.0f ), 0.5f, 50, 
											   EMITTER_TYPE_EXPLOSION );
			return;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Map::SpawnEnemyBullet( const WorldCoords& spawningEnemyPosition, float spawnOrientationDegrees,
	float spawningEnemyCosmeticRadius )
{
	if ( !g_theGame->m_disableShooting )
	{
		m_entities.insert( new EnemyBullet( spawningEnemyPosition, spawnOrientationDegrees,
			spawningEnemyCosmeticRadius ) );
	}
}


//-----------------------------------------------------------------------------------------------
void Map::Restart()
{
	for ( int index = 0; index < MAX_EXPLOSION_COUNT; ++index )
	{
		if ( m_explosions[ index ] != nullptr )
		{
			delete m_explosions[ index ];
			m_explosions[ index ] = nullptr;
		}
	}

	m_entities.clear();
	m_tiles.clear();
	m_numAliveSeekEnemies = 0;
	m_numAlivePathEnemies = 0;
	m_numAliveWanderEnemies = 0;
	m_numAlivePickups = 0;
	g_theGame->m_score = 0;
	InitializeArrays();
	InitializeTiles();
	PopulateMap();
}
