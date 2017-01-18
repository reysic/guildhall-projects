#pragma once

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Meshes/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Cameras/Camera.hpp"
#include "Engine/Tools/Jobs/JobSystem.hpp"
#include "Game/Environment/Map.hpp"


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
	void HandleStateSpecificInput();
	void Render() const;
	void DrawHUD() const;

	// Accessors
	GameState GetState() const { return m_state; }

	// Mutators
	bool SetState( GameState newState );

public:
	// Members
	int m_score;
	bool m_disableShooting;

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