#include "Game/World.hpp"
#include "Game/TheApp.hpp"
//#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/TheGame.hpp"
#include <string>
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"

const int WORLD_X = 4;
const int WORLD_Y = 4;


World::World()
	: m_startTrace(0.f, 0.f, 0.f)
	, m_endTrace(0.f, 0.f, 0.f)
{

	//m_player = Player(Vector3(2.3f, 2.3f, 69.f + (1.85f / 2.f)));
	m_player = Player(Vector3(10.f, -10.f, 15.f));
	//m_camera.m_position.SetXYZ(2.f, 2.f, 70.f);
	m_camera.m_position = m_player.m_pos;
	g_camera.m_orientation.m_pitchDegreesAboutX -= 60.f;
	m_moveMod = 1.f;

}

void World::Update(float deltaSeconds)
{
	UpdateCameraAndPlayer(deltaSeconds);

}

void World::Render()
{
	g_theRenderer->SetAlphaTest(true, 0.5f);
	//g_theRenderer->DrawAABB3Wireframe(m_player.m_bounds, Rgba::WHITE);
	g_theRenderer->SetAlphaTest(false);
}

void World::UpdateCameraAndPlayer(float deltaSeconds)
{
	if (!g_theApp->m_inputSystem.m_isInFocus)
		return;
	//Raycast endpos : dist * ForwardXYZ().Norm + cameraPos
	IntVector2 cursorPos = g_theApp->m_inputSystem.GetMousePos();
	g_theApp->m_inputSystem.ShowMouse(false);
	g_theApp->m_inputSystem.SetMousePos(snapBackPos.x, snapBackPos.y);
	IntVector2 cursorDelta = cursorPos - snapBackPos;


	if (cursorDelta.x != 0 || cursorDelta.y != 0)
	{
		//m_camera.m_orientation.m_yawDegreesAboutZ -= mouseSensitivity  * (float)cursorDelta.x;
		//m_camera.m_orientation.m_pitchDegreesAboutY += mouseSensitivity  * (float)cursorDelta.y;

// 		g_camera.m_orientation.m_yawDegreesAboutZ += mouseSensitivity  * (float)cursorDelta.x;
// 		g_camera.m_orientation.m_pitchDegreesAboutX -= mouseSensitivity  * (float)cursorDelta.y;
	}

	//m_camera.FixAndClampAngles();
	g_camera.FixAndClampAngles();

	m_player.ClearMoveStates();

	if (g_theApp->m_inputSystem.IsKeyDown(VK_SHIFT))
	{
		m_player.m_isSprinting = true;
		//m_moveMod = 8.f;
	}
	else
	{
		m_player.m_isSprinting = false;
		//m_moveMod = 1.f;
	}

	if (g_theApp->m_inputSystem.IsKeyDown('W'))
	{
		m_player.m_isMovingForward = true;
		m_player.m_isMovingBackwards = false;
		//m_camera.m_position += m_camera.GetForwardXY() * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	else if (g_theApp->m_inputSystem.IsKeyDown('S'))
	{
		m_player.m_isMovingForward = false;
		m_player.m_isMovingBackwards = true;
		//m_camera.m_position -= m_camera.GetForwardXY() * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	if (g_theApp->m_inputSystem.IsKeyDown('A'))
	{
		m_player.m_isMovingLeft = true;
		m_player.m_isMovingRight = false;
		//m_camera.m_position += m_camera.GetLeftXY() * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	else if (g_theApp->m_inputSystem.IsKeyDown('D'))
	{
		m_player.m_isMovingLeft = false;
		m_player.m_isMovingRight = true;
		//m_camera.m_position -= m_camera.GetLeftXY() * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	if (g_theApp->m_inputSystem.IsKeyDown('Q'))
	{
		m_player.m_isMovingUp = true;
		m_player.m_isMovingDown = false;
		//m_camera.m_position.z += VERT_MOD * MOVE_SPEED * m_moveMod * deltaSeconds;
	}
	else if (g_theApp->m_inputSystem.IsKeyDown('Z'))
	{
		m_player.m_isMovingUp = false;
		m_player.m_isMovingDown = true;
		//m_camera.m_position.z -= VERT_MOD * MOVE_SPEED * m_moveMod * deltaSeconds;
	}

	m_player.Update(deltaSeconds);




	//TODO: MOUSE WHEEL
	int mouseWheelDir = g_theApp->m_inputSystem.GetMouseWheelDirection();

	if (mouseWheelDir != 0)
	{
		/*
		SoundID sound = g_audio->CreateOrGetSound(BLOCK_SELECT_SOUND);
		g_audio->PlaySound(sound, 0.5f);

		if (mouseWheelDir > 0)
		{
			m_selectedBlockIndex++;
			if (m_selectedBlockIndex > 9)
				m_selectedBlockIndex = 0;
		}
		else
		{
			m_selectedBlockIndex--;
			if (m_selectedBlockIndex < 0)
				m_selectedBlockIndex = 9;
		}
		*/
	}




	//Vector3 cameraForwardDirection = m_camera.GetForwardXYZ().GetNormalized();
	Vector3 cameraForwardDirection = g_camera.GetForwardXYZ().GetNormalized();
	//DebuggerPrintf("Camera Pos: X: %f,Y: %f, Z:%f\nCamForwardXYZ: X: %f,Y: %f, Z:%f\n", m_camera.m_position.x, m_camera.m_position.y, m_camera.m_position.z, cameraForwardDirection.x, cameraForwardDirection.y, cameraForwardDirection.z);

	


}