#pragma once

#include <string>

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
const float ACTIVE_RADIUS = 64.0f;
const float FLUSH_RADIUS = 80.0f;
const float CAMERA_FORWARD_RAYCAST_LENGTH = 8.0f;
const float PLAYER_EYE_LEVEL = 0.69f;
const float PLAYER_BOUNDING_BOX_HORIZONTAL_RADIUS = 0.3f;
const float PLAYER_BOUNDING_BOX_VERTICAL_RADIUS = 0.925f;
const float DEFAULT_DIG_TIME_SECONDS = 1.0f;
const float PLAYER_MOVE_SPEED = 75.0f;
const float WALKING_FRICTION = 0.15f;
const float WALKING_SPEED_CONSERVATION = 1.0f - WALKING_FRICTION;
const float JUMP_IMPULSE = 8.5f;
const float GRAVITY = 25.0f;
const Vector3 PLAYER_INITIAL_POSITION = Vector3( -5.33f, -5.14f, 92.04f );
const EulerAngles PLAYER_INITIAL_ORIENTATION = EulerAngles( 0.0f, 53.0f, 45.0f );
const int CHUNK_BITS_X = 4; // 2^4 = 16
const int CHUNK_BITS_Y = 4;
const int CHUNK_BITS_Z = 7; // 2^7 = 128
const int CHUNK_BITS_XY = CHUNK_BITS_X + CHUNK_BITS_Y;
const int BLOCKS_WIDE_X = BIT( CHUNK_BITS_X );
const int BLOCKS_WIDE_Y = BIT( CHUNK_BITS_Y );
const int BLOCKS_TALL_Z = BIT( CHUNK_BITS_Z );
const int LOCAL_X_MASK = BLOCKS_WIDE_X - 1;
const int LOCAL_Y_MASK = ( BLOCKS_WIDE_Y - 1 ) << CHUNK_BITS_X;
const int LOCAL_Z_MASK = ( BLOCKS_TALL_Z - 1 ) << ( CHUNK_BITS_X + CHUNK_BITS_Y );
const int LIGHTING_BITS = 4;
const int NUM_BLOCKS_PER_CHUNK_LAYER = BLOCKS_WIDE_X * BLOCKS_WIDE_Y;
const int NUM_BLOCKS_PER_CHUNK = BLOCKS_WIDE_X * BLOCKS_WIDE_Y * BLOCKS_TALL_Z;
const int SEA_LEVEL_Z = 64;
const unsigned int CHUNK_NOISE_NUM_OCTAVES = 4;
const float CHUNK_NOISE_PERSISTENCE = 0.4f;
const unsigned int TREE_NOISE_NUM_OCTAVES = 2;
const float TREE_NOISE_PERSISTENCE = 0.4f;
const unsigned char BITMASK_BLOCK_LIGHT = 0x0F; // 00001111 (bits 0-3) store light value, 0 to 15
const unsigned char BITMASK_BLOCK_IS_SKY = 0x80; // 10000000 (bit 7) is on if block sees sky
const unsigned char BITMASK_BLOCK_LIGHT_DIRTY = 0x40; // 01000000 (bit 6) is on if light is dirty
const unsigned char BITMASK_BLOCK_IS_OPAQUE = 0x20; // 00100000 (bit 5) on if block type is opaque
const std::string BLOCK_TYPE_CHANGE_SOUND_FILE = "Data/Audio/popSound.wav";
const std::string BLOCK_BROKEN_SOUND_FILE = "Data/Audio/digGrass.ogg";
const std::string BLOCK_PLACE_SOUND_FILE = "Data/Audio/digStone.ogg";


//-----------------------------------------------------------------------------------------------
extern unsigned char g_hudSelectedBlockType;
extern bool g_saveLoadChunks;