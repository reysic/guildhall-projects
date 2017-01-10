#include "Game/TheGame.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
//#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB3.hpp"

#include <time.h>
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Console.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vector3Double.hpp"
#include "Game/ScoringNodes.hpp"
#include "Game/GameCommon.hpp"



TheGame* g_theGame = nullptr;


const float XHAIR_LENGTH = 15.f;
const std::string FONT_NAME = "Data/Fonts/ArialFont.fnt";
//const IntVector2 snapBackPos = IntVector2(800, 450);

const std::string TEST_FILE = "Data/Images/wood_7.png";

TheGame::TheGame()
	: m_score( 0 )
	, m_currentLane( 0 )
{
	//InitializeCommon();

	m_world = new World();
	m_testTex = Texture::CreateOrGetTexture(TEST_FILE);
//	g_theRenderer->AddDebugPoint(Vector3(2.f, 0.f, 1.f), Rgba::RED, 10.f, RenderCommand::DEPTH_TEST_ON);
// 	g_theRenderer->AddDebugLine(Vector3(1.f, 1.f, 1.f), Vector3(10.f, 2.f, 5.f), Rgba::RED, -1.f, RenderCommand::DEPTH_TEST_ON);
//	g_theRenderer->AddDebugArrow(Vector3(1.f, 1.f, 1.f), Vector3(10.f, 10.f, 10.f), Rgba::BLUE, 7.f, RenderCommand::DEPTH_TEST_OFF);

//	g_theRenderer->AddDebugAABB3(Vector3(1.f, 1.f, 1.f), Vector3(4.f, 4.f, 4.f), Rgba::BLACK, -1.f, RenderCommand::DEPTH_TEST_ON);
//	g_theRenderer->AddDebugSphere(Vector3(3.f, 3.f, 3.f), 1.f, Rgba::PINK, -1.f, RenderCommand::XRAY);

	Console::RegisterCommand("draw", DrawDebugArrow );

	m_TestYoyo = YoYo( Vector3Double(0.f, 0.f, -1.f), Vector3Double::ZERO,  5.f,  9.5f,  .9f, 3.f  );

	m_testNode = Node( Vector3Double( 1.f, 1.f, 1.f ), 1.0 );
	m_testNode.m_velocity.z = 4.8f;
	m_testNode.m_prevVelocity.z = 4.8f;

	m_testFixedNode= Node( Vector3Double( 2.f, 1.f, 1.f ), 1.0 );
	m_testFixedNode.m_isFixed = true;


	m_testFixedNode2 = Node( Vector3Double(0.f, 1.f, 1.f), 1.0);
	m_testFixedNode2.m_isFixed = true;

	m_testFixedNode.AddSpring(&m_testNode, Spring::SPRING_STRUCTURAL, 1.f, 9.5f, .9f);
	m_testNode.AddSpring(&m_testFixedNode2, Spring::SPRING_STRUCTURAL, 1.f, 9.5f, 0.9f);

	m_testCloth = Cloth( Vector3Double( 0.0, 10.0, 2.0 ) );


// 	m_balls.push_back(Ball(100.f, 1.3f, BallState(Vector3(-3.f, 13.f, 0.f), Vector3(3.f, -3.f, 0.f))));
// 	m_balls.push_back(Ball(100.f, 1.3f, BallState(Vector3(7.f, 2.f, 0.f), Vector3(-3.f, 3.f, 0.f))));
// 	for (float row = 0; row < 10.f; ++row)
// 	{
// 		for (float col = row; col < 10.f; ++col)
// 		{
// 			m_balls.push_back(Ball(1.f, 0.2f, BallState(Vector3(row, col, 0.f), Vector3(0.f, 0.f, 0.f))));
// 		}
// 	}

	m_balls.push_back( new Ball(100.f, .3f, BallState(Vector3(4.1f, 10.f, 2.f), Vector3(0.f, 0.f, -0.0f))));

	ClothCollisionHelper* firstHelper = new ClothCollisionHelper();
	firstHelper->m_cloth = &m_testCloth;
	firstHelper->m_ball = m_balls[0];
	m_clothCollisionHelper.push_back( firstHelper );

	// Create scoring nodes
	m_scoringNodes = new ScoringNodes( m_testCloth );
}

void TheGame::SetUpPerspectiveProjection() const
{
	float aspect = (16.f / 9.f);
	//float fovDegreesHorizontal = 90.f;
	float fovDegreesVertical = 70.f;
	float zNear = 0.1f;
	float zFar = 100.f;

	g_theRenderer->SetPerspective(fovDegreesVertical, aspect, zNear, zFar);
}

void TheGame::Update(const float deltaSeconds)
{
	//   ----------------------------------------------------------------------
	//   -README READ ME  CONTROLS-
	//   ----------------------------------------------------------------------
	float stiffIncrement = .15f;
	Vector3Double velocityIncrement = Vector3Double( 0.0, 0.0, 10.0 );
	Vector3Double positionIncrement = Vector3Double( 0.0, 0.0, .50 );

	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD1 ))
	{
		m_testCloth.AdjustStiffness( Spring::SPRING_STRUCTURAL, -stiffIncrement );
	}
	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD2 ))
	{
		m_testCloth.AdjustStiffness( Spring::SPRING_STRUCTURAL, stiffIncrement );
	}
	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD4 ))
	{
		m_testCloth.AdjustStiffness( Spring::SPRING_SHEAR, -stiffIncrement );
	}
	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD5 ))
	{
		m_testCloth.AdjustStiffness( Spring::SPRING_SHEAR, stiffIncrement );
	}
	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD7 ))
	{
		m_testCloth.AdjustStiffness( Spring::SPRING_BEND, -stiffIncrement );
	}
	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD8 ))
	{
		m_testCloth.AdjustStiffness( Spring::SPRING_BEND, stiffIncrement );
	}
	

	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD3 ))
	{
		m_testCloth.AddVelocity( velocityIncrement );
	}
	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD6 ))
	{
		m_testCloth.InstantDisplacement( positionIncrement );
	}
	if (g_theApp->m_inputSystem.IsKeyJustPressed( VK_NUMPAD9 ))
	{
		m_testCloth.AddVelocity( Vector3Double(3.0, 3.0, 6.0) );
	}

	//   ----------------------------------------------------------------------

	//Textures!

	if (g_theApp->m_inputSystem.IsKeyJustPressed('1'))
	{
		g_texIndexToDraw = 0;
	}
	else if (g_theApp->m_inputSystem.IsKeyJustPressed('2'))
	{
		g_texIndexToDraw = 1;
	}
	else if (g_theApp->m_inputSystem.IsKeyJustPressed('3'))
	{
		g_texIndexToDraw = 2;
	}
	else if (g_theApp->m_inputSystem.IsKeyJustPressed('4'))
	{
		g_texIndexToDraw = 3;
	}



	if (g_theApp->m_inputSystem.IsKeyJustPressed(0xc0) && g_theApp->m_inputSystem.IsKeyDown(0xc0)) //~
	{
		g_theConsole->m_isOpen = !g_theConsole->m_isOpen;
	}
	if (!g_theConsole->m_isOpen)
	{
		m_world->Update(deltaSeconds);
		if (g_theApp->m_inputSystem.IsKeyPressed('M'))
		{
			m_TestYoyo.AddImpulse();
		}
	}
	else
	{
		g_theConsole->Update(deltaSeconds);
	}
	if (g_theApp->m_inputSystem.IsKeyPressed('O'))
	{
		g_debugDrawing = !g_debugDrawing;
	}
	if (g_theApp->m_inputSystem.IsKeyPressed('P'))
	{
		g_effectState = !g_effectState;
	}

	// Handle lane selection input
	HandleLaneSelectionInput();

	// Handle ball spawning input
	HandleBallSpawningInput();

	g_theApp->m_inputSystem.UpdateKeyJustPressedState();
	g_theConsole->m_inputSystem.UpdateKeyJustPressedState();
	

//	m_TestYoyo.Update( deltaSeconds );
//	m_testNode.Update( deltaSeconds );
//	m_testFixedNode.Update( deltaSeconds );
//	m_testFixedNode2.Update( deltaSeconds );
	float deltaTime = 0.016f;

	m_testCloth.Update( deltaTime );

	// Update scoring nodes
	m_scoringNodes->Update( deltaTime );

	//Update
	for (auto ballIter = m_balls.begin(); ballIter != m_balls.end(); ++ballIter)
	{
		(*ballIter)->Update(deltaTime);
	}

	for (auto ballIter1 = m_balls.begin(); ballIter1 != m_balls.end(); ++ballIter1)
	{
		for (auto pegIter = m_testCloth.m_fixedNodes.begin(); pegIter != m_testCloth.m_fixedNodes.end(); ++pegIter)
		{
			(*ballIter1)->CollideWithNode((*pegIter), 1.f, deltaTime);
		}
	}

	//Collide
	for (auto iter = m_clothCollisionHelper.begin(); iter != m_clothCollisionHelper.end();)
	{
		if ((*iter)->m_ball)
		{
			(*iter)->DoClotheHelper();
			++iter;
		}
		else
		{
			iter = m_clothCollisionHelper.erase( iter );
		}
	}
	if ( g_theApp->m_inputSystem.IsKeyDown( 'G' ) )
	{
		bool breakNow = true;
		breakNow;
	}
}

void TheGame::Render(const float deltaSeconds) const
{

	g_theRenderer->ClearScreenDepth(0.5f, 0.85f, 0.9f);
	SetUpPerspectiveProjection();
	g_theRenderer->RotateView(-90.f, 1.f, 0.f, 0.f);

	g_theRenderer->RotateView(-g_camera.m_orientation.m_pitchDegreesAboutX, 1.f, 0.f, 0.f);
	g_theRenderer->RotateView(g_camera.m_orientation.m_yawDegreesAboutZ, 0.f, 0.f, 1.f);
	g_theRenderer->TranslateView(-g_camera.m_position);


	m_world->Render();

// 	Vertex3D_PCT testVerts[3];
// 	testVerts[0] = Vertex3D_PCT(Vector3(0.f, 0.f, 1.f), Rgba::WHITE, Vector2(0.f, 1.f));
// 	testVerts[1] = Vertex3D_PCT(Vector3(1.f, 0.f, 1.f), Rgba::WHITE, Vector2(0.f, 1.f));
// 	testVerts[2] = Vertex3D_PCT(Vector3(1.f, 0.f, 2.f), Rgba::WHITE, Vector2(0.f, 1.f));
// 	g_theRenderer->DrawVertexArray_PCT(&testVerts[0], 3, PRIMITIVE_TRIANGLES);
	

	// Render scoring nodes
	m_scoringNodes->Render();

	// Render lane selection indicator
	DrawSelectedLaneIndicator();

	if (g_debugDrawing)
	{
		g_theRenderer->DrawAxes();
	}

	//g_theRenderer->DrawGrid(20, 20);
	g_theRenderer->HandleRenderCommands();
	g_theRenderer->UpdateRenderCommands(deltaSeconds);
	
	//g_theRenderer->DrawSphere(Vector3(1.f, 1.f, 1.f), 4.f);
	g_theRenderer->SetColor(1.f, 1.f, 1.f, 1.f);
	static BitmapFontMeta* bitmapFont = BitmapFontMeta::CreateOrGetFont(FONT_NAME);
	//g_theRenderer->DrawTextMeta2D(Vector3(8.f, 2.f, 15.f), 0.02f, "Krunk ain't dead.", Rgba::WHITE, bitmapFont, Vector3(1.f,0.f,0.f), Vector3(0.f, 0.f,-1.f));
	static float m_age;
	m_age += deltaSeconds;
	std::string text = Stringf("Camera: %.1f,%.1f,%.1f", g_camera.m_position.x, g_camera.m_position.y, g_camera.m_position.z );
	g_theRenderer->RenderStep(deltaSeconds);
	for (Ball* currentBall : m_balls)
	{
// 		m_lightMeshRenderer->SetTransform(Transform(currentBall.m_state.m_position, Quaternion::ZERO, Vector3(currentBall.m_radius*2.f)));
// 		g_theRenderer->MeshRender(m_lightMeshRenderer, m_camera);
		g_theRenderer->DrawSphere(currentBall->m_state.m_position, currentBall->m_radius, Rgba::RED);
	}


	Vector3 clothTopLeftPos = Vector3( ( float ) m_testCloth.m_topLeftClothPosition.x, ( float ) m_testCloth.m_topLeftClothPosition.y, ( float ) m_testCloth.m_topLeftClothPosition.z - 1.0f );
	Vector3 clothTopRightPos = clothTopLeftPos;
	clothTopRightPos.x += m_testCloth.m_width - 1;
	Vector3 clothBottomLeftPos = clothTopLeftPos;
	clothBottomLeftPos.y -= m_testCloth.m_height - 1;
	Vector3 clothBottomRightPos = clothBottomLeftPos;
	clothBottomRightPos.x += m_testCloth.m_width - 1;

	g_theRenderer->DrawTexturedQuad3D(*m_testTex, clothBottomLeftPos, clothBottomRightPos, clothTopRightPos, clothTopLeftPos,
		AABB2(Vector2(0.f, 0.f), Vector2(1.f, 4.f)));
	m_testCloth.Render();
	g_theRenderer->SetOrtho(Vector2(0.f, 1600.f), Vector2(0.f, 900.f));
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "SquirrelFixedFont.png" );
	std::string titleString = Stringf( "PachinkoSim" );
	std::string scoreString = Stringf( "Score: %i", m_score );
	g_theRenderer->DrawText2D( Vector2( 700.0f, 870.0f ), titleString, 25.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( Vector2( 700.0f, 0.0f ), scoreString, 25.0f, Rgba::WHITE, fixedFont );
	g_theRenderer->SetDepthTest(false);
//	g_theRenderer->DrawTextMeta2D(Vector3(50.f,80.f,0.f),1.f, text, Rgba::BLUE, bitmapFont);
	g_theConsole->Render();

// 	if (g_theGame->m_world->m_camMode != CameraMode::FAT_CAM)
// 	{
		g_theGame->DrawCrosshair();
		// g_theRenderer->SetCullFace(false);
// 	}

// 	m_TestYoyo.Render(  );
// 	m_testNode.Render();
// 	m_testFixedNode.Render();
// 	m_testFixedNode2.Render();
	

}


//-----------------------------------------------------------------------------------------------
void TheGame::DebugText(std::string text)
{
	//g_theRenderer->DrawText2D(Vector2(20.f, 800.f), text, 15.f, Rgba::WHITE, m_font);
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawCrosshair()
{
	Rgba color = Rgba::WHITE;
	Vector2 xhairBottom = Vector2((float)snapBackPos.x, (float)snapBackPos.y - XHAIR_LENGTH);
	Vector2 xhairTop = Vector2((float)snapBackPos.x, (float)snapBackPos.y + XHAIR_LENGTH);
	Vector2 xhairLeft = Vector2((float)snapBackPos.x - XHAIR_LENGTH, (float)snapBackPos.y);
	Vector2 xhairRight = Vector2((float)snapBackPos.x + XHAIR_LENGTH, (float)snapBackPos.y);

	g_theRenderer->SetInverseDestBlend();
	g_theRenderer->DrawLine(xhairBottom, xhairTop, color.m_red, color.m_green, color.m_blue, color.m_alpha, 2.f);
	g_theRenderer->DrawLine(xhairLeft, xhairRight, color.m_red, color.m_green, color.m_blue, color.m_alpha, 2.f);
	g_theRenderer->SetAlphaBlend();
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleBallSpawningInput()
{
	if ( g_theApp->m_inputSystem.IsKeyJustPressed( VK_SPACE ) && g_theApp->m_inputSystem.IsKeyDown( VK_SPACE ) )
	{
		Vector3 ballSpawnPosition;

		ballSpawnPosition = Vector3( ( float ) m_testCloth.m_topLeftClothPosition.x + m_currentLane + 0.5f, ( float ) m_testCloth.m_topLeftClothPosition.y - 0.5f, 5.0f );

		Ball* tmpBall = new Ball( 100.f, .3f, BallState( ballSpawnPosition, Vector3( 0.f, 0.f, -0.0f ) ) );
		m_balls.push_back( tmpBall );
		ClothCollisionHelper* tmp = new ClothCollisionHelper();
		tmp->m_cloth = &m_testCloth;
		tmp->m_ball = m_balls[m_balls.size() - 1];
		m_clothCollisionHelper.push_back( tmp );
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::HandleLaneSelectionInput()
{
	if ( g_theApp->m_inputSystem.IsKeyJustPressed( VK_LEFT ) && g_theApp->m_inputSystem.IsKeyDown( VK_LEFT ) )
	{
		if ( m_currentLane > 0 )
		{
			m_currentLane--;
		}
		else
		{
			m_currentLane = ( m_testCloth.m_width - 2 );
		}
	}

	if ( g_theApp->m_inputSystem.IsKeyJustPressed( VK_RIGHT ) && g_theApp->m_inputSystem.IsKeyDown( VK_RIGHT ) )
	{
		if ( m_currentLane < ( m_testCloth.m_width - 2 ) )
		{
			m_currentLane++;
		}
		else
		{
			m_currentLane = 0;
		}
	}
}


//-----------------------------------------------------------------------------------------------
void TheGame::DrawSelectedLaneIndicator() const
{
	Vector3 vertex1( ( float ) m_testCloth.m_topLeftClothPosition.x + m_currentLane, ( float ) m_testCloth.m_topLeftClothPosition.y, 4.0f );
	Vector3 vertex2( ( float ) m_testCloth.m_topLeftClothPosition.x + m_currentLane + 1.0f, ( float ) m_testCloth.m_topLeftClothPosition.y, 4.0f );
	Vector3 vertex3( ( float ) m_testCloth.m_topLeftClothPosition.x + m_currentLane + 1.0f, ( float ) m_testCloth.m_topLeftClothPosition.y, 5.0f );
	Vector3 vertex4( ( float ) m_testCloth.m_topLeftClothPosition.x + m_currentLane, ( float ) m_testCloth.m_topLeftClothPosition.y, 5.0f );
	AABB2 texCoords( Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ) );
	Texture* arrowTexture = Texture::CreateOrGetTexture( "Data/Images/arrow.png" );
	g_theRenderer->DrawTexturedQuad3D( *arrowTexture, vertex1, vertex2, vertex3, vertex4, texCoords );
	
	Vector3 laneIndicatorPosition;
	laneIndicatorPosition = Vector3( ( float ) m_testCloth.m_topLeftClothPosition.x + m_currentLane + 0.5f, ( float ) m_testCloth.m_topLeftClothPosition.y - 0.5f, 5.0f );
	//g_theRenderer->DrawSphere( laneIndicatorPosition, 0.5f, Rgba::RED );
}


//-----------------------------------------------------------------------------------------------
void DrawDebugArrow( Command& args )
{
	
	if (args.m_argList.empty())
	{
		g_theRenderer->AddDebugArrow(Vector3(1.f, 1.f, 1.f), Vector3(10.f, 50.f, 10.f), Rgba::YELLOW, 2.f, RenderCommand::XRAY);
	}
}