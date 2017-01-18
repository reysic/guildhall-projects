#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Meshes/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Cameras/Camera.hpp"
#include "Engine/Tools/Jobs/JobSystem.hpp"
#include "Engine/Networking/Session.hpp"
#include "Game/Environment/Map.hpp"
#include "Game/Entities/NetObject.hpp"
#include "Game/Entities/NetPlayer.hpp"
#include "Game/Entities/NetBullet.hpp"


//-----------------------------------------------------------------------------------------------
class TheGame;
class Sprite;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
enum GameState
{
	STATE_MAIN_MENU,
	STATE_PLAYING,
	STATE_GAME_OVER,
	NUM_GAME_STATES
};


//-----------------------------------------------------------------------------------------------
const float NET_PLAYER_FIRING_RATE_BULLETS_PER_SECOND = 10.0f;
const int MAX_BULLET_COUNT = 1000;
const int MAX_NET_OBJECTS = 5000;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	// Constructors & Destructors
	TheGame();
	~TheGame();

	// Shader Test
	void ShaderTestInitialize();
	void ShaderTestRender() const;

	// Methods
	void Update( float deltaSeconds );
	void UpdateFPSCounter( float deltaSeconds );
	void UpdateWorld( float deltaSeconds );
	void HandleMouseAndKeyboardInput();
	void HandleTwinStickInput();
	void HandleStateSpecificInput();
	void Render() const;
	void DrawHUD() const;
	void DrawNetDebugInfo() const;

	// Accessors
	GameState GetState() const { return m_state; }

	// Mutators
	bool SetState( GameState newState );

	void SpawnPlayerBullet();
	void SpawnPlayerBulletIDoNotOwn( uint8_t ownerConnectionIndex, uint16_t netID, const Vector2& position, 
		float xboxRightStickPositionPolarThetaRadians );
	void DestroyDeadBullets();
	void CheckBulletsForCollisions();

	uint16_t GetNextNetID();
	NetObject* FindNetObjectByID( uint8_t connectionID, uint16_t netID );
	void DeleteNetObjectByID( uint8_t connectionID, uint16_t netID );
	void IncrementRedScore() { ++m_redScore; }
	void IncrementGreenScore() { ++m_greenScore; }
	void IncrementBlueScore() { ++m_blueScore; }

public:
	// Members
	int m_score;
	bool m_disableShooting;
	NetObject* m_netObjects[ MAX_NET_OBJECTS ];
	Connection* m_myConnection;
	Session* m_mySession;
	bool m_waitToSpawnNewPlayerBullet;
	float m_timeBetweenPlayerBullets;
	float m_timeLastPlayerBulletFired;

	uint16_t m_nextNetID;
	uint16_t m_redScore;
	uint16_t m_greenScore;
	uint16_t m_blueScore;

private:
	// Members
	float m_age;
	Sprite* m_testSprite;
	GameState m_state;

	// ShaderTest
	ShaderProgram* m_shaderProgram;
	GLuint m_vaoHandle;
};


//-----------------------------------------------------------------------------------------------
void TestJobFunction( Job* job );