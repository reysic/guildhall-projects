#pragma once

#include <map>

#include "Engine/Math/AABB3.hpp"
#include "Game/Chunk.hpp"
#include "Game/GameCommon.hpp"
#include "Game/BlockInfo.hpp"


//-----------------------------------------------------------------------------------------------
struct RaycastResult3D
{
	RaycastResult3D()
		: m_didHit( false )
		, m_fractionComplete( 0.0f )
		, m_hitPosition( Vector3::ZERO )
		, m_faceNormal( Vector3::ZERO )
	{
		BlockInfo m_blockInfo = BlockInfo();
		BlockInfo m_blockInfoBeforeHit = BlockInfo();
	}
	
	BlockInfo m_blockInfo;
	BlockInfo m_blockInfoBeforeHit;
	Vector3 m_hitPosition;
	Vector3 m_faceNormal;
	bool m_didHit;
	float m_fractionComplete;
};


//-----------------------------------------------------------------------------------------------
class World
{
public:
	World();
	void Update( float deltaSeconds );
	void Render() const;
	void UpdateCameraAndPlayer();
	void SetPlayerBoundingBoxDrawPositions( const Vector3& playerBoundingBox );
	void SetBlockBreakingVertexes( const IntVector3& blockGlobalCoords );
	RaycastResult3D AWRaycast3D( const Vector3& start, const Vector3& end );
	bool ActivateNearestMissingChunk(); // returns true if a chunk was activated this frame
	void ConnectChunk( Chunk* chunk );
	void DeactivateFarthestObsoleteChunk();
	void DisconnectChunk( Chunk* chunk );
	void UpdateChunks();
	void SaveActiveChunksToDisk();
	void SavePlayerToDisk();
	IntVector2 GetChunkCoordsForWorldPosition( const Vector3& worldPosition );
	IntVector3 GetBlockLocalCoordsForWorldPosition( const Vector3& worldPosition );
	IntVector3 GetBlockGlobalCoordsForWorldPosition( const Vector3& worldPosition );
	bool AreBlockLocalCoordsWithinChunk( const IntVector3& blockLocalCoords);
	bool PlayerGrounded();
	BlockInfo GetBlockInfoForWorldPosition( const Vector3& worldPosition );
	void HandleNorthFaceSelection( BlockInfo selectedBlock, BlockInfo northNeighbor );
	void HandleSouthFaceSelection( BlockInfo selectedBlock, BlockInfo southNeighbor);
	void HandleEastFaceSelection( BlockInfo selectedBlock, BlockInfo eastNeighbor );
	void HandleWestFaceSelection( BlockInfo selectedBlock, BlockInfo westNeighbor );
	void HandleTopFaceSelection( BlockInfo selectedBlock, BlockInfo topNeighbor );
	void HandleBottomFaceSelection( BlockInfo selectedBlock, BlockInfo bottomNeighbor );
	void CheckForBlockDigging( BlockInfo selectedBlock, const IntVector3& blockGlobalCoords );

public:
	Chunk* m_chunk;
	std::map< IntVector2, Chunk* > m_activeChunks;
	int m_numRenderedChunks;
	int m_numSavedChunks;
	int m_numLoadedChunks;
	int m_indexOfBlockBeingDug;
	float m_diggingStartTime;
	float m_diggingEndTime;
	bool m_activeChunksSavedToDisk;
	bool m_playerSavedToDisk;
	bool m_blockBeingDug;
	Vector3 m_playerBoundingBoxVertexes[ 20 ];
	Vector3 m_blockBreakingVertexes[ 24 ];
	RaycastResult3D m_raycastResult;
};