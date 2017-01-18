#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Noise.hpp"
#include "Game/Chunk.hpp"
#include "Game/BlockDefinition.hpp"
#include "Game/BlockInfo.hpp"
#include "Game/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
Chunk::Chunk( IntVector2 chunkCoords )
	: m_chunkCoords( chunkCoords )
	, m_vboIsOutdated( true )
	, m_northNeighbor( nullptr )
	, m_southNeighbor( nullptr )
	, m_eastNeighbor( nullptr )
	, m_westNeighbor( nullptr )
	, m_isRendered( true )
	, m_vboID( 0 )
	, m_numVerts( 0 )
{
	std::memset( m_blocks, 0, sizeof m_blocks );
	m_worldMins = Vector3( m_chunkCoords.x * ( float ) BLOCKS_WIDE_X, m_chunkCoords.y * ( float ) BLOCKS_WIDE_Y, 0.0f );
	m_chunkCenterPos = Vector2( m_worldMins.x + ( BLOCKS_WIDE_X / 2 ), m_worldMins.y + ( BLOCKS_WIDE_Y / 2 ) );
	GenerateBlocks();
}


//-----------------------------------------------------------------------------------------------
Chunk::Chunk( IntVector2 chunkCoords, Block loadedBlocks[ NUM_BLOCKS_PER_CHUNK ] )
	: m_chunkCoords( chunkCoords )
	, m_vboIsOutdated( true )
	, m_northNeighbor( nullptr )
	, m_southNeighbor( nullptr )
	, m_eastNeighbor( nullptr )
	, m_westNeighbor( nullptr )
	, m_isRendered( true )
	, m_vboID( 0 )
	, m_numVerts( 0 )
{
	std::memset( m_blocks, 0, sizeof m_blocks );

	for ( int currentArrayIndex = 0; currentArrayIndex < NUM_BLOCKS_PER_CHUNK; ++currentArrayIndex )
	{
		m_blocks[ currentArrayIndex ] = loadedBlocks[ currentArrayIndex ];
	}

	std::memset( loadedBlocks, 0, sizeof loadedBlocks );

	m_worldMins = Vector3( m_chunkCoords.x * ( float ) BLOCKS_WIDE_X, m_chunkCoords.y * ( float ) BLOCKS_WIDE_Y, 0.0f );
	m_chunkCenterPos = Vector2( m_worldMins.x + ( BLOCKS_WIDE_X / 2 ), m_worldMins.y + ( BLOCKS_WIDE_Y / 2 ) );
}


//-----------------------------------------------------------------------------------------------
Chunk::~Chunk()
{
	// Delete chunk's VBO
	if ( m_vboID != 0 )
	{
		glDeleteBuffers( 1, &m_vboID ); // #TODO: Make wrapper in Renderer.cpp
	}
}


//-----------------------------------------------------------------------------------------------
void Chunk::ComputePerlinNoiseForChunkColumns()
{
	float minChunkColumnHeight = 20.0f;
	float maxChunkColumnHeight = 120.0f;

	// Generate chunk column heights using Perlin noise
	int arrayIndex = 0;

	for ( float currentY = m_worldMins.y; currentY < ( m_worldMins.y + BLOCKS_WIDE_Y ); ++currentY )
	{
		for ( float currentX = m_worldMins.x; currentX < ( m_worldMins.x + BLOCKS_WIDE_X ); ++currentX )
		{
			m_chunkColumnHeights[ arrayIndex ] = RangeMap( Compute2dPerlinNoise( currentX, currentY, 200.0f, CHUNK_NOISE_NUM_OCTAVES, CHUNK_NOISE_PERSISTENCE ), -1.0f, 1.0f, minChunkColumnHeight, maxChunkColumnHeight );
		
			arrayIndex++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Chunk::ComputePerlinNoiseForTrees()
{
	// Generation of Perlin noise for trees
	int arrayIndex = 0;

	for ( float currentY = m_worldMins.y - 2; currentY < ( m_worldMins.y + BLOCKS_WIDE_Y + 2 ); ++currentY ) // #TODO: Use named constant for tree radius
	{
		for ( float currentX = m_worldMins.x - 2; currentX < ( m_worldMins.x + BLOCKS_WIDE_X + 2 ); ++currentX )
		{
			m_perlinNoiseForTrees[ arrayIndex ] = Compute2dPerlinNoise( currentX, currentY, 20.0f, TREE_NOISE_NUM_OCTAVES, TREE_NOISE_PERSISTENCE );

			arrayIndex++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Chunk::Update()
{
	if ( m_vboIsOutdated )
	{
		GenerateVertexArrayAndVBO();
	}

	CheckFrustumCulling();
}


//-----------------------------------------------------------------------------------------------
// VBO creation and updating (done ONLY when the MapChunk is marked as Dirty!)
// Code by Squirrel Eiserloh
void Chunk::GenerateVertexArrayAndVBO()
{
	std::vector< Vertex_PCT > vertexArray;
	PopulateVertexArray( vertexArray );

	if ( m_vboID == 0 )
	{
		glGenBuffers( 1, &m_vboID ); // #TODO: Make wrapper in Renderer.cpp
	}

	size_t vertexArrayNumBytes = sizeof( Vertex_PCT ) * vertexArray.size();
	g_theRenderer->BindBuffer( m_vboID );
	g_theRenderer->BufferData( vertexArrayNumBytes, vertexArray.data() );
	g_theRenderer->BindBuffer( 0 );
	m_vboIsOutdated = false;
}


//-----------------------------------------------------------------------------------------------
// Frustum cull, don't render chunks behind the camera
void Chunk::CheckFrustumCulling()
{
	bool renderChunk = false;
	Vector3 cameraPosition = g_theGame->m_camera.m_position;
	Vector3 cameraForwardXYZ = g_theGame->m_camera.GetForwardXYZ();

	Vector3 displacementVector = m_worldMins - cameraPosition;
	float dotProduct = DotProductVector3( displacementVector, cameraForwardXYZ );
	if ( dotProduct > 0.0f )
		renderChunk = true;

	displacementVector = ( m_worldMins + Vector3( ( float ) BLOCKS_WIDE_X, 0.0f, 0.0f ) ) - cameraPosition;
	dotProduct = DotProductVector3( displacementVector, cameraForwardXYZ );
	if ( dotProduct > 0.0f )
		renderChunk = true;

	displacementVector = ( m_worldMins + Vector3( ( float ) BLOCKS_WIDE_X, ( float ) BLOCKS_WIDE_Y, 0.0f ) ) - cameraPosition;
	dotProduct = DotProductVector3( displacementVector, cameraForwardXYZ );
	if ( dotProduct > 0.0f )
		renderChunk = true;

	displacementVector = ( m_worldMins + Vector3( 0.0f, ( float ) BLOCKS_WIDE_Y, 0.0f ) ) - cameraPosition;
	dotProduct = DotProductVector3( displacementVector, cameraForwardXYZ );
	if ( dotProduct > 0.0f )
		renderChunk = true;

	displacementVector = ( m_worldMins + Vector3( 0.0f, 0.0f, ( float ) BLOCKS_TALL_Z ) ) - cameraPosition;
	dotProduct = DotProductVector3( displacementVector, cameraForwardXYZ );
	if ( dotProduct > 0.0f )
		renderChunk = true;

	displacementVector = ( m_worldMins + Vector3( ( float ) BLOCKS_WIDE_X, 0.0f, ( float ) BLOCKS_TALL_Z ) ) - cameraPosition;
	dotProduct = DotProductVector3( displacementVector, cameraForwardXYZ );
	if ( dotProduct > 0.0f )
		renderChunk = true;

	displacementVector = ( m_worldMins + Vector3( ( float ) BLOCKS_WIDE_X, ( float ) BLOCKS_WIDE_Y, ( float ) BLOCKS_TALL_Z ) ) - cameraPosition;
	dotProduct = DotProductVector3( displacementVector, cameraForwardXYZ );
	if ( dotProduct > 0.0f )
		renderChunk = true;

	displacementVector = ( m_worldMins + Vector3( 0.0f, ( float ) BLOCKS_WIDE_Y, ( float ) BLOCKS_TALL_Z ) ) - cameraPosition;
	dotProduct = DotProductVector3( displacementVector, cameraForwardXYZ );
	if ( dotProduct > 0.0f )
		renderChunk = true;

	m_isRendered = renderChunk;
}


//-----------------------------------------------------------------------------------------------
void Chunk::GenerateBlocks()
{
	ComputePerlinNoiseForChunkColumns();

	int arrayIndex = 0;

	for ( int blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; blockIndex++ )
	{
		IntVector3 blockLocalCoords = GetLocalCoordsForBlockIndex( blockIndex );
		Vector2 blockGlobalCoords2D( blockLocalCoords.x + ( m_chunkCoords.x * ( float ) BLOCKS_WIDE_X ), blockLocalCoords.y + ( m_chunkCoords.y * ( float ) BLOCKS_WIDE_Y ) );
		int chunkColumnHeight = ( int ) m_chunkColumnHeights[ arrayIndex ];

		if ( blockLocalCoords.z < chunkColumnHeight - 5 )
		{
			m_blocks[ blockIndex ].SetBlockType( BLOCK_TYPE_STONE );
		}
		else if ( blockLocalCoords.z < chunkColumnHeight - 1 )
		{
			m_blocks[ blockIndex ].SetBlockType( BLOCK_TYPE_DIRT );
		}
		else if ( blockLocalCoords.z < chunkColumnHeight )
		{
			m_blocks[ blockIndex ].SetBlockType( BLOCK_TYPE_GRASS );
		}
		else
		{
			m_blocks[ blockIndex ].SetBlockType( BLOCK_TYPE_AIR );
		}

		// Now make some blocks sand and some blocks water
		if ( blockLocalCoords.z < ( SEA_LEVEL_Z ) && m_blocks[ blockIndex ].m_blockType == BLOCK_TYPE_AIR )
		{
			m_blocks[ blockIndex ].SetBlockType( BLOCK_TYPE_WATER );
		}
		else if ( blockLocalCoords.z == ( SEA_LEVEL_Z - 1 ) && ( m_blocks[ blockIndex ].m_blockType == BLOCK_TYPE_DIRT || m_blocks[ blockIndex ].m_blockType == BLOCK_TYPE_GRASS ) )
		{
			m_blocks[ blockIndex ].SetBlockType( BLOCK_TYPE_SAND );
		}

		if ( arrayIndex == 255 )
		{
			arrayIndex = 0;
		}
		else
		{
			arrayIndex++;
		}
	}

	ComputePerlinNoiseForTrees();
	LocateTrees();
}


//-----------------------------------------------------------------------------------------------
// Code to build vertex array data for a chunk, based on the blocks in the chunk
// Based on code by Squirrel Eiserloh
void Chunk::PopulateVertexArray( std::vector< Vertex_PCT >& out_vertexArray  )
{
	out_vertexArray.clear(); // Clear the contents of the vertex array before population
	out_vertexArray.reserve( 10000 ); // We want a conservative, but realistic, semi-high estimate

	for ( int blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK; blockIndex++ )
	{
		AddBlockVertexes( blockIndex, out_vertexArray );
	}

	m_numVerts = out_vertexArray.size();
}


//-----------------------------------------------------------------------------------------------
void Chunk::AddBlockVertexes( int blockIndex, std::vector< Vertex_PCT >& out_vertexArray )
{
	Block& block = m_blocks[ blockIndex ];
	if ( block.m_blockType == BLOCK_TYPE_AIR )
		return;

	BlockDefinition& blockDef = g_blockDefinitions[ block.m_blockType ];
	const AABB2& topTexCoords = blockDef.m_topTexCoords;
	const AABB2& sideTexCoords = blockDef.m_sideTexCoords;
	const AABB2& bottomTexCoords = blockDef.m_bottomTexCoords;
	IntVector3 blockLocalCoords = GetLocalCoordsForBlockIndex( blockIndex );
	Vector3 mins( ( float ) blockLocalCoords.x, ( float ) blockLocalCoords.y, ( float ) blockLocalCoords.z );
	Vector3 maxs( mins.x + 1.0f, mins.y + 1.0f, mins.z + 1.0f );

	BlockInfo currentBlock( this, blockIndex );
	BlockInfo northNeighbor = currentBlock.GetNorthNeighborInfo();
	BlockInfo southNeighbor = currentBlock.GetSouthNeighborInfo();
	BlockInfo eastNeighbor = currentBlock.GetEastNeighborInfo();
	BlockInfo westNeighbor = currentBlock.GetWestNeighborInfo();
	BlockInfo topNeighbor = currentBlock.GetTopNeighborInfo();
	BlockInfo bottomNeighbor = currentBlock.GetBottomNeighborInfo();

	if ( northNeighbor.m_chunk != nullptr )
	{
		if ( currentBlock.GetBlockType() == BLOCK_TYPE_WATER ) // Special case for water, only render sides if touching air
		{
			if ( northNeighbor.GetBlockType() == BLOCK_TYPE_AIR )
			{
				// Add the vertexes of the north face to the vertex array

				out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.maxs.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.mins.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.mins.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.maxs.y ) ) );
			}
		}
		else if ( !g_blockDefinitions[ northNeighbor.GetBlock().m_blockType ].m_isOpaque ) // HSR
		{
			// Add the vertexes of the north face to the vertex array

			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.mins.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.mins.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.maxs.y ) ) );
		}
	}

	if ( southNeighbor.m_chunk != nullptr )
	{
		if ( currentBlock.GetBlockType() == BLOCK_TYPE_WATER ) // Special case for water, only render sides if touching air
		{
			if ( southNeighbor.GetBlockType() == BLOCK_TYPE_AIR )
			{
				// Add the vertexes of the north face to the vertex array

				out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.maxs.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.mins.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.mins.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.maxs.y ) ) );
			}
		}
		else if ( !g_blockDefinitions[ southNeighbor.GetBlock().m_blockType ].m_isOpaque ) // HSR
		{
			// Add the vertexes of the south face to the vertex array

			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, mins.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, mins.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, mins.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.mins.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, mins.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.mins.y ) ) );
		}
	}

	if ( eastNeighbor.m_chunk != nullptr )
	{
		if ( currentBlock.GetBlockType() == BLOCK_TYPE_WATER ) // Special case for water, only render sides if touching air
		{
			if ( eastNeighbor.GetBlockType() == BLOCK_TYPE_AIR )
			{
				// Add the vertexes of the north face to the vertex array

				out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.maxs.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.mins.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.mins.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.maxs.y ) ) );
			}
		}
		else if ( !g_blockDefinitions[ eastNeighbor.GetBlock().m_blockType ].m_isOpaque ) // HSR
		{
			// Add the vertexes of the east face to the vertex array

			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, mins.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.mins.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, mins.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.mins.y ) ) );
		}
	}

	if ( westNeighbor.m_chunk != nullptr )
	{
		if ( currentBlock.GetBlockType() == BLOCK_TYPE_WATER ) // Special case for water, only render sides if touching air
		{
			if ( westNeighbor.GetBlockType() == BLOCK_TYPE_AIR )
			{
				// Add the vertexes of the north face to the vertex array

				out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.maxs.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.mins.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.mins.y ) ) );
				out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.maxs.y ) ) );
			}
		}
		else if ( !g_blockDefinitions[ westNeighbor.GetBlock().m_blockType ].m_isOpaque ) // HSR
		{
			// Add the vertexes of the west face to the vertex array

			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, mins.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, mins.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.maxs.x, sideTexCoords.mins.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.mins.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( sideTexCoords.mins.x, sideTexCoords.maxs.y ) ) );
		}
	}

	if ( blockLocalCoords.z < 127 && blockLocalCoords.z >= 0 )
	{
		if ( !g_blockDefinitions[ topNeighbor.GetBlock().m_blockType ].m_isOpaque ) // HSR
		{
			// Add the vertexes of the top face to the vertex array

			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, mins.y, maxs.z ), Rgba::WHITE, Vector2( topTexCoords.mins.x, topTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, mins.y, maxs.z ), Rgba::WHITE, Vector2( topTexCoords.maxs.x, topTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( topTexCoords.maxs.x, topTexCoords.mins.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, maxs.z ), Rgba::WHITE, Vector2( topTexCoords.mins.x, topTexCoords.mins.y ) ) );
		}
	}

	if ( blockLocalCoords.z > 0 && blockLocalCoords.z <= 127 )
	{
		if ( !g_blockDefinitions[ bottomNeighbor.GetBlock().m_blockType ].m_isOpaque ) // HSR
		{
			// Add the vertexes of the bottom face to the vertex array

			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, mins.y, mins.z ), Rgba::WHITE, Vector2( bottomTexCoords.mins.x, bottomTexCoords.mins.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( mins.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( bottomTexCoords.mins.x, bottomTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, maxs.y, mins.z ), Rgba::WHITE, Vector2( bottomTexCoords.maxs.x, bottomTexCoords.maxs.y ) ) );
			out_vertexArray.push_back( Vertex_PCT( Vector3( maxs.x, mins.y, mins.z ), Rgba::WHITE, Vector2( bottomTexCoords.maxs.x, bottomTexCoords.mins.y ) ) );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Chunk::Render() const
{
	static SpriteSheet spriteSheet( "Data/Images/SimpleMinerAtlas.png", 16, 16 );

	g_theRenderer->EnableDepthTesting();
	g_theRenderer->EnableBackFaceCulling();

	g_theRenderer->PushView();
	g_theRenderer->TranslateView( m_worldMins );

	g_theRenderer->BindTexture( spriteSheet.GetTexture() );

	g_theRenderer->DrawVBO_PCT( m_vboID, m_numVerts, PRIMITIVE_QUADS );

	g_theRenderer->PopView();
}


//-----------------------------------------------------------------------------------------------
void Chunk::LocateTrees()
{
	int arrayIndex = ( BLOCKS_WIDE_X + 4 ) + ( BLOCKS_WIDE_X + 4 ) + 2;
	int columnArrayIndex = 0;

	for ( int blockIndex = 0; blockIndex < NUM_BLOCKS_PER_CHUNK_LAYER; blockIndex++ )
	{
		IntVector3 blockLocalCoords = GetLocalCoordsForBlockIndex( blockIndex );
		Vector2 blockGlobalCoords2D( blockLocalCoords.x + ( m_chunkCoords.x * ( float ) BLOCKS_WIDE_X ), blockLocalCoords.y + ( m_chunkCoords.y * ( float ) BLOCKS_WIDE_Y ) );
		Vector2 northNeighborGlobalCoords2D( blockGlobalCoords2D.x, blockGlobalCoords2D.y + 1.0f );
		Vector2 southNeighborGlobalCoords2D( blockGlobalCoords2D.x, blockGlobalCoords2D.y - 1.0f );
		Vector2 eastNeighborGlobalCoords2D( blockGlobalCoords2D.x + 1.0f, blockGlobalCoords2D.y );
		Vector2 westNeighborGlobalCoords2D( blockGlobalCoords2D.x - 1.0f, blockGlobalCoords2D.y );
		int chunkColumnHeight = ( int ) m_chunkColumnHeights[ columnArrayIndex ];

		if ( IsTreeAtArrayIndex( arrayIndex ) ) // Check if current column contains a tree
		{
			// Current column contains a tree

			MakeTree( chunkColumnHeight, blockGlobalCoords2D );
		}
		else if ( IsTreeAtArrayIndex( arrayIndex + ( BLOCKS_WIDE_X + 4 ) ) ) // Check if north neighbor's column contains a tree
		{
			// North neighbor's column contains a tree

			int northNeighborColumnHeight = ( int ) RangeMap( Compute2dPerlinNoise( northNeighborGlobalCoords2D.x, northNeighborGlobalCoords2D.y, 200.0f, CHUNK_NOISE_NUM_OCTAVES, CHUNK_NOISE_PERSISTENCE ), -1.0f, 1.0f, 20.0f, 120.0f );

			MakeTree( northNeighborColumnHeight, northNeighborGlobalCoords2D );
		}
		else if ( IsTreeAtArrayIndex( arrayIndex - ( BLOCKS_WIDE_X + 4 ) ) ) // Check if south neighbor's column contains a tree
		{
			// South neighbor's column contains a tree

			int southNeighborColumnHeight = ( int ) RangeMap( Compute2dPerlinNoise( southNeighborGlobalCoords2D.x, southNeighborGlobalCoords2D.y, 200.0f, CHUNK_NOISE_NUM_OCTAVES, CHUNK_NOISE_PERSISTENCE ), -1.0f, 1.0f, 20.0f, 120.0f );

			MakeTree( southNeighborColumnHeight, southNeighborGlobalCoords2D );
		}
		else if ( IsTreeAtArrayIndex( arrayIndex + 1 ) ) // Check if east neighbor's column contains a tree
		{
			// East neighbor's column contains a tree

			int eastNeighborColumnHeight = ( int ) RangeMap( Compute2dPerlinNoise( eastNeighborGlobalCoords2D.x, eastNeighborGlobalCoords2D.y, 200.0f, CHUNK_NOISE_NUM_OCTAVES, CHUNK_NOISE_PERSISTENCE ), -1.0f, 1.0f, 20.0f, 120.0f );

			MakeTree( eastNeighborColumnHeight, eastNeighborGlobalCoords2D );
		}
		else if ( IsTreeAtArrayIndex( arrayIndex - 1 ) ) // Check if west neighbor's column contains a tree
		{
			// West neighbor's column contains a tree

			int westNeighborColumnHeight = ( int ) RangeMap( Compute2dPerlinNoise( westNeighborGlobalCoords2D.x, westNeighborGlobalCoords2D.y, 200.0f, CHUNK_NOISE_NUM_OCTAVES, CHUNK_NOISE_PERSISTENCE ), -1.0f, 1.0f, 20.0f, 120.0f );

			MakeTree( westNeighborColumnHeight, westNeighborGlobalCoords2D );
		}

		if ( columnArrayIndex == 15 || columnArrayIndex == 31 || columnArrayIndex == 47 ||
			 columnArrayIndex == 63 || columnArrayIndex == 79 || columnArrayIndex == 95 ||
			 columnArrayIndex == 111 || columnArrayIndex == 127 || columnArrayIndex == 143 ||
			 columnArrayIndex == 159 || columnArrayIndex == 175 || columnArrayIndex == 191 ||
			 columnArrayIndex == 207 || columnArrayIndex == 223 || columnArrayIndex == 239 )
		{
			arrayIndex += 5;
			columnArrayIndex++;
		}
		else
		{
			arrayIndex++;
			columnArrayIndex++;
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Chunk::IsTreeAtArrayIndex( int arrayIndex )
{
	float treePerlinNoise = m_perlinNoiseForTrees[ arrayIndex ];
	float northNeighborTreePerlinNoise = m_perlinNoiseForTrees[ arrayIndex + ( BLOCKS_WIDE_X + 4 ) ];
	float southNeighborTreePerlinNoise = m_perlinNoiseForTrees[ arrayIndex - ( BLOCKS_WIDE_X + 4 ) ];
	float eastNeighborTreePerlinNoise = m_perlinNoiseForTrees[ arrayIndex + 1 ];
	float westNeighborTreePerlinNoise = m_perlinNoiseForTrees[ arrayIndex - 1 ];
	float northeastNeighborTreePerlinNoise = m_perlinNoiseForTrees[ arrayIndex + ( BLOCKS_WIDE_X + 4 ) + 1 ];
	float northwestNeighborTreePerlinNoise = m_perlinNoiseForTrees[ arrayIndex + ( BLOCKS_WIDE_X + 4 ) - 1 ];
	float southeastNeighborTreePerlinNoise = m_perlinNoiseForTrees[ arrayIndex - ( BLOCKS_WIDE_X + 4 ) - 1 ];
	float southwestNeighborTreePerlinNoise = m_perlinNoiseForTrees[ arrayIndex - ( BLOCKS_WIDE_X + 4 ) + 1 ];

	if ( treePerlinNoise > northNeighborTreePerlinNoise &&
		treePerlinNoise > southNeighborTreePerlinNoise &&
		treePerlinNoise > eastNeighborTreePerlinNoise &&
		treePerlinNoise > westNeighborTreePerlinNoise &&
		treePerlinNoise > northeastNeighborTreePerlinNoise &&
		treePerlinNoise > northwestNeighborTreePerlinNoise &&
		treePerlinNoise > southeastNeighborTreePerlinNoise &&
		treePerlinNoise > southwestNeighborTreePerlinNoise )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
void Chunk::MakeTree( int columnHeight, const Vector2& globalCoords2D )
{
	Vector3 treePosition = Vector3( globalCoords2D.x, globalCoords2D.y, ( float ) columnHeight - 1 );
	Vector3 worldMaxs = Vector3( ( m_chunkCoords.x * ( float ) BLOCKS_WIDE_X ) + ( float ) BLOCKS_WIDE_X, ( m_chunkCoords.y * ( float ) BLOCKS_WIDE_Y ) + BLOCKS_WIDE_Y, 0.0f );

	if ( treePosition.z < SEA_LEVEL_Z )
	{
		return;
	}

	if ( columnHeight % 2 == 0 )
	{
		for ( int i = 0; i < OAK_TREE_NUM_BLOCKS; i++ )
		{
			Vector3 treeBlockPosition = treePosition + ( OAK_TREE_BLOCK_OFFSETS[ i ].m_offsetFromTreeBase );

			bool treeBlockWithinChunk = ( treeBlockPosition.x >= m_worldMins.x ) && ( treeBlockPosition.x < worldMaxs.x ) && ( treeBlockPosition.y >= m_worldMins.y ) && ( treeBlockPosition.y < worldMaxs.y );
			if ( treeBlockWithinChunk )
			{
				IntVector3 treeBlockLocalCoords = g_theGame->m_world->GetBlockLocalCoordsForWorldPosition( treeBlockPosition );
				int treeBlockIndex = GetBlockIndexForLocalCoords( treeBlockLocalCoords );

				this->m_blocks[ treeBlockIndex ].SetBlockType( OAK_TREE_BLOCK_OFFSETS[ i ].m_type );
			}
		}
	}
	else
	{
		for ( int i = 0; i < BIRCH_TREE_NUM_BLOCKS; i++ )
		{
			Vector3 treeBlockPosition = treePosition + ( BIRCH_TREE_BLOCK_OFFSETS[ i ].m_offsetFromTreeBase );

			bool treeBlockWithinChunk = ( treeBlockPosition.x >= m_worldMins.x ) && ( treeBlockPosition.x < worldMaxs.x ) && ( treeBlockPosition.y >= m_worldMins.y ) && ( treeBlockPosition.y < worldMaxs.y );
			if ( treeBlockWithinChunk )
			{
				IntVector3 treeBlockLocalCoords = g_theGame->m_world->GetBlockLocalCoordsForWorldPosition( treeBlockPosition );
				int treeBlockIndex = GetBlockIndexForLocalCoords( treeBlockLocalCoords );

				this->m_blocks[ treeBlockIndex ].SetBlockType( BIRCH_TREE_BLOCK_OFFSETS[ i ].m_type );
			}
		}
	}
}
