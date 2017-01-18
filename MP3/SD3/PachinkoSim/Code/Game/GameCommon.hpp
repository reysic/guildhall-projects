#pragma once
#include <vector>
class Vector3Double;
class Rgba;
struct Vertex3D_PC;



#define BIT(x) (1<<(x))
const int BITS_WIDE_X = 4;
const int BITS_WIDE_Y = 4;
const int BITS_WIDE_Z = 7;







extern bool g_lockLastTrace;
extern bool g_showDirtyBlocks;
extern bool g_DebugWhiteBlocks;
extern bool g_HandleEdgeLights;
extern bool g_DrawWireFrame;
extern bool g_GravityOn;
extern bool g_enableBlockSounds;
extern int g_texIndexToDraw;

extern std::vector<Vertex3D_PC>  g_debugPoints;

void InitializeCommon();

void AddDebugPoint(const Vector3Double& position, const Rgba& color); // push_back in vector
void DrawDebugPoints(float pointSize, bool enableDepthTesting); // fastest using VAs!
void ClearDebugPoints(); // clear the (global) vector
