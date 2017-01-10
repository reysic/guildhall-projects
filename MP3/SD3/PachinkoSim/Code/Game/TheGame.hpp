#pragma once
#include "Engine/MathUtils.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Game/World.hpp"
#include "Engine/Renderer/BitmapFontMeta.hpp"
#include "YoYo.hpp"
#include "Game/Node.hpp"
#include "Game/Cloth.hpp"
#include "Game/Ball.hpp"
#include "Game/ClothCollisionHelper.hpp"
#include "Game/ScoringNodes.hpp"


//-----------------------------------------------------------------------------------------------
class TheGame;
class Command;
class ScoringNodes;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;
extern float g_timeScale;

void DrawDebugArrow(Command&);

class TheGame
{
public:
	TheGame::TheGame();


	bool m_canPause;
	bool m_isPaused;
	World* m_world;
	int m_score;
	int m_currentLane;
	Texture* m_testTex;

	YoYo m_TestYoyo;
	Node m_testNode;
	Node m_testFixedNode;
	Node m_testFixedNode2;

	Cloth m_testCloth;
	std::vector<Ball*> m_balls;
	std::vector<ClothCollisionHelper*> m_clothCollisionHelper;
	ScoringNodes* m_scoringNodes;

	void SetUpPerspectiveProjection() const;
	void Update(const float deltaSeconds);
	void Render(const float deltaSeconds) const;
	void DrawHUD();
	void DebugText(std::string text);
	void DrawCrosshair();
	void HandleBallSpawningInput();
	void HandleLaneSelectionInput();
	void DrawSelectedLaneIndicator() const;

protected:
};