#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Camera3D.hpp"
#include "Engine/Math/EulerAngles.hpp"

Camera3D g_camera;
float g_engineDeltaSeconds;
bool g_effectState;
bool g_debugDrawing;
void InitializeEngineCommon()
{
	g_engineDeltaSeconds = 0.f;
	g_effectState = false;
	g_debugDrawing = false;
	
	g_camera.m_position = Vector3::ZERO;
	g_camera.m_orientation = EulerAngles::ZERO;
}

