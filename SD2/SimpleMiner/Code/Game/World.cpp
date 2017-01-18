#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/TheGame.hpp"
#include "Game/World.hpp"
#include "Game/BlockDefinition.hpp"


//-----------------------------------------------------------------------------------------------
bool g_blockHighlighted = false;
bool g_waitForRightMouseButtonRelease = false;
Vector3 g_blockHighlightQuadVertex1( 0.0f, 0.0f, 0.0f );
Vector3 g_blockHighlightQuadVertex2( 0.0f, 0.0f, 0.0f );
Vector3 g_blockHighlightQuadVertex3( 0.0f, 0.0f, 0.0f );
Vector3 g_blockHighlightQuadVertex4( 0.0f, 0.0f, 0.0f );
Vector3 g_playerRaycastStart( 0.0f, 0.0f, 0.0f );
Vector3 g_playerRaycastEnd( 0.0f, 0.0f, 0.0f );
int g_breakingSpriteIndex = 0;


//-----------------------------------------------------------------------------------------------
World::World()
	: m_numRenderedChunks( 0 )
	, m_numSavedChunks( 0 )
	, m_numLoadedChunks( 0 )
	, m_indexOfBlockBeingDug( -1 )
	, m_diggingStartTime( 0.0f )
	, m_diggingEndTime( 0.0f )
	, m_activeChunksSavedToDisk( false )
	, m_playerSavedToDisk( false )
	, m_blockBeingDug( false )
{
	BlockDefinition::InitializeBlockDefinitions();
	std::memset( m_playerBoundingBoxVertexes, 0, sizeof m_playerBoundingBoxVertexes );
	std::memset( m_blockBreakingVertexes, 0, sizeof m_blockBreakingVertexes );
}


//-----------------------------------------------------------------------------------------------
void World::Update( float deltaSeconds )
{
	deltaSeconds;

	UpdateCameraAndPlayer();

	// Check to see if any VAs/VBOs need to be updated
	bool vboNeedsUpdating = false;

	for ( auto activeChunkIterator = m_activeChunks.begin(); activeChunkIterator != m_activeChunks.end(); ++activeChunkIterator )
	{
		if ( activeChunkIterator->second->m_vboIsOutdated )
			vboNeedsUpdating = true;
	}

	// If no VAs/VBOs need updating, check if a chunk needs to be generated/loaded
	if ( !vboNeedsUpdating )
	{
		bool chunkActivatedThisFrame = ActivateNearestMissingChunk();

		// If no chunk was generated/loaded this frame, deactivate a chunk
		if ( !chunkActivatedThisFrame )
		{
			DeactivateFarthestObsoleteChunk();
		}
	}

	UpdateChunks();
}


//-----------------------------------------------------------------------------------------------
void World::Render() const
{

	// Render chunks
	for ( auto activeChunkIterator = m_activeChunks.begin(); activeChunkIterator != m_activeChunks.end(); activeChunkIterator++ )
	{
		if ( activeChunkIterator->second->m_isRendered )
			activeChunkIterator->second->Render();
	}

	static SpriteSheet spriteSheet( "Data/Images/SimpleMinerAtlas.png", 16, 16 );

	// Draw the selected block face highlight
	if ( g_blockHighlighted )
	{
		g_theRenderer->DrawSquareLineLoop3D( g_blockHighlightQuadVertex1, g_blockHighlightQuadVertex2, g_blockHighlightQuadVertex3, g_blockHighlightQuadVertex4, Rgba::WHITE );
	}

	// Draw the block breaking texture
	if ( m_blockBeingDug )
	{
		AABB2 texCoords( spriteSheet.GetTexCoordsForSpriteCoords( g_breakingSpriteIndex, 14 ) );
		Rgba tint( 255, 255, 255, 60 );
		g_theRenderer->DrawTexturedQuad3D( m_blockBreakingVertexes[ 0 ], m_blockBreakingVertexes[ 1 ], m_blockBreakingVertexes[ 2 ], m_blockBreakingVertexes[ 3 ], spriteSheet.GetTexture(), texCoords.mins, texCoords.maxs, tint );
		g_theRenderer->DrawTexturedQuad3D( m_blockBreakingVertexes[ 4 ], m_blockBreakingVertexes[ 5 ], m_blockBreakingVertexes[ 6 ], m_blockBreakingVertexes[ 7 ], spriteSheet.GetTexture(), texCoords.mins, texCoords.maxs, tint );
		g_theRenderer->DrawTexturedQuad3D( m_blockBreakingVertexes[ 8 ], m_blockBreakingVertexes[ 9 ], m_blockBreakingVertexes[ 10 ], m_blockBreakingVertexes[ 11 ], spriteSheet.GetTexture(), texCoords.mins, texCoords.maxs, tint );
		g_theRenderer->DrawTexturedQuad3D( m_blockBreakingVertexes[ 12 ], m_blockBreakingVertexes[ 13 ], m_blockBreakingVertexes[ 14 ], m_blockBreakingVertexes[ 15 ], spriteSheet.GetTexture(), texCoords.mins, texCoords.maxs, tint );
		g_theRenderer->DrawTexturedQuad3D( m_blockBreakingVertexes[ 16 ], m_blockBreakingVertexes[ 17 ], m_blockBreakingVertexes[ 18 ], m_blockBreakingVertexes[ 19 ], spriteSheet.GetTexture(), texCoords.mins, texCoords.maxs, tint );
		g_theRenderer->DrawTexturedQuad3D( m_blockBreakingVertexes[ 20 ], m_blockBreakingVertexes[ 21 ], m_blockBreakingVertexes[ 22 ], m_blockBreakingVertexes[ 23 ], spriteSheet.GetTexture(), texCoords.mins, texCoords.maxs, tint );
	}

	// Draw the player bounding box
	g_theRenderer->DrawSquareLineLoop3DXray( m_playerBoundingBoxVertexes[ 0 ], m_playerBoundingBoxVertexes[ 1 ], m_playerBoundingBoxVertexes[ 2 ], m_playerBoundingBoxVertexes[ 3 ], Rgba::WHITE );
	g_theRenderer->DrawSquareLineLoop3DXray( m_playerBoundingBoxVertexes[ 4 ], m_playerBoundingBoxVertexes[ 5 ], m_playerBoundingBoxVertexes[ 6 ], m_playerBoundingBoxVertexes[ 7 ], Rgba::WHITE );
	g_theRenderer->DrawSquareLineLoop3DXray( m_playerBoundingBoxVertexes[ 16 ], m_playerBoundingBoxVertexes[ 17 ], m_playerBoundingBoxVertexes[ 18 ], m_playerBoundingBoxVertexes[ 19 ], Rgba::WHITE );
	g_theRenderer->DrawSquareLineLoop3DXray( m_playerBoundingBoxVertexes[ 12 ], m_playerBoundingBoxVertexes[ 13 ], m_playerBoundingBoxVertexes[ 14 ], m_playerBoundingBoxVertexes[ 15 ], Rgba::WHITE );
	
	// Draw the player raycast line
	if ( g_blockHighlighted )
	{
		g_theRenderer->DrawPlayerRaycastLine3DXray( g_playerRaycastStart, g_playerRaycastEnd, Rgba::RED );
	}
	else
	{
		g_theRenderer->DrawPlayerRaycastLine3DXray( g_playerRaycastStart, g_playerRaycastEnd, Rgba::WHITE );
	}
}


//-----------------------------------------------------------------------------------------------
void World::UpdateCameraAndPlayer()
{
	Vector3 playerForwardXYZ = g_theGame->GetPlayerForwardXYZ();
	playerForwardXYZ.Normalize();
	Vector3 raycastStartWorldPos3D = g_theGame->m_player.m_position + Vector3( 0.0f, 0.0f, PLAYER_EYE_LEVEL );
	Vector3 raycastEndWorldPos3D = raycastStartWorldPos3D + ( playerForwardXYZ * CAMERA_FORWARD_RAYCAST_LENGTH );

	Vector3 playerBoundingBoxCenter = g_theGame->m_player.m_position;
	g_theGame->m_player.m_boundingBox = AABB3( Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS ), 
		                                       Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS ) );

	SetPlayerBoundingBoxDrawPositions( playerBoundingBoxCenter );

	g_playerRaycastStart = raycastStartWorldPos3D;
	g_playerRaycastEnd = raycastEndWorldPos3D;

	m_raycastResult = AWRaycast3D( raycastStartWorldPos3D, raycastEndWorldPos3D );

	if ( m_raycastResult.m_didHit && ( m_raycastResult.m_blockInfo.m_chunk != nullptr ) )
	{
		g_blockHighlighted = true;

		BlockInfo selectedBlock = m_raycastResult.m_blockInfo;
		BlockInfo eastNeighbor = selectedBlock.GetEastNeighborInfo();
		BlockInfo westNeighbor = selectedBlock.GetWestNeighborInfo();
		BlockInfo northNeighbor = selectedBlock.GetNorthNeighborInfo();
		BlockInfo southNeighbor = selectedBlock.GetSouthNeighborInfo();
		BlockInfo topNeighbor = selectedBlock.GetTopNeighborInfo();
		BlockInfo bottomNeighbor = selectedBlock.GetBottomNeighborInfo();

		if ( m_raycastResult.m_faceNormal.x == 1.0f )
		{
			HandleEastFaceSelection( selectedBlock, eastNeighbor );
		}
		else if ( m_raycastResult.m_faceNormal.x == -1.0f )
		{
			HandleWestFaceSelection( selectedBlock, westNeighbor );
		}
		else if ( m_raycastResult.m_faceNormal.y == 1.0f )
		{
			HandleNorthFaceSelection( selectedBlock, northNeighbor );
		}
		else if ( m_raycastResult.m_faceNormal.y == -1.0f )
		{
			HandleSouthFaceSelection( selectedBlock, southNeighbor );
		}
		else if ( m_raycastResult.m_faceNormal.z == 1.0f )
		{
			HandleTopFaceSelection( selectedBlock, topNeighbor );
		}
		else if ( m_raycastResult.m_faceNormal.z == -1.0f )
		{
			HandleBottomFaceSelection( selectedBlock, bottomNeighbor );
		}

		if ( m_blockBeingDug )
		{
			if ( selectedBlock.m_blockIndex != m_indexOfBlockBeingDug )
			{
				m_blockBeingDug = false;
			}
			else
			{
				float currentTime = ( float ) GetCurrentTimeSeconds();
				float timeElapsed = currentTime - m_diggingStartTime;
				float duration = m_diggingEndTime - m_diggingStartTime;
				float fractionComplete = timeElapsed / duration;

				g_breakingSpriteIndex = ( int ) RangeMap( fractionComplete, 0.0f, 1.0f, 0.0f, 9.0f );

				if ( currentTime > m_diggingEndTime )
				{
					selectedBlock.m_chunk->m_blocks[ selectedBlock.m_blockIndex ].SetBlockType( BLOCK_TYPE_AIR );
					g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_BROKEN_SOUND_FILE ), 1.0f );
					selectedBlock.m_chunk->m_vboIsOutdated = true;

					if ( selectedBlock.m_chunk != westNeighbor.m_chunk ) // Selected block is on western edge of chunk
						westNeighbor.m_chunk->m_vboIsOutdated = true;

					if ( selectedBlock.m_chunk != eastNeighbor.m_chunk ) // Selected block is on eastern edge of chunk
						eastNeighbor.m_chunk->m_vboIsOutdated = true;

					if ( selectedBlock.m_chunk != southNeighbor.m_chunk ) // Selected block is on southern edge of chunk
						southNeighbor.m_chunk->m_vboIsOutdated = true;

					if ( selectedBlock.m_chunk != northNeighbor.m_chunk ) // Selected block is on northern edge of chunk
						northNeighbor.m_chunk->m_vboIsOutdated = true;

					m_blockBeingDug = false;
				}
			}
		}
	}
	else
	{
		g_blockHighlighted = false;
		m_blockBeingDug = false;
	}

	if ( !g_theInputSystem->GetLeftMouseButtonDownStatus() )
	{
		m_blockBeingDug = false;
	}

	if ( !g_theInputSystem->GetRightMouseButtonDownStatus() )
		g_waitForRightMouseButtonRelease = false;
}


//-----------------------------------------------------------------------------------------------
void World::SetPlayerBoundingBoxDrawPositions( const Vector3& playerBoundingBoxCenter)
{
	// Top of player bounding box
	m_playerBoundingBoxVertexes[ 0 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 1 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 2 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 3 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );

	// Bottom of player bounding box
	m_playerBoundingBoxVertexes[ 4 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 5 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 6 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 7 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );

	// Set positions of middle corner of player bounding box
	m_playerBoundingBoxVertexes[ 8 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z );
	m_playerBoundingBoxVertexes[ 9 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z );
	m_playerBoundingBoxVertexes[ 10 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z );
	m_playerBoundingBoxVertexes[ 11 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z );

	// North of player bounding box
	m_playerBoundingBoxVertexes[ 12 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 13 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 14 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 15 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );

	// South of player bounding box
	m_playerBoundingBoxVertexes[ 16 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 17 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z - PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 18 ] = Vector3( playerBoundingBoxCenter.x + PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
	m_playerBoundingBoxVertexes[ 19 ] = Vector3( playerBoundingBoxCenter.x - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.y - PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS, playerBoundingBoxCenter.z + PLAYER_BOUNDING_BOX_VERTICAL_RADIUS );
}


//-----------------------------------------------------------------------------------------------
void World::SetBlockBreakingVertexes( const IntVector3& blockGlobalCoords )
{
	// East face
	m_blockBreakingVertexes[ 0 ] = Vector3( ( float ) blockGlobalCoords.x + 1.01f, ( float ) blockGlobalCoords.y + 0.01f, ( float ) blockGlobalCoords.z + 0.01f );
	m_blockBreakingVertexes[ 1 ] = Vector3( ( float ) blockGlobalCoords.x + 1.01f, ( float ) blockGlobalCoords.y + 0.99f, ( float ) blockGlobalCoords.z + 0.01f );
	m_blockBreakingVertexes[ 2 ] = Vector3( ( float ) blockGlobalCoords.x + 1.01f, ( float ) blockGlobalCoords.y + 0.99f, ( float ) blockGlobalCoords.z + 0.99f );
	m_blockBreakingVertexes[ 3 ] = Vector3( ( float ) blockGlobalCoords.x + 1.01f, ( float ) blockGlobalCoords.y + 0.01f, ( float ) blockGlobalCoords.z + 0.99f );

	// West face
	m_blockBreakingVertexes[ 4 ] = Vector3( ( float ) blockGlobalCoords.x - 0.01f, ( float ) blockGlobalCoords.y + 0.99f, ( float ) blockGlobalCoords.z + 0.01f );
	m_blockBreakingVertexes[ 5 ] = Vector3( ( float ) blockGlobalCoords.x - 0.01f, ( float ) blockGlobalCoords.y + 0.01f, ( float ) blockGlobalCoords.z + 0.01f );
	m_blockBreakingVertexes[ 6 ] = Vector3( ( float ) blockGlobalCoords.x - 0.01f, ( float ) blockGlobalCoords.y + 0.01f, ( float ) blockGlobalCoords.z + 0.99f );
	m_blockBreakingVertexes[ 7 ] = Vector3( ( float ) blockGlobalCoords.x - 0.01f, ( float ) blockGlobalCoords.y + 0.99f, ( float ) blockGlobalCoords.z + 0.99f );

	// North face
	m_blockBreakingVertexes[ 8 ] = Vector3( ( float ) blockGlobalCoords.x + 0.99f, ( float ) blockGlobalCoords.y + 1.01f, ( float ) blockGlobalCoords.z + 0.99f );
	m_blockBreakingVertexes[ 9 ] = Vector3( ( float ) blockGlobalCoords.x + 0.01f, ( float ) blockGlobalCoords.y + 1.01f, ( float ) blockGlobalCoords.z + 0.99f );
	m_blockBreakingVertexes[ 10 ] = Vector3( ( float ) blockGlobalCoords.x + 0.01f, ( float ) blockGlobalCoords.y + 1.01f, ( float ) blockGlobalCoords.z + 0.01f );
	m_blockBreakingVertexes[ 11 ] = Vector3( ( float ) blockGlobalCoords.x + 0.99f, ( float ) blockGlobalCoords.y + 1.01f, ( float ) blockGlobalCoords.z + 0.01f );

	// South face
	m_blockBreakingVertexes[ 12 ] = Vector3( ( float ) blockGlobalCoords.x + 0.99f, ( float ) blockGlobalCoords.y - 0.01f, ( float ) blockGlobalCoords.z + 0.99f );
	m_blockBreakingVertexes[ 13 ] = Vector3( ( float ) blockGlobalCoords.x + 0.01f, ( float ) blockGlobalCoords.y - 0.01f, ( float ) blockGlobalCoords.z + 0.99f );
	m_blockBreakingVertexes[ 14 ] = Vector3( ( float ) blockGlobalCoords.x + 0.01f, ( float ) blockGlobalCoords.y - 0.01f, ( float ) blockGlobalCoords.z + 0.01f );
	m_blockBreakingVertexes[ 15 ] = Vector3( ( float ) blockGlobalCoords.x + 0.99f, ( float ) blockGlobalCoords.y - 0.01f, ( float ) blockGlobalCoords.z + 0.01f );

	// Top face
	m_blockBreakingVertexes[ 16 ] = Vector3( ( float ) blockGlobalCoords.x + 0.01f, ( float ) blockGlobalCoords.y + 0.01f, ( float ) blockGlobalCoords.z + 1.01f );
	m_blockBreakingVertexes[ 17 ] = Vector3( ( float ) blockGlobalCoords.x + 0.01f, ( float ) blockGlobalCoords.y + 0.99f, ( float ) blockGlobalCoords.z + 1.01f );
	m_blockBreakingVertexes[ 18 ] = Vector3( ( float ) blockGlobalCoords.x + 0.99f, ( float ) blockGlobalCoords.y + 0.99f, ( float ) blockGlobalCoords.z + 1.01f );
	m_blockBreakingVertexes[ 19 ] = Vector3( ( float ) blockGlobalCoords.x + 0.99f, ( float ) blockGlobalCoords.y + 0.01f, ( float ) blockGlobalCoords.z + 1.01f );

	// Bottom face
	m_blockBreakingVertexes[ 20 ] = Vector3( ( float ) blockGlobalCoords.x + 0.01f, ( float ) blockGlobalCoords.y + 0.01f, ( float ) blockGlobalCoords.z - 0.01f );
	m_blockBreakingVertexes[ 21 ] = Vector3( ( float ) blockGlobalCoords.x + 0.99f, ( float ) blockGlobalCoords.y + 0.01f, ( float ) blockGlobalCoords.z - 0.01f );
	m_blockBreakingVertexes[ 22 ] = Vector3( ( float ) blockGlobalCoords.x + 0.99f, ( float ) blockGlobalCoords.y + 0.99f, ( float ) blockGlobalCoords.z - 0.01f );
	m_blockBreakingVertexes[ 23 ] = Vector3( ( float ) blockGlobalCoords.x + 0.01f, ( float ) blockGlobalCoords.y + 0.99f, ( float ) blockGlobalCoords.z - 0.01f );
}


//-----------------------------------------------------------------------------------------------
// Based on code by Squirrel Eiserloh and Nick Dorbin
RaycastResult3D World::AWRaycast3D( const Vector3& start, const Vector3& end )
{
	RaycastResult3D raycast = RaycastResult3D();
	BlockInfo blockInfoStartingBlock = GetBlockInfoForWorldPosition( start );
	Vector3 currentBlockPosition( floor( start.x ), floor( start.y ), floor( start.z ) );
	BlockInfo blockInfoEndingBlock = GetBlockInfoForWorldPosition( end );

	if ( !( blockInfoStartingBlock.IsValid() ) || !( blockInfoEndingBlock.IsValid() ) )
	{
		raycast.m_didHit = false;
		raycast.m_fractionComplete = 0.0f;
		raycast.m_faceNormal = Vector3::ZERO;
		return raycast;
	}

	if ( blockInfoStartingBlock.IsSolid() )
	{
		raycast.m_blockInfo = blockInfoStartingBlock;
		raycast.m_blockInfoBeforeHit = blockInfoStartingBlock;
		raycast.m_didHit = true;
		raycast.m_fractionComplete = -1.0f;
		raycast.m_hitPosition = start;
		raycast.m_faceNormal = Vector3::ZERO;
		return raycast;
	}
	else if ( start == end )
	{
		raycast.m_didHit = false;
		raycast.m_fractionComplete = 0.0f;
		return raycast;
	}

	Vector3 displacementVector = end - start;

	// X
	float tDeltaX = abs( 1.0f / displacementVector.x );
	if ( !std::isfinite( tDeltaX ) )
	{
		tDeltaX = 9999;
	}
	int blockStepX = 0;
	if ( displacementVector.x > 0.0f )
	{
		blockStepX = 1;
	}
	else
	{
		blockStepX = -1;
	}
	int offsetToLeadingEdgeX = ( blockStepX + 1 ) / 2;
	float firstVerticalIntersectionX = currentBlockPosition.x + offsetToLeadingEdgeX;
	float tOfNextXCrossing = abs( firstVerticalIntersectionX - start.x ) * tDeltaX;

	// Y
	float tDeltaY = abs( 1.0f / displacementVector.y );
	if ( !std::isfinite( tDeltaY ) )
	{
		tDeltaY = 9999;
	}
	int blockStepY = 0;
	if ( displacementVector.y > 0.0f )
	{
		blockStepY = 1;
	}
	else
	{
		blockStepY = -1;
	}
	int offsetToLeadingEdgeY = ( blockStepY + 1 ) / 2;
	float firstVerticalIntersectionY = currentBlockPosition.y + offsetToLeadingEdgeY;
	float tOfNextYCrossing = abs( firstVerticalIntersectionY - start.y ) * tDeltaY;

	// Z
	float tDeltaZ = abs( 1.0f / displacementVector.z );
	if ( !std::isfinite( tDeltaZ ) )
	{
		tDeltaZ = 9999;
	}
	int blockStepZ = 0;
	if ( displacementVector.z > 0.0f )
	{
		blockStepZ = 1;
	}
	else
	{
		blockStepZ = -1;
	}
	int offsetToLeadingEdgeZ = ( blockStepZ + 1 ) / 2;
	float firstVerticalIntersectionZ = currentBlockPosition.z + offsetToLeadingEdgeZ;
	float tOfNextZCrossing = abs( firstVerticalIntersectionZ - start.z ) * tDeltaZ;

	for ( ; ; )
	{
		float lowestCrossing = min( tOfNextXCrossing, tOfNextYCrossing );
		lowestCrossing = min( lowestCrossing, tOfNextZCrossing );

		// X
		if ( lowestCrossing == tOfNextXCrossing )
		{
			if ( lowestCrossing > 1.0f )
			{
				// Did not hit
				raycast.m_didHit = false;
				raycast.m_fractionComplete = 1.0f;
				return raycast;
			}
			currentBlockPosition.x += blockStepX;
			BlockInfo currentBlockInfo = GetBlockInfoForWorldPosition( currentBlockPosition );

			if ( currentBlockInfo.IsSolid() )
			{
				// Did hit
				raycast.m_didHit = true;
				raycast.m_hitPosition = start + ( displacementVector * tOfNextXCrossing );
				raycast.m_fractionComplete = tOfNextXCrossing;
				raycast.m_blockInfo = currentBlockInfo;
				if ( displacementVector.x > 0.0f )
				{
					raycast.m_faceNormal = Vector3( -1.0f, 0.0f, 0.0f );
					raycast.m_blockInfoBeforeHit = currentBlockInfo.GetWestNeighborInfo();
				}
				else
				{
					raycast.m_faceNormal = Vector3( 1.0f, 0.0f, 0.0f );
					raycast.m_blockInfoBeforeHit = currentBlockInfo.GetEastNeighborInfo();
				}
				raycast.m_hitPosition += ( raycast.m_faceNormal * 0.0005f );
				return raycast;
			}
			else
			{
				tOfNextXCrossing += tDeltaX;
			}
		}

		// Y
		if ( lowestCrossing == tOfNextYCrossing )
		{
			if ( lowestCrossing > 1.0f )
			{
				// Did not hit
				raycast.m_didHit = false;
				raycast.m_fractionComplete = 1.0f;
				return raycast;
			}
			currentBlockPosition.y += blockStepY;
			BlockInfo currentBlockInfo = GetBlockInfoForWorldPosition( currentBlockPosition );

			if ( currentBlockInfo.IsSolid() )
			{
				// Did hit
				raycast.m_didHit = true;
				raycast.m_hitPosition = start + ( displacementVector * tOfNextYCrossing );
				raycast.m_fractionComplete = tOfNextYCrossing;
				raycast.m_blockInfo = currentBlockInfo;
				if ( displacementVector.y > 0.0f )
				{
					raycast.m_faceNormal = Vector3( 0.0f, -1.0f, 0.0f );
					raycast.m_blockInfoBeforeHit = currentBlockInfo.GetSouthNeighborInfo();
				}
				else
				{
					raycast.m_faceNormal = Vector3( 0.0f, 1.0f, 0.0f );
					raycast.m_blockInfoBeforeHit = currentBlockInfo.GetNorthNeighborInfo();
				}
				raycast.m_hitPosition += ( raycast.m_faceNormal * 0.0005f );
				return raycast;
			}
			else
			{
				tOfNextYCrossing += tDeltaY;
			}
		}

		// Z
		if ( lowestCrossing == tOfNextZCrossing )
		{
			if ( lowestCrossing > 1.0f )
			{
				// Did not hit
				raycast.m_didHit = false;
				raycast.m_fractionComplete = 1.0f;
				return raycast;
			}
			currentBlockPosition.z += blockStepZ;
			BlockInfo currentBlockInfo = GetBlockInfoForWorldPosition( currentBlockPosition );

			if ( currentBlockInfo.IsSolid() )
			{
				// Did hit
				raycast.m_didHit = true;
				raycast.m_hitPosition = start + ( displacementVector * tOfNextZCrossing );
				raycast.m_fractionComplete = tOfNextZCrossing;
				raycast.m_blockInfo = currentBlockInfo;
				if ( displacementVector.z > 0.0f )
				{
					raycast.m_faceNormal = Vector3( 0.0f, 0.0f, -1.0f );
					raycast.m_blockInfoBeforeHit = currentBlockInfo.GetBottomNeighborInfo();
				}
				else
				{
					raycast.m_faceNormal = Vector3( 0.0f, 0.0f, 1.0f );
					raycast.m_blockInfoBeforeHit = currentBlockInfo.GetTopNeighborInfo();
				}
				raycast.m_hitPosition += ( raycast.m_faceNormal * 0.0005f );
				return raycast;
			}
			else
			{
				tOfNextZCrossing += tDeltaZ;
			}
		}
	}

	//return raycast;
}


//-----------------------------------------------------------------------------------------------
void World::DeactivateFarthestObsoleteChunk()
{
	bool foundAnObsoleteChunk = false;
	float distanceToFarthestObsoleteChunk = 0.0f;
	IntVector2 chunkCoordsFarthestObsoleteChunk;
	Chunk* chunkPointerFarthestObsoleteChunk = nullptr;

	for ( auto activeChunkIterator = m_activeChunks.begin(); activeChunkIterator != m_activeChunks.end(); ++activeChunkIterator )
	{
		float distanceToChunk = CalculateDistanceBetweenPoints( Vector2( g_theGame->m_player.m_position.x, g_theGame->m_player.m_position.y ), activeChunkIterator->second->m_chunkCenterPos );

		// Check if distance to chunk is greater than the active radius and the current farthest distance to farthest obsolete chunk
		if ( ( distanceToChunk > ACTIVE_RADIUS ) && ( distanceToChunk > distanceToFarthestObsoleteChunk ) )
		{
			foundAnObsoleteChunk = true;
			distanceToFarthestObsoleteChunk = distanceToChunk;
			chunkCoordsFarthestObsoleteChunk = activeChunkIterator->first;
			chunkPointerFarthestObsoleteChunk = activeChunkIterator->second;
		}
	}

	if ( foundAnObsoleteChunk )
	{
		if ( g_saveLoadChunks )
		{
			// Save the chunk to disk
			std::vector< unsigned char > rleBuffer;
			unsigned char currentType = ( unsigned char ) chunkPointerFarthestObsoleteChunk->m_blocks[ 0 ].m_blockType;
			int numOfType = 1;

			for ( int blockIndex = 1; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
			{
				if ( ( unsigned char ) chunkPointerFarthestObsoleteChunk->m_blocks[ blockIndex ].m_blockType == currentType )
				{
					// Check if numOfType is at 255, max value a byte can hold
					if ( numOfType == 255 )
					{
						rleBuffer.push_back( currentType );
						rleBuffer.push_back( ( const unsigned char ) numOfType );

						numOfType = 1;
					}
					else
					{
						++numOfType;
					}
				}
				else // Next block is of a different type
				{
					rleBuffer.push_back( currentType );
					rleBuffer.push_back( ( const unsigned char ) numOfType );

					numOfType = 1;
					currentType = ( unsigned char ) chunkPointerFarthestObsoleteChunk->m_blocks[ blockIndex ].m_blockType;
				}

				if ( ( blockIndex + 1 ) == NUM_BLOCKS_PER_CHUNK )
				{
					rleBuffer.push_back( currentType );
					rleBuffer.push_back( ( const unsigned char ) numOfType );
				}
			}

			const std::string& filePath = Stringf( "Data/SaveSlot0/Chunk(%d,%d).chunk", chunkCoordsFarthestObsoleteChunk.x, chunkCoordsFarthestObsoleteChunk.y );

			bool success = SaveBufferToBinaryFile( filePath, rleBuffer );

			if ( success )
				m_numSavedChunks++;
		}

		// Remove the chunk
		DisconnectChunk( chunkPointerFarthestObsoleteChunk );
		m_activeChunks.erase( chunkCoordsFarthestObsoleteChunk );
	}
}


//-----------------------------------------------------------------------------------------------
void World::DisconnectChunk( Chunk* chunk )
{
	IntVector2 chunkCoords( chunk->m_chunkCoords.x, chunk->m_chunkCoords.y );

	IntVector2 northNeighborChunkCoords( chunkCoords.x, chunkCoords.y + 1 );
	IntVector2 southNeighborChunkCoords( chunkCoords.x, chunkCoords.y - 1 );
	IntVector2 eastNeighborChunkCoords( chunkCoords.x + 1, chunkCoords.y );
	IntVector2 westNeighborChunkCoords( chunkCoords.x - 1, chunkCoords.y );

	// North neighbor
	if ( m_activeChunks.find( northNeighborChunkCoords ) != m_activeChunks.end() )
	{
		Chunk* northNeighbor = m_activeChunks.find( northNeighborChunkCoords )->second;

		northNeighbor->m_southNeighbor = nullptr;
	}

	// South neighbor
	if ( m_activeChunks.find( southNeighborChunkCoords ) != m_activeChunks.end() )
	{
		Chunk* southNeighbor = m_activeChunks.find( southNeighborChunkCoords )->second;

		southNeighbor->m_northNeighbor = nullptr;
	}

	// East neighbor
	if ( m_activeChunks.find( eastNeighborChunkCoords ) != m_activeChunks.end() )
	{
		Chunk* eastNeighbor = m_activeChunks.find( eastNeighborChunkCoords )->second;

		eastNeighbor->m_westNeighbor = nullptr;
	}

	// West neighbor
	if ( m_activeChunks.find( westNeighborChunkCoords ) != m_activeChunks.end() )
	{
		Chunk* westNeighbor = m_activeChunks.find( westNeighborChunkCoords )->second;

		westNeighbor->m_eastNeighbor = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
bool World::ActivateNearestMissingChunk()
{
	bool foundAMissingChunk = false;
	float distanceToNearestMissingChunk = ACTIVE_RADIUS;
	IntVector2 chunkCoordsNearestMissingChunk;

	float minActiveRegionWorldX = g_theGame->m_player.m_position.x - ACTIVE_RADIUS;
	float maxActiveRegionWorldX = g_theGame->m_player.m_position.x + ACTIVE_RADIUS;
	float minActiveRegionWorldY = g_theGame->m_player.m_position.y - ACTIVE_RADIUS;
	float maxActiveRegionWorldY = g_theGame->m_player.m_position.y + ACTIVE_RADIUS;
	int minChunkX = ( int ) floor( minActiveRegionWorldX ) / BLOCKS_WIDE_X;
	int maxChunkX = ( int ) ceil( maxActiveRegionWorldX ) / BLOCKS_WIDE_X;
	int minChunkY = ( int ) floor( minActiveRegionWorldY ) / BLOCKS_WIDE_Y;
	int maxChunkY = ( int ) ceil( maxActiveRegionWorldY ) / BLOCKS_WIDE_Y;

	for ( int chunkX = minChunkX; chunkX <= maxChunkX; ++chunkX )
	{
		for ( int chunkY = minChunkY; chunkY <= maxChunkY; ++chunkY )
		{
			Vector2 chunkWorldMins( chunkX * ( float ) BLOCKS_WIDE_X, chunkY * ( float ) BLOCKS_WIDE_Y );
			Vector2 chunkCenterPos( chunkWorldMins.x + ( BLOCKS_WIDE_X / 2 ), chunkWorldMins.y + ( BLOCKS_WIDE_Y / 2 ) );
			float distanceToChunk = CalculateDistanceBetweenPoints( Vector2( g_theGame->m_player.m_position.x, g_theGame->m_player.m_position.y ), chunkCenterPos );

			// Check if distance to chunk is less than current closest distance to nearest missing chunk and less than or equal to the active radius
			if ( distanceToChunk < distanceToNearestMissingChunk )
			{
				// Check if chunk is already active
				if ( m_activeChunks.find( IntVector2( chunkX, chunkY ) ) == m_activeChunks.end() )
				{
					// Chunk is not active yet
					foundAMissingChunk = true;
					distanceToNearestMissingChunk = distanceToChunk;
					chunkCoordsNearestMissingChunk = IntVector2( chunkX, chunkY );
				}
			}
		}
	}

	if ( foundAMissingChunk )
	{
		if ( g_saveLoadChunks )
		{
			// First check if the chunk already exists on disk
			const std::string filePath = Stringf( "Data/SaveSlot0/Chunk(%d,%d).chunk", chunkCoordsNearestMissingChunk.x, chunkCoordsNearestMissingChunk.y );

			std::vector< unsigned char > rleBuffer;
			bool success = LoadBinaryFileToBuffer( filePath, rleBuffer );

			if ( success ) // Load chunk from disk
			{
				Block loadedBlocks[ NUM_BLOCKS_PER_CHUNK ];

				int currentBlockIndex = 0;
				int numLoadedBlocks = 0;

				for ( unsigned int vectorIndex = 0; vectorIndex < rleBuffer.size(); vectorIndex += 2 )
				{
					numLoadedBlocks += rleBuffer[ vectorIndex + 1 ];
					while ( currentBlockIndex < numLoadedBlocks )
					{
						loadedBlocks[ currentBlockIndex ].m_blockType = rleBuffer[ vectorIndex ];
						currentBlockIndex++;
					}
				}

				m_activeChunks[ chunkCoordsNearestMissingChunk ] = new Chunk( chunkCoordsNearestMissingChunk, loadedBlocks );
				ConnectChunk( m_activeChunks[ chunkCoordsNearestMissingChunk ] );
				m_numLoadedChunks++;
			}
			else // Generate chunk
			{
				m_activeChunks[ chunkCoordsNearestMissingChunk ] = new Chunk( chunkCoordsNearestMissingChunk );
				ConnectChunk( m_activeChunks[ chunkCoordsNearestMissingChunk ] );
			}
		}
		else // Generate chunk
		{
			m_activeChunks[ chunkCoordsNearestMissingChunk ] = new Chunk( chunkCoordsNearestMissingChunk );
			ConnectChunk( m_activeChunks[ chunkCoordsNearestMissingChunk ] );
		}
	}

	return foundAMissingChunk;
}


//-----------------------------------------------------------------------------------------------
void World::ConnectChunk( Chunk* chunk )
{
	IntVector2 chunkCoords( chunk->m_chunkCoords.x, chunk->m_chunkCoords.y );

	IntVector2 northNeighborChunkCoords( chunkCoords.x, chunkCoords.y + 1 );
	IntVector2 southNeighborChunkCoords( chunkCoords.x, chunkCoords.y - 1 );
	IntVector2 eastNeighborChunkCoords( chunkCoords.x + 1, chunkCoords.y );
	IntVector2 westNeighborChunkCoords( chunkCoords.x - 1, chunkCoords.y );

	// North neighbor
	if ( m_activeChunks.find( northNeighborChunkCoords ) != m_activeChunks.end() )
	{
		Chunk* northNeighbor = m_activeChunks.find( northNeighborChunkCoords )->second;

		chunk->m_northNeighbor = northNeighbor;
		northNeighbor->m_southNeighbor = chunk;
		northNeighbor->m_vboIsOutdated = true; // Dirty VA of neighbor
	}

	// South neighbor
	if ( m_activeChunks.find( southNeighborChunkCoords ) != m_activeChunks.end() )
	{
		Chunk* southNeighbor = m_activeChunks.find( southNeighborChunkCoords )->second;

		chunk->m_southNeighbor = southNeighbor;
		southNeighbor->m_northNeighbor = chunk;
		southNeighbor->m_vboIsOutdated = true; // Dirty VA of neighbor
	}

	// East neighbor
	if ( m_activeChunks.find( eastNeighborChunkCoords ) != m_activeChunks.end() )
	{
		Chunk* eastNeighbor = m_activeChunks.find( eastNeighborChunkCoords )->second;

		chunk->m_eastNeighbor = eastNeighbor;
		eastNeighbor->m_westNeighbor = chunk;
		eastNeighbor->m_vboIsOutdated = true; // Dirty VA of neighbor
	}

	// West neighbor
	if ( m_activeChunks.find( westNeighborChunkCoords ) != m_activeChunks.end() )
	{
		Chunk* westNeighbor = m_activeChunks.find( westNeighborChunkCoords )->second;

		chunk->m_westNeighbor = westNeighbor;
		westNeighbor->m_eastNeighbor = chunk;
		westNeighbor->m_vboIsOutdated = true; // Dirty VA of neighbor
	}
}


//-----------------------------------------------------------------------------------------------
void World::UpdateChunks()
{
	int numRenderedChunks = 0;
	for ( auto activeChunkIterator = m_activeChunks.begin(); activeChunkIterator != m_activeChunks.end(); ++activeChunkIterator )
	{
		activeChunkIterator->second->Update();

		if ( activeChunkIterator->second->m_isRendered )
			numRenderedChunks++;
	}

	m_numRenderedChunks = numRenderedChunks;
}


//-----------------------------------------------------------------------------------------------
void World::SaveActiveChunksToDisk()
{
	if ( !g_saveLoadChunks )
	{
		m_activeChunksSavedToDisk = true;
		return;
	}

	for ( auto activeChunkIterator = m_activeChunks.begin(); activeChunkIterator != m_activeChunks.end(); activeChunkIterator++ )
	{
		// Save the chunk to disk
		std::vector< unsigned char > rleBuffer;
		unsigned char currentType = ( unsigned char ) activeChunkIterator->second->m_blocks[ 0 ].m_blockType;
		int numOfType = 1;

		for ( int blockIndex = 1; blockIndex < NUM_BLOCKS_PER_CHUNK; ++blockIndex )
		{
			if ( ( unsigned char ) activeChunkIterator->second->m_blocks[ blockIndex ].m_blockType == currentType )
			{
				// Check if numOfType is at 255, max value a byte can hold
				if ( numOfType == 255 )
				{
					rleBuffer.push_back( currentType );
					rleBuffer.push_back( ( const unsigned char ) numOfType );

					numOfType = 1;
				}
				else
				{
					++numOfType;
				}
			}
			else // Next block is of a different type
			{
				rleBuffer.push_back( currentType );
				rleBuffer.push_back( ( const unsigned char ) numOfType );

				numOfType = 1;
				currentType = ( unsigned char ) activeChunkIterator->second->m_blocks[ blockIndex ].m_blockType;
			}

			if ( ( blockIndex + 1 ) == NUM_BLOCKS_PER_CHUNK )
			{
				rleBuffer.push_back( currentType );
				rleBuffer.push_back( ( const unsigned char ) numOfType );
			}
		}

		const std::string& filePath = Stringf( "Data/SaveSlot0/Chunk(%d,%d).chunk", activeChunkIterator->first.x, activeChunkIterator->first.y );

		bool success = SaveBufferToBinaryFile( filePath, rleBuffer );

		success;
	}

	m_activeChunksSavedToDisk = true;
}


//-----------------------------------------------------------------------------------------------
void World::SavePlayerToDisk()
{
	std::vector< float > buffer;

	// Push player position floats to buffer
	buffer.push_back( g_theGame->m_camera.m_position.x );
	buffer.push_back( g_theGame->m_camera.m_position.y );
	buffer.push_back( g_theGame->m_camera.m_position.z );

	// Push player orientation floats to buffer
	buffer.push_back( g_theGame->m_camera.m_orientation.m_rollDegreesAboutX );
	buffer.push_back( g_theGame->m_camera.m_orientation.m_pitchDegreesAboutY );
	buffer.push_back( g_theGame->m_camera.m_orientation.m_yawDegreesAboutZ );

	const std::string& filePath = Stringf( "Data/SaveSlot0/PlayerInfo.player" );

	bool success = SaveBufferToBinaryFileFloat( filePath, buffer );
	success;

	m_playerSavedToDisk = true;
}


//-----------------------------------------------------------------------------------------------
IntVector2 World::GetChunkCoordsForWorldPosition( const Vector3& worldPosition )
{
	int chunkX = ( int ) floor( worldPosition.x / BLOCKS_WIDE_X );
	int chunkY = ( int ) floor( worldPosition.y / BLOCKS_WIDE_Y );

	return IntVector2( chunkX, chunkY );
}


//-----------------------------------------------------------------------------------------------
IntVector3 World::GetBlockLocalCoordsForWorldPosition( const Vector3& worldPosition )
{
	IntVector2 chunkCoordsForWorldPosition = GetChunkCoordsForWorldPosition( worldPosition );
	return IntVector3( ( int ) abs( worldPosition.x - ( chunkCoordsForWorldPosition.x * BLOCKS_WIDE_X ) ), ( int ) abs( worldPosition.y - ( chunkCoordsForWorldPosition.y * BLOCKS_WIDE_Y ) ), ( int ) worldPosition.z );
}


//-----------------------------------------------------------------------------------------------
IntVector3 World::GetBlockGlobalCoordsForWorldPosition( const Vector3& worldPosition )
{
	return IntVector3( ( int ) floor( worldPosition.x ), ( int ) floor( worldPosition.y ), ( int ) floor( worldPosition.z ) );
}


//-----------------------------------------------------------------------------------------------
bool World::AreBlockLocalCoordsWithinChunk( const IntVector3& blockLocalCoords)
{
	if ( blockLocalCoords.x >= 0 &&
		 blockLocalCoords.x <= ( BLOCKS_WIDE_X - 1 ) &&
		 blockLocalCoords.y >= 0 &&
		 blockLocalCoords.y <= ( BLOCKS_WIDE_Y - 1 ) &&
		 blockLocalCoords.z >= 0 &&
		 blockLocalCoords.z <= ( BLOCKS_TALL_Z - 1 ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
bool World::PlayerGrounded()
{
	Vector3 normalizedMovementDirection( 0.0f, 0.0f, -1.0f );
	normalizedMovementDirection.Normalize();

	float shortestRaycastFractionComplete = 1.0f;

	Vector3 raycastStart = m_playerBoundingBoxVertexes[ 4 ];
	Vector3 raycastEnd = m_playerBoundingBoxVertexes[ 4 ] + ( normalizedMovementDirection * 0.1f );
	float playerRaycastFractionComplete = AWRaycast3D( raycastStart, raycastEnd ).m_fractionComplete;
	if ( playerRaycastFractionComplete < shortestRaycastFractionComplete )
		shortestRaycastFractionComplete = playerRaycastFractionComplete;

	raycastStart = m_playerBoundingBoxVertexes[ 5 ];
	raycastEnd = m_playerBoundingBoxVertexes[ 5 ] + ( normalizedMovementDirection * 0.1f );
	playerRaycastFractionComplete = AWRaycast3D( raycastStart, raycastEnd ).m_fractionComplete;
	if ( playerRaycastFractionComplete < shortestRaycastFractionComplete )
		shortestRaycastFractionComplete = playerRaycastFractionComplete;

	raycastStart = m_playerBoundingBoxVertexes[ 6 ];
	raycastEnd = m_playerBoundingBoxVertexes[ 6 ] + ( normalizedMovementDirection * 0.1f );
	playerRaycastFractionComplete = AWRaycast3D( raycastStart, raycastEnd ).m_fractionComplete;
	if ( playerRaycastFractionComplete < shortestRaycastFractionComplete )
		shortestRaycastFractionComplete = playerRaycastFractionComplete;

	raycastStart = m_playerBoundingBoxVertexes[ 7 ];
	raycastEnd = m_playerBoundingBoxVertexes[ 7 ] + ( normalizedMovementDirection * 0.1f );
	playerRaycastFractionComplete = AWRaycast3D( raycastStart, raycastEnd ).m_fractionComplete;
	if ( playerRaycastFractionComplete < shortestRaycastFractionComplete )
		shortestRaycastFractionComplete = playerRaycastFractionComplete;

	if ( shortestRaycastFractionComplete < 0.10f )
		return true;
	else
		return false;
}


//-----------------------------------------------------------------------------------------------
BlockInfo World::GetBlockInfoForWorldPosition( const Vector3& worldPosition )
{
	IntVector2 chunkCoordsForWorldPosition = GetChunkCoordsForWorldPosition( worldPosition );
	std::map< IntVector2, Chunk* >::iterator found = m_activeChunks.find( chunkCoordsForWorldPosition );

	if ( found != m_activeChunks.end() )
	{
		Chunk* foundChunk = found->second;
		IntVector3 blockLocalCoordsForWorldPosition = GetBlockLocalCoordsForWorldPosition( worldPosition );

		int blockIndex = foundChunk->GetBlockIndexForLocalCoords( blockLocalCoordsForWorldPosition );
		BlockInfo blockInfo( foundChunk, blockIndex );
		return blockInfo;
	}

	return BlockInfo();
}


//-----------------------------------------------------------------------------------------------
void World::HandleNorthFaceSelection( BlockInfo selectedBlock, BlockInfo northNeighbor )
{
	IntVector3 blockGlobalCoordsForHitPosition = GetBlockGlobalCoordsForWorldPosition( Vector3( m_raycastResult.m_hitPosition.x, m_raycastResult.m_hitPosition.y - 0.5f, m_raycastResult.m_hitPosition.z ) );

	// Highlight north face
	g_blockHighlightQuadVertex1 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.99f, ( float ) blockGlobalCoordsForHitPosition.y + 1.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.99f );
	g_blockHighlightQuadVertex2 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 1.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.99f );
	g_blockHighlightQuadVertex3 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 1.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.01f );
	g_blockHighlightQuadVertex4 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.99f, ( float ) blockGlobalCoordsForHitPosition.y + 1.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.01f );

	if ( g_theInputSystem->GetRightMouseButtonDownStatus() && !g_waitForRightMouseButtonRelease )
	{
		northNeighbor.m_chunk->m_blocks[ northNeighbor.m_blockIndex ].SetBlockType( ( BlockType ) g_hudSelectedBlockType );
		g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_PLACE_SOUND_FILE ), 1.0f );
		northNeighbor.m_chunk->m_vboIsOutdated = true;
		g_waitForRightMouseButtonRelease = true;
	}

	CheckForBlockDigging( selectedBlock, blockGlobalCoordsForHitPosition );
}


//-----------------------------------------------------------------------------------------------
void World::HandleSouthFaceSelection( BlockInfo selectedBlock, BlockInfo southNeighbor )
{
	IntVector3 blockGlobalCoordsForHitPosition = GetBlockGlobalCoordsForWorldPosition( Vector3( m_raycastResult.m_hitPosition.x, m_raycastResult.m_hitPosition.y + 0.5f, m_raycastResult.m_hitPosition.z ) );

	// Highlight south face
	g_blockHighlightQuadVertex1 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.99f, ( float ) blockGlobalCoordsForHitPosition.y - 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.99f );
	g_blockHighlightQuadVertex2 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.01f, ( float ) blockGlobalCoordsForHitPosition.y - 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.99f );
	g_blockHighlightQuadVertex3 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.01f, ( float ) blockGlobalCoordsForHitPosition.y - 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.01f );
	g_blockHighlightQuadVertex4 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.99f, ( float ) blockGlobalCoordsForHitPosition.y - 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.01f );

	if ( g_theInputSystem->GetRightMouseButtonDownStatus() && !g_waitForRightMouseButtonRelease )
	{
		southNeighbor.m_chunk->m_blocks[ southNeighbor.m_blockIndex ].SetBlockType( ( BlockType ) g_hudSelectedBlockType );
		g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_PLACE_SOUND_FILE ), 1.0f );
		southNeighbor.m_chunk->m_vboIsOutdated = true;
		g_waitForRightMouseButtonRelease = true;
	}

	CheckForBlockDigging( selectedBlock, blockGlobalCoordsForHitPosition );
}


//-----------------------------------------------------------------------------------------------
void World::HandleEastFaceSelection( BlockInfo selectedBlock, BlockInfo eastNeighbor )
{
	IntVector3 blockGlobalCoordsForHitPosition = GetBlockGlobalCoordsForWorldPosition( Vector3( m_raycastResult.m_hitPosition.x - 0.5f, m_raycastResult.m_hitPosition.y, m_raycastResult.m_hitPosition.z ) );

	// Highlight east face
	g_blockHighlightQuadVertex1 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 1.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.01f );
	g_blockHighlightQuadVertex2 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 1.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.99f, ( float ) blockGlobalCoordsForHitPosition.z + 0.01f );
	g_blockHighlightQuadVertex3 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 1.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.99f, ( float ) blockGlobalCoordsForHitPosition.z + 0.99f );
	g_blockHighlightQuadVertex4 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 1.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.99f );

	if ( g_theInputSystem->GetRightMouseButtonDownStatus() && !g_waitForRightMouseButtonRelease )
	{
		eastNeighbor.m_chunk->m_blocks[ eastNeighbor.m_blockIndex ].SetBlockType( ( BlockType ) g_hudSelectedBlockType );
		g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_PLACE_SOUND_FILE ), 1.0f );
		eastNeighbor.m_chunk->m_vboIsOutdated = true;
		g_waitForRightMouseButtonRelease = true;
	}

	CheckForBlockDigging( selectedBlock, blockGlobalCoordsForHitPosition );
}


//-----------------------------------------------------------------------------------------------
void World::HandleWestFaceSelection( BlockInfo selectedBlock, BlockInfo westNeighbor )
{
	IntVector3 blockGlobalCoordsForHitPosition = GetBlockGlobalCoordsForWorldPosition( Vector3( m_raycastResult.m_hitPosition.x + 0.5f, m_raycastResult.m_hitPosition.y, m_raycastResult.m_hitPosition.z ) );

	// Highlight west face
	g_blockHighlightQuadVertex1 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x - 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.99f, ( float ) blockGlobalCoordsForHitPosition.z + 0.01f );
	g_blockHighlightQuadVertex2 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x - 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.01f );
	g_blockHighlightQuadVertex3 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x - 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 0.99f );
	g_blockHighlightQuadVertex4 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x - 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.99f, ( float ) blockGlobalCoordsForHitPosition.z + 0.99f );

	if ( g_theInputSystem->GetRightMouseButtonDownStatus() && !g_waitForRightMouseButtonRelease )
	{
		westNeighbor.m_chunk->m_blocks[ westNeighbor.m_blockIndex ].SetBlockType( ( BlockType ) g_hudSelectedBlockType );
		g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_PLACE_SOUND_FILE ), 1.0f );
		westNeighbor.m_chunk->m_vboIsOutdated = true;
		g_waitForRightMouseButtonRelease = true;
	}

	CheckForBlockDigging( selectedBlock, blockGlobalCoordsForHitPosition );
}


//-----------------------------------------------------------------------------------------------
void World::HandleTopFaceSelection( BlockInfo selectedBlock, BlockInfo topNeighbor )
{
	IntVector3 blockGlobalCoordsForHitPosition = GetBlockGlobalCoordsForWorldPosition( Vector3( m_raycastResult.m_hitPosition.x, m_raycastResult.m_hitPosition.y, m_raycastResult.m_hitPosition.z - 0.5f ) );

	// Highlight top face
	g_blockHighlightQuadVertex1 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 1.01f );
	g_blockHighlightQuadVertex2 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.99f, ( float ) blockGlobalCoordsForHitPosition.z + 1.01f );
	g_blockHighlightQuadVertex3 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.99f, ( float ) blockGlobalCoordsForHitPosition.y + 0.99f, ( float ) blockGlobalCoordsForHitPosition.z + 1.01f );
	g_blockHighlightQuadVertex4 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.99f, ( float ) blockGlobalCoordsForHitPosition.y + 0.01f, ( float ) blockGlobalCoordsForHitPosition.z + 1.01f );

	if ( g_theInputSystem->GetRightMouseButtonDownStatus() && !g_waitForRightMouseButtonRelease )
	{
		topNeighbor.m_chunk->m_blocks[ topNeighbor.m_blockIndex ].SetBlockType( ( BlockType ) g_hudSelectedBlockType );
		g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_PLACE_SOUND_FILE ), 1.0f );
		topNeighbor.m_chunk->m_vboIsOutdated = true;
		g_waitForRightMouseButtonRelease = true;
	}

	CheckForBlockDigging( selectedBlock, blockGlobalCoordsForHitPosition );
}


//-----------------------------------------------------------------------------------------------
void World::HandleBottomFaceSelection( BlockInfo selectedBlock, BlockInfo bottomNeighbor )
{
	IntVector3 blockGlobalCoordsForHitPosition = GetBlockGlobalCoordsForWorldPosition( Vector3( m_raycastResult.m_hitPosition.x, m_raycastResult.m_hitPosition.y, m_raycastResult.m_hitPosition.z + 0.5f ) );

	// Highlight bottom face
	g_blockHighlightQuadVertex1 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.01f, ( float ) blockGlobalCoordsForHitPosition.z - 0.01f );
	g_blockHighlightQuadVertex2 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.99f, ( float ) blockGlobalCoordsForHitPosition.y + 0.01f, ( float ) blockGlobalCoordsForHitPosition.z - 0.01f );
	g_blockHighlightQuadVertex3 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.99f, ( float ) blockGlobalCoordsForHitPosition.y + 0.99f, ( float ) blockGlobalCoordsForHitPosition.z - 0.01f );
	g_blockHighlightQuadVertex4 = Vector3( ( float ) blockGlobalCoordsForHitPosition.x + 0.01f, ( float ) blockGlobalCoordsForHitPosition.y + 0.99f, ( float ) blockGlobalCoordsForHitPosition.z - 0.01f );

	if ( g_theInputSystem->GetRightMouseButtonDownStatus() && !g_waitForRightMouseButtonRelease )
	{
		bottomNeighbor.m_chunk->m_blocks[ bottomNeighbor.m_blockIndex ].SetBlockType( ( BlockType ) g_hudSelectedBlockType );
		g_theAudioSystem->PlaySound( g_theAudioSystem->CreateOrGetSound( BLOCK_PLACE_SOUND_FILE ), 1.0f );
		bottomNeighbor.m_chunk->m_vboIsOutdated = true;
		g_waitForRightMouseButtonRelease = true;
	}

	CheckForBlockDigging( selectedBlock, blockGlobalCoordsForHitPosition );
}


//-----------------------------------------------------------------------------------------------
void World::CheckForBlockDigging( BlockInfo selectedBlock, const IntVector3& blockGlobalCoords )
{
	if ( g_theInputSystem->GetLeftMouseButtonDownStatus() )
	{
		if ( !m_blockBeingDug )
		{
			m_indexOfBlockBeingDug = selectedBlock.m_blockIndex;
			SetBlockBreakingVertexes( blockGlobalCoords );
			m_diggingStartTime = ( float ) GetCurrentTimeSeconds();
			m_diggingEndTime = m_diggingStartTime + ( g_blockDefinitions[ selectedBlock.GetBlock().m_blockType ].m_toughness * DEFAULT_DIG_TIME_SECONDS );
			m_blockBeingDug = true;
		}
	}
}