
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include <gl/GLU.h>
#include <direct.h>
#include <sstream>
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Camera3D.hpp"
#include "Engine/Core/StringUtils.hpp"


#pragma warning( disable : 4201 )  // nonstandard extension used: nameless struct/union
#pragma warning( disable : 4127 )  // conditional expression is constant

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "glu32" ) // Link in the OpenGL32.lib static library

Renderer* g_theRenderer = nullptr;
std::vector<RenderCommand*> g_Commands;
const std::string DEFAULT_IMAGE_FILE = "Data/Images/TestImageWhite.png";

const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const int WINDOW_PHYSICAL_WIDTH = 1600;
const int WINDOW_PHYSICAL_HEIGHT = 900;
const double VIEW_LEFT = 0.0;
const double VIEW_RIGHT = 1600.0;
const double VIEW_BOTTOM = 0.0;
const double VIEW_TOP = VIEW_RIGHT * static_cast<double>(WINDOW_PHYSICAL_HEIGHT) / static_cast<double>(WINDOW_PHYSICAL_WIDTH);

static GLuint gShaderProgram;
static GLuint gVAO = NULL;
static GLuint gVBO = NULL;
static GLuint gvIBO = NULL;
static GLuint gShaderTimedProgram;

static GLuint gSamplerId = NULL;
static GLuint gDiffuseTex = NULL;
static GLuint gNormalTex = NULL;
static GLuint gSpecularTex = NULL;


Renderer::Renderer() : g_DefaultImage(DEFAULT_IMAGE_FILE, 1, 1)
{
	InitializeEngineCommon();
	const char* versionString = (const char*) glGetString(GL_VERSION);
	DebuggerPrintf("OpenGL version: %s\n", versionString);
	const char* glslString = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
	DebuggerPrintf("GLSL version: %s\n", glslString);

	glGenBuffers = (PFNGLGENBUFFERSPROC) wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");

	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");

	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("wglGetProcAddress");

	glGenVertexArrays		= (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
	glDeleteVertexArrays			= (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
	glBindVertexArray			= (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
	glGetAttribLocation			= (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer		= (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
	glUseProgram				= (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");

	glGetUniformLocation	= (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1fv			= (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform3fv			= (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv			= (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniform1iv			= (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniformMatrix4fv		= (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");

	glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	glBindSampler = (PFNGLBINDSAMPLERPROC)wglGetProcAddress("glBindSampler");
	glGenSamplers = (PFNGLGENSAMPLERSPROC)wglGetProcAddress("glGenSamplers");
	glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)wglGetProcAddress("glSamplerParameteri");

// 	GLuint vs = LoadShader("Data/Shaders/fvf.vert", GL_VERTEX_SHADER);
// 	GLuint fs = LoadShader("Data/Shaders/fvf.frag", GL_FRAGMENT_SHADER);
	std::string vsStr = ("Data/Shaders/basic_light.vert");
	std::string fsStr = ("Data/Shaders/basic_light.frag");
	std::string vsTimedStr = "Data/Shaders/fvf2.vert";
	std::string fsTimedStr = "Data/Shaders/fvf2.frag";
	gShaderProgram = CreateAndLinkProgram(vsStr, fsStr);
	gShaderTimedProgram = CreateAndLinkProgram(vsTimedStr, fsTimedStr);
	ASSERT_OR_DIE(gShaderProgram != NULL, "");

// 	glDeleteShader(vs);
// 	glDeleteShader(fs);
	
	ShaderProgramSetVec4(gShaderProgram, "gColor", vec4_fl( 1.f, 1.f, 1.f,1.f));
	
	
	ShaderProgramSetVec4(gShaderTimedProgram, "gColor", vec4_fl(1.0f, 1.0f, 1.0f, 1.0f));
	//gLightPosition
	
	Vertex3D_PCT quad[4];
	quad[0].m_position = Vector3( -0.5f, +0.0f, -0.5f); //BL
	quad[1].m_position = Vector3( +0.5f, +0.0f, +0.5f); //TR
	quad[2].m_position = Vector3( +0.5f, +0.0f, -0.5f); //BR
	quad[3].m_position = Vector3( -0.5f, +0.0f, +0.5f); //TL

	quad[0].m_color = Rgba::BLUE;
	quad[1].m_color = Rgba::RED;
	quad[2].m_color = Rgba::GREEN;
	quad[3].m_color = Rgba::YELLOW;

	quad[0].m_texCoords = Vector2(0.f,1.f); //BL
	quad[1].m_texCoords = Vector2(1.f,0.f); //TR
	quad[2].m_texCoords = Vector2(1.f,1.f); //BR
	quad[3].m_texCoords = Vector2(0.f,0.f); //TL


	//Cube mesh
	m_cube = new Mesh();
	m_cube->MakeCubeMesh();

	m_cubeVBO = RenderBufferCreate(&m_cube->m_verts[0], m_cube->m_verts.size(), sizeof(m_cube->m_verts[0]), GL_STATIC_DRAW);

	m_cubeIBO = RenderBufferCreate(&m_cube->m_indices[0], m_cube->m_indices.size(), sizeof(m_cube->m_indices[0]), GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_cubeVAO);
	ASSERT_OR_DIE(m_cubeVAO != NULL, "Error generating vertex arrays");

	BindMeshToVAO(m_cubeVAO, m_cubeVBO, m_cubeIBO, gShaderProgram);

	gSamplerId = CreateSampler(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);

	BindBuffer(0);


	//Sphere mesh
	m_sphere = new Mesh();
	m_sphere->MakeSphereMesh();

	m_sphereVBO = RenderBufferCreate(&m_sphere->m_verts[0], m_sphere->m_verts.size(), sizeof(m_sphere->m_verts[0]), GL_STATIC_DRAW);

	m_sphereIBO = RenderBufferCreate(&m_sphere->m_indices[0], m_sphere->m_indices.size(), sizeof(m_sphere->m_indices[0]), GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_sphereVAO);
	ASSERT_OR_DIE(m_sphereVAO != NULL, "Error generating vertex arrays");

	BindMeshToVAO(m_sphereVAO, m_sphereVBO, m_sphereIBO, gShaderTimedProgram);


	uint16_t indicies[] = {
		0,1,2,
		0,3,1
	};
	/*
	GenerateBuffer(1, &gVBO);
	BindBuffer(gVBO);
	BufferData(sizeof(Vertex3D_PCT) * 24, &cubeVerts[0]);
	BindBuffer(0);

	

	//gVBO = RenderBufferCreate(quad, 4, sizeof(quad[0]));
//  	GenerateBuffer(1, &gvIBO);
// 	BindBuffer(gvIBO);
// 	BufferData(sizeof(uint16_t) * 4, quad);
	gvIBO = RenderBufferCreate(&cubeIndices[0], 36, sizeof(cubeIndices[0]));
	glGenVertexArrays(1, &gVAO);
	BindMeshToVAO(gVAO, gVBO, gvIBO,  gShaderProgram);

	*/
	gSamplerId = CreateSampler(GL_NEAREST, GL_NEAREST, GL_REPEAT, GL_REPEAT);

	//gDiffuseTex = Texture::CreateOrGetTexture("Data/Images/joltik.png")->m_openglTextureID;
	gDiffuseTex = Texture::CreateOrGetTexture("Data/Images/cobblestone.jpg")->m_openglTextureID;
	gNormalTex = Texture::CreateOrGetTexture("Data/Images/cobblestone_normal.jpg")->m_openglTextureID;
	gSpecularTex = Texture::CreateOrGetTexture("Data/Images/cobblestone_spec.jpg")->m_openglTextureID;
	ShaderProgramSetInt(gShaderProgram, "gDiffuseTex", 0);
	ShaderProgramSetInt(gShaderTimedProgram, "gEffectState", g_effectState);
	ShaderProgramSetInt(gShaderTimedProgram, "gDiffuseTex", 0);
	BindBuffer(0);
	
	
	
}

void Renderer::CreateScreen()
{
	glOrtho(VIEW_LEFT, VIEW_RIGHT, VIEW_BOTTOM, VIEW_TOP, 0.f, 1.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.f);
	glEnable(GL_LINE_SMOOTH);
}

void Renderer::ClearScreen(float red, float green, float blue)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(red, green, blue, 1.f);
}

void Renderer::SetOrtho(const Vector2& leftRight, const Vector2& bottomTop)
{
	glLoadIdentity();
	glOrtho(leftRight.x, leftRight.y, bottomTop.x, bottomTop.y, 0.f, 1.f);
}

void Renderer::DrawLine(const Vector2& start, const Vector2& end, float red, float green, float blue, float alpha, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	glDisable(GL_TEXTURE_2D);
	SetColor(red, green, blue, alpha);
	glLineWidth(lineThickness);

	unsigned char uRed = FloatToUChar(red);
	unsigned char uGreen = FloatToUChar(green);
	unsigned char uBlue = FloatToUChar(blue);
	unsigned char uAlpha = FloatToUChar(alpha);

	Rgba color = Rgba(uRed, uGreen, uBlue, uAlpha);

	const int numVerts = 2;
	Vertex3D_PCT verts[numVerts];
	Vector2 texCoords = Vector2(0.f, 1.f);
	verts[0].m_position = Vector3(start.x, start.y, 0.f);
	verts[0].m_color = color;
	verts[0].m_texCoords = texCoords;

	verts[1].m_position = Vector3(end.x, end.y, 0.f);
	verts[1].m_color = color;
	verts[1].m_texCoords = texCoords;

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		glVertex2f(start.x, start.y);
		glVertex2f(end.x, end.y);
	}
	glEnd();
	*/
}


void Renderer::DrawLine(const Vector3& start, const Vector3& end, float red, float green, float blue, float alpha, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	glEnable(GL_TEXTURE_2D);
	SetColor(red, green, blue, alpha);
	glLineWidth(lineThickness);

	unsigned char uRed = FloatToUChar(red);
	unsigned char uGreen = FloatToUChar(green);
	unsigned char uBlue = FloatToUChar(blue);
	unsigned char uAlpha = FloatToUChar(alpha);

	Rgba color = Rgba(uRed, uGreen, uBlue, uAlpha);

	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 2;
	Vertex3D_PCT verts[numVerts];
	verts[0].m_position = Vector3(start.x, start.y, start.z);
	verts[0].m_color = color;
	verts[0].m_texCoords = Vector2(0.f, 0.f);

	verts[1].m_position = Vector3(end.x, end.y, end.z);
	verts[1].m_color = color;
	verts[1].m_texCoords = Vector2(1.f, 1.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);
	}
	glEnd();
	*/
}


void Renderer::DrawLine(const Vector3& start, const Vector3& end, Rgba color, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	SetColor(color.m_red, color.m_green, color.m_blue, color.m_alpha);
	glLineWidth(lineThickness);
	glEnable(GL_TEXTURE_2D);
	Vector2 texCoords = Vector2(1.f, 1.f);
	const int numVerts = 2;
	Vertex3D_PCT verts[numVerts];
	verts[0].m_position = Vector3(start.x, start.y, start.z);
	verts[0].m_color = color;
	verts[0].m_texCoords = Vector2(0.f, 0.f);

	verts[1].m_position = Vector3(end.x, end.y, end.z);
	verts[1].m_color = color;
	verts[1].m_texCoords = Vector2(1.f, 1.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		glVertex3f(start.x, start.y, start.z);
		glVertex3f(end.x, end.y, end.z);
	}
	glEnd();
	*/
}

void Renderer::DrawLineLoop(const std::vector< Vertex3D_PCT >& vertexes, float lineThickness)
{
	for (unsigned int i = 1; i < vertexes.size(); i++)
	{
		DrawLine(vertexes[i - 1].m_position, vertexes[i].m_position, vertexes[i].m_color, lineThickness);
	}
	DrawLine(vertexes[vertexes.size() - 1].m_position, vertexes[0].m_position, vertexes[0].m_color, lineThickness);
}

void Renderer::RenderPolygon(float centerX, float centerY, float radius, float numSides, float degreesOffset)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	//const float pi = 3.141592653589793f;
	const float radiansTotal = 2.f * pi;
	const float radiansPerSide = radiansTotal / numSides;

	Vector2 texCoords = Vector2(0.f, 1.f);
	std::vector<Vertex3D_PCT> verts;
	int index = 0;

	for (float radians = 0.f; radians < radiansTotal; radians += radiansPerSide)
	{
		Vertex3D_PCT vert;
		float x = centerX + (radius * cos(radians + degreesOffset));
		float y = centerY + (radius * sin(radians + degreesOffset));
		vert.m_position = Vector3(x, y, 0.f);
		vert.m_color = Rgba::WHITE;
		vert.m_texCoords = texCoords;
		/*
		verts[index].m_position = Vector3(x, y,0.f);
		verts[index].m_color = Rgba::WHITE;
		verts[index].m_texCoords = texCoords;
		*/
		verts.push_back(vert);
		index++;
	}
	DrawVertexArray_PCT(&verts[0], index, PRIMITIVE_LINES_LOOP);
	/*
	glBegin(GL_LINE_LOOP);
	{
		for (float radians = 0.f; radians < radiansTotal; radians += radiansPerSide)
		{
			float x = centerX + (radius * cos(radians + degreesOffset));
			float y = centerY + (radius * sin(radians + degreesOffset));
			glVertex2f(x, y);
		}
	}
	glEnd();
	*/
}

void Renderer::SetColor(float red, float green, float blue, float alpha)
{
	glColor4f(red, green, blue, alpha);
}

void Renderer::SetColor(Rgba color)
{
	glColor4f(color.m_red, color.m_green, color.m_blue, color.m_alpha);
}

void Renderer::DrawLoop(Vector2* vecArray[], int vertCount, float red, float green, float blue, float alpha, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	SetColor(red, green, blue, alpha);
	glLineWidth(lineThickness);
	Vector2 texCoords = Vector2(0.f, 1.f);
	Vertex3D_PCT* verts = new Vertex3D_PCT;

	unsigned char uRed = FloatToUChar(red);
	unsigned char uGreen = FloatToUChar(green);
	unsigned char uBlue = FloatToUChar(blue);
	unsigned char uAlpha = FloatToUChar(alpha);

	Vector2* thisVertex;
	for (int i = 0; i < vertCount; i++)
	{
		thisVertex = vecArray[i];
		verts[i].m_position = Vector3(thisVertex->x, thisVertex->y, 0.f);
		verts[i].m_color = Rgba(uRed, uGreen, uBlue, uAlpha);
		verts[i].m_texCoords = texCoords;
	}


	DrawVertexArray_PCT(verts, vertCount, PRIMITIVE_LINES_LOOP);
	/*
	glBegin(GL_LINE_LOOP);
	{
		Vector2* thisVertex;
		for (int i = 0; i < vertCount; i++)
		{
			thisVertex = vecArray[i];
			glVertex2f(thisVertex->x, thisVertex->y);
		}
	}
	glEnd();
	*/
}

void Renderer::DrawLoop(AABB2 bounds, Rgba color, float lineThickness)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	SetColor(color.m_red, color.m_alpha, color.m_blue, color.m_alpha);
	glLineWidth(lineThickness);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_texCoords = texCoords;
		verts[i].m_color = color;
	}


	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);

	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES_LOOP);
	/*
	glBegin(GL_LINE_LOOP);
	{
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);

	}
	glEnd();
	*/
}

//Draws a flat shaded (solid, non-textured) quad
void Renderer::DrawAABB(const AABB2& bounds, const Rgba& color)
{
	glEnable(GL_TEXTURE_2D);
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	SetColor(color.m_red, color.m_green, color.m_blue, color.m_alpha);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_texCoords = texCoords;
		verts[i].m_color = color;
	}

	
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);
	
	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_QUADS);
	/*
	glBegin(GL_QUADS);
	{
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
	}
	glEnd();
	*/
	glDisable(GL_TEXTURE_2D);
}

//Draws a textured quad
void Renderer::DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.m_openglTextureID);

	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_color = Rgba::WHITE;
	}

	verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);

	verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);
	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);
	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);
	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_QUADS);
	/*
	glBegin(GL_QUADS);
	{
		//Tex Coords start at the top left, vertex coords start at the bottom left

		glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);

		glTexCoord2f(texCoordMins.x, texCoordMins.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
	}
	glEnd();
	*/
	glDisable(GL_TEXTURE_2D);
}

//Draws a textured (and tinted) quad
void Renderer::DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.m_openglTextureID);
	glColor4f(tint.m_red, tint.m_green, tint.m_blue, tint.m_alpha);


	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_color = tint;
	}

	verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);

	verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);
	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);
	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);
	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_QUADS);
	/*
	glBegin(GL_QUADS);
	{
		//Tex Coords start at the top left, vertex coords start at the bottom left

		glTexCoord2f(texCoordMins.x, texCoordMaxs.y); 
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);

		glTexCoord2f(texCoordMins.x, texCoordMins.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
	}
	glEnd();
	*/
	glDisable(GL_TEXTURE_2D);
}

void Renderer::DrawTexturedAABB(const AABB2& bounds, const int& textureID, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);
	//BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];

	//TODO: No color here, may influence other games
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_color = tint;
	}

	verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, 0.f);

	verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);
	verts[1].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, 0.f);

	verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);
	verts[2].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, 0.f);

	verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);
	verts[3].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, 0.f);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_QUADS);

	//glColor4f(tint.m_red, tint.m_green, tint.m_blue, tint.m_alpha);
	/*
	glBegin(GL_QUADS);
	{
		//Tex Coords start at the top left, vertex coords start at the bottom left

		glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
		glVertex2f(bounds.m_mins.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_mins.y);

		glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
		glVertex2f(bounds.m_maxs.x, bounds.m_maxs.y);

		glTexCoord2f(texCoordMins.x, texCoordMins.y);
		glVertex2f(bounds.m_mins.x, bounds.m_maxs.y);
	}
	glEnd();
	*/
	glDisable(GL_TEXTURE_2D);
}



void Renderer::RotateView(const float& rotation)
{
	glRotatef(rotation, 0.f, 0.f, 1.f);
}

void Renderer::RotateView(float degrees, float x, float y, float z)
{
	glRotatef(degrees, x, y, z);
}

void Renderer::RotateView(float degrees, Vector3 axis)
{
	glRotatef(degrees, axis.x, axis.y, axis.z);
}

void Renderer::SetCullFace(bool enable)
{
	if (enable)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void Renderer::SetDepthTest(bool enable)
{
	if (enable)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void Renderer::TranslateView(const Vector2& position)
{
	glTranslatef(position.x, position.y, 0.f);
}

void Renderer::DrawPoint(const Vector3& pos, const Rgba& tint)
{
	Vertex3D_PCT verts[6];
	verts[0].m_color = tint;
	verts[0].m_position = Vector3(pos.x - 0.5f, pos.y,pos.z);
	verts[0].m_texCoords = Vector2(0.f, 1.f);

	verts[1].m_color = tint;
	verts[1].m_position = Vector3(pos.x + 0.5f, pos.y, pos.z);
	verts[1].m_texCoords = Vector2(0.f, 1.f);

	verts[2].m_color = tint;
	verts[2].m_position = Vector3(pos.x, pos.y - 0.5f, pos.z);
	verts[2].m_texCoords = Vector2(0.f, 1.f);

	verts[3].m_color = tint;
	verts[3].m_position = Vector3(pos.x, pos.y + 0.5f, pos.z);
	verts[3].m_texCoords = Vector2(0.f, 1.f);

	verts[4].m_color = tint;
	verts[4].m_position = Vector3(pos.x, pos.y, pos.z - 0.5f);
	verts[4].m_texCoords = Vector2(0.f, 1.f);

	verts[5].m_color = tint;
	verts[5].m_position = Vector3(pos.x, pos.y, pos.z + 0.5f);
	verts[5].m_texCoords = Vector2(0.f, 1.f);

	DrawVertexArray_PCT(verts, 6, PRIMITIVE_LINES);
}

void Renderer::DrawAxes()
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);
	Vector2 texCoords = Vector2(0.f, 1.f);
	const int numVerts = 6;
	Vertex3D_PCT verts[numVerts];
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);
	glLineWidth(2.f);

	verts[0].m_position = Vector3(0.f, 0.f, 0.f);
	verts[0].m_color = Rgba::RED;
	verts[0].m_texCoords = texCoords;

	verts[1].m_position = Vector3(1.f, 0.f, 0.f);
	verts[1].m_color = Rgba::RED;
	verts[1].m_texCoords = texCoords;

	verts[2].m_position = Vector3(0.f, 0.f, 0.f);
	verts[2].m_color = Rgba::GREEN;
	verts[2].m_texCoords = texCoords;

	verts[3].m_position = Vector3(0.f, 1.f, 0.f);
	verts[3].m_color = Rgba::GREEN;
	verts[3].m_texCoords = texCoords;

	verts[4].m_position = Vector3(0.f, 0.f, 0.f);
	verts[4].m_color = Rgba::BLUE;
	verts[4].m_texCoords = texCoords;

	verts[5].m_position = Vector3(0.f, 0.f, 1.f);
	verts[5].m_color = Rgba::BLUE;
	verts[5].m_texCoords = texCoords;

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		SetColor(1.f, 0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(1.f, 0.f, 0.f);

		SetColor(0.f, 1.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 1.f, 0.f);

		SetColor(0.f, 0.f, 1.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 0.f, 1.f);
	}
	glEnd();
	*/
	glEnable(GL_DEPTH_TEST);
	glLineWidth(3.f);

	verts[0].m_position = Vector3(0.f, 0.f, 0.f);
	verts[0].m_color = Rgba::RED;
	verts[0].m_texCoords = texCoords;

	verts[1].m_position = Vector3(1.f, 0.f, 0.f);
	verts[1].m_color = Rgba::RED;
	verts[1].m_texCoords = texCoords;

	verts[2].m_position = Vector3(0.f, 0.f, 0.f);
	verts[2].m_color = Rgba::GREEN;
	verts[2].m_texCoords = texCoords;

	verts[3].m_position = Vector3(0.f, 1.f, 0.f);
	verts[3].m_color = Rgba::GREEN;
	verts[3].m_texCoords = texCoords;

	verts[4].m_position = Vector3(0.f, 0.f, 0.f);
	verts[4].m_color = Rgba::BLUE;
	verts[4].m_texCoords = texCoords;

	verts[5].m_position = Vector3(0.f, 0.f, 1.f);
	verts[5].m_color = Rgba::BLUE;
	verts[5].m_texCoords = texCoords;

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);
	/*
	glBegin(GL_LINES);
	{
		SetColor(1.f, 0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(3.f, 0.f, 0.f);

		SetColor(0.f, 1.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 3.f, 0.f);

		SetColor(0.f, 0.f, 1.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 0.f, 3.f);
	}
	glEnd();
	*/
}

void Renderer::DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText, float cellHeight, 
	const Rgba& tint, const BitmapFont* font)
{
	//glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);

	Vector2 curMins = startBottomLeft;
	Vector2 curMaxs = Vector2(curMins.x + 16.f, curMins.y + cellHeight);
	AABB2 curPos; 
	Texture* tex = font->GetTexture();
	for (std::string::const_iterator it = asciiText.begin(); it != asciiText.end(); ++it) {

		curMaxs = Vector2(curMins.x + 16.f, curMins.y + cellHeight);
		curPos = AABB2(curMins, curMaxs);
		AABB2 texCoords = font->GetTexCoordsForGlyph(*it);
		DrawTexturedAABB(curPos, *tex, texCoords.m_mins, texCoords.m_maxs, tint);
		curMins.x += 16.f;
	}
}

void Renderer::DrawTextMeta2D(const Vector3& position, float scale, const std::string& asciiText, const Rgba& color, 
	BitmapFontMeta* font /*= nullptr*/, const Vector3& up /*= Vector3(0.f, 0.f, 1.f)*/, const Vector3& right /*= Vector3(1.f, 0.f, 0.f)*/)
{

	if (asciiText == "")
	{
		return;
	}

	Vector3 cursor = position;
	float tex_width = (float)font->m_textureWidth;
	float tex_height = (float)font->m_textureHeight;

	Glyph* prev_glyph = nullptr;
	const int vertLength = asciiText.length() * 4;
	std::vector<Vertex3D_PCT> verts;

	for (std::string::const_iterator it = asciiText.begin(); it != asciiText.end(); ++it)
	{
		Glyph* thisGlyph = font->m_glyphRegistry.at(*it);

		if (thisGlyph == nullptr)
			continue;
		int kerningVal = 0;
		if (prev_glyph != nullptr)
		{
			font->GetKerning(prev_glyph->m_charId, thisGlyph->m_charId, kerningVal);
		}
		

		cursor += kerningVal * scale * right;

		Vector3 topLeft = cursor + thisGlyph->m_xOffset * scale * right
			- thisGlyph->m_yOffset * scale * up;
// 		topLeft.x += thisGlyph->m_xOffset * scale;
// 		topLeft.y -= thisGlyph->m_yOffset * scale;

		Vector3 topRight = topLeft + scale * (float)thisGlyph->m_width * right;
		//topRight.x += scale * thisGlyph->m_width;

		Vector3 bottomLeft = topLeft - up * scale * (float)thisGlyph->m_height;
		//bottomLeft.y -= scale * thisGlyph->m_height;

		Vector3 bottomRight = bottomLeft + scale * (float)thisGlyph->m_width * right;
		//bottomRight.x += scale * thisGlyph->m_width;

		Vector2 uv_topLeft = Vector2((float) thisGlyph->m_xPos / tex_width, (float)thisGlyph->m_yPos / tex_height);

		Vector2 uv_topRight = uv_topLeft;
		uv_topRight.x += ((float)thisGlyph->m_width / tex_width);

		Vector2 uv_bottomLeft = uv_topLeft;
		uv_bottomLeft.y += ((float)thisGlyph->m_height / tex_height);

		Vector2 uv_bottomRight = uv_bottomLeft;
		uv_bottomRight.x += ((float)thisGlyph->m_width / tex_width);

		Vertex3D_PCT vert;
		vert.m_position = bottomLeft;
		vert.m_texCoords = uv_bottomLeft;
		vert.m_color = color;
		verts.push_back(vert);

		vert.m_position = bottomRight;
		vert.m_texCoords = uv_bottomRight;
		vert.m_color = color;
		verts.push_back(vert);

		vert.m_position = topRight;
		vert.m_texCoords = uv_topRight;
		vert.m_color = color;
		verts.push_back(vert);

		vert.m_position = topLeft;
		vert.m_texCoords = uv_topLeft;
		vert.m_color = color;
		verts.push_back(vert);




		

		cursor += thisGlyph->m_xAdvance * scale * right;
		prev_glyph = thisGlyph;

	}

	BindTexture(*font->GetTexture());
	DrawVertexArray_PCT(&verts[0], vertLength, PRIMITIVE_QUADS);
}

void Renderer::PushMatrix()
{
	glPushMatrix();
}

void Renderer::PopMatrix()
{
	glPopMatrix();
}

void Renderer::DrawGrid(int xCells, int yCells)
{
	Vector2 v1 = Vector2(0, 0);
	Vector2 v2 = Vector2(0, 0);
	for (int i = 0; i < xCells; i++)
	{
		v1.x = (v1.x + 1);
		v2.x = v1.x;
		v2.y = WINDOW_PHYSICAL_HEIGHT;
		DrawLine(v1,v2,1.f,1.f,1.f,1.f,2.f);
	}

	v1 = Vector2(0, 0);
	v2 = Vector2(0, 0);

	for (int j = 0; j < yCells; j++)
	{
		v1.y = (v1.y + 1);
		v2.y = v1.y;
		v2.x = WINDOW_PHYSICAL_WIDTH;
		DrawLine(v1, v2, 1.f, 1.f, 1.f, 1.f, 2.f);
	}
}

void Renderer::ScaleView(const Vector2& scale)
{
	glScalef(scale.x, scale.y, 0.f);
}

void Renderer::SetInverseDestBlend()
{
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
}
void Renderer::SetAlphaBlend()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::SetAdditiveBlend()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}

void Renderer::SetPointSize(float size)
{
	glPointSize(size);
}

void Renderer::SetPerspective(float fovDegreesY, float aspect, float nearDist, float farDist)
{
	glLoadIdentity();
	gluPerspective(fovDegreesY, aspect, nearDist, farDist);
}

void Renderer::ClearScreenDepth(float red, float green, float blue)
{
	glClearColor(red, green, blue, 1.f);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::TranslateView(float x, float y, float z)
{
	glTranslatef(x, y, z);
}

void Renderer::TranslateView(const Vector3& translation)
{
	glTranslatef(translation.x, translation.y, translation.z);
}

// void Renderer::DrawCubeFace(CubeDirection face, const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint)
// {
// 	glEnable(GL_CULL_FACE);
// 	glEnable(GL_DEPTH_TEST);
// 	glEnable(GL_TEXTURE_2D);
// 	glColor4f(1.f, 1.f, 1.f, 1.f);
// 	//glColor4f(tint.m_red, tint.m_green, tint.m_blue, tint.m_alpha);
// 	glBindTexture(GL_TEXTURE_2D, texture.m_openglTextureID);
// 	/*
// 	// 		11
// 	// 		10
// 	// 		00
// 	// 		01
// 	// 		*/
// 	glBegin(GL_QUADS);
// 	{
// 		switch (face)
// 		{
// 		case FRONT:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(1.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(0.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 0.f);
// 			break;
// 		}
// 		case BACK:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 1.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(0.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(1.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 1.f, 0.f);
// 			break;
// 		}
// 		case LEFT:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(0.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(0.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 1.f, 0.f);
// 			break;
// 		}
// 		case RIGHT:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 1.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(1.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(1.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 0.f);
// 			break;
// 		}
// 		case TOP:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(1.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(0.f, 1.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 1.f);
// 			break;
// 		}
// 		case BOTTOM:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(0.f, 1.f, 0.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(1.f, 1.f, 0.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 0.f);
// 			break;
// 		}
// 		default:
// 		{
// 			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
// 			glVertex3f(1.f, 0.f, 0.f);
// 			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
// 			glVertex3f(1.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMins.y);
// 			glVertex3f(0.f, 0.f, 1.f);
// 			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
// 			glVertex3f(0.f, 0.f, 0.f);
// 			break;
// 		};
// 		}
// 	}
// 	glEnd();
// 	glDisable(GL_TEXTURE_2D);
// // 	glDisable(GL_CULL_FACE);
// // 	glDisable(GL_DEPTH_TEST);
// }

void Renderer::DrawTexturedQuad3D(const Texture& tex, const Vector3& vertex1, const Vector3& vertex2, const Vector3& vertex3, const Vector3& vertex4, 
	const AABB2& texCoords, const bool mirrored, const Rgba& tint /*= Rgba::WHITE*/ )
{
	const int numVerts = 4;
	Vertex3D_PCT verts[numVerts];
	Vector2 texCoordMaxs = Vector2(texCoords.m_maxs);
	Vector2 texCoordMins = Vector2(texCoords.m_mins);
	glColor4f(tint.m_red, tint.m_green, tint.m_blue, tint.m_alpha);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex.m_openglTextureID);
	if (mirrored)
	{
		verts[0].m_position = Vector3(vertex1.x, vertex1.y, vertex1.z);
		verts[0].m_color = tint;
		verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);

		verts[1].m_position = Vector3(vertex2.x, vertex2.y, vertex2.z);
		verts[1].m_color = tint;
		verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);

		verts[2].m_position = Vector3(vertex3.x, vertex3.y, vertex3.z);
		verts[2].m_color = tint;
		verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);

		verts[3].m_position = Vector3(vertex4.x, vertex4.y, vertex4.z);
		verts[3].m_color = tint;
		verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);
		/*
		glBegin(GL_QUADS);
		{
			glTexCoord2f(texCoordMins.x, texCoordMins.y);
			glVertex3f(vertex1.x, vertex1.y, vertex1.z);
			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
			glVertex3f(vertex2.x, vertex2.y, vertex2.z);
			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
			glVertex3f(vertex3.x, vertex3.y, vertex3.z);
			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
			glVertex3f(vertex4.x, vertex4.y, vertex4.z);

		}
		glEnd();
		*/
	}
	else
	{

		verts[0].m_position = Vector3(vertex1.x, vertex1.y, vertex1.z);
		verts[0].m_color = tint;
		verts[0].m_texCoords = Vector2(texCoordMins.x, texCoordMaxs.y);

		verts[1].m_position = Vector3(vertex2.x, vertex2.y, vertex2.z);
		verts[1].m_color = tint;
		verts[1].m_texCoords = Vector2(texCoordMaxs.x, texCoordMaxs.y);

		verts[2].m_position = Vector3(vertex3.x, vertex3.y, vertex3.z);
		verts[2].m_color = tint;
		verts[2].m_texCoords = Vector2(texCoordMaxs.x, texCoordMins.y);

		verts[3].m_position = Vector3(vertex4.x, vertex4.y, vertex4.z);
		verts[3].m_color = tint;
		verts[3].m_texCoords = Vector2(texCoordMins.x, texCoordMins.y);
		/*
		glBegin(GL_QUADS);
		{
			glTexCoord2f(texCoordMins.x, texCoordMaxs.y);
			glVertex3f(vertex1.x, vertex1.y, vertex1.z);
			glTexCoord2f(texCoordMaxs.x, texCoordMaxs.y);
			glVertex3f(vertex2.x, vertex2.y, vertex2.z);
			glTexCoord2f(texCoordMaxs.x, texCoordMins.y);
			glVertex3f(vertex3.x, vertex3.y, vertex3.z);
			glTexCoord2f(texCoordMins.x, texCoordMins.y);
			glVertex3f(vertex4.x, vertex4.y, vertex4.z);

		}
		glEnd();
		*/
	}
	
	DrawVertexArray_PCT(verts,numVerts,PRIMITIVE_QUADS);
	glDisable(GL_TEXTURE_2D);
}


void Renderer::DrawSphere(const Vector3& pos, const float radius, const Rgba& color /*= Rgba::WHITE*/)
{
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);

	int numSides = 25; // # of triangles used to draw polygon

	float twicePi = 2.0f * pi;

	Vertex3D_PCT verts[ 25 ];

	for ( int i = 0; i < numSides; i++ )
	{
		verts[ i ].m_position = Vector3( pos.x + ( radius * cos( i *  twicePi / numSides ) ), pos.y + ( radius* sin( i * twicePi / numSides ) ), pos.z );
		verts[ i ].m_color = color;
		verts[ i ].m_texCoords = Vector2( 0.0f, 0.0f );
	}

	DrawVertexArray_PCT( verts, 25, PRIMITIVE_LINES_LOOP );

	for ( int i = 0; i < numSides; i++ )
	{
		verts[ i ].m_position = Vector3( pos.x + ( radius * cos( i *  twicePi / numSides ) ), pos.y, pos.z + ( radius* sin( i * twicePi / numSides ) ) );
		verts[ i ].m_color = color;
		verts[ i ].m_texCoords = Vector2( 0.0f, 0.0f );
	}

	DrawVertexArray_PCT( verts, 25, PRIMITIVE_LINES_LOOP );

	for ( int i = 0; i < numSides; i++ )
	{
		verts[ i ].m_position = Vector3( pos.x, pos.y + ( radius * cos( i *  twicePi / numSides ) ), pos.z + ( radius* sin( i * twicePi / numSides ) ) );
		verts[ i ].m_color = color;
		verts[ i ].m_texCoords = Vector2( 0.0f, 0.0f );
	}

	DrawVertexArray_PCT( verts, 25, PRIMITIVE_LINES_LOOP );
}

void Renderer::BindTexture(const Texture& texture)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture.m_openglTextureID);
}


void Renderer::DrawVertexArray(const std::vector< Vertex3D_PCT >& vertexes)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D_PCT), &vertexes[0].m_position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D_PCT), &vertexes[0].m_color); // float Rgbas
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D_PCT), &vertexes[0].m_texCoords);

	glDrawArrays(GL_QUADS, 0, vertexes.size());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Renderer::DrawVertexArrayPoints(const std::vector< Vertex3D_PC >& vertexes)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D_PC), &vertexes[0].m_position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D_PC), &vertexes[0].m_color); // float Rgbas


	glDrawArrays(GL_POINT, 0, vertexes.size());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

}

void Renderer::DrawVertexArray_PCT(const Vertex3D_PCT* verts, int numVerts, PrimitiveType primitiveType = PRIMITIVE_POINTS)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D_PCT), &verts[0].m_position);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D_PCT), &verts[0].m_color); // float Rgbas
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D_PCT), &verts[0].m_texCoords);

	switch (primitiveType)
	{
	case PRIMITIVE_POINTS:
	{
		glDrawArrays(GL_POINT, 0, numVerts);
		break;
	}
	case PRIMITIVE_LINES:
	{
		glDrawArrays(GL_LINES, 0, numVerts);
		break;
	}
	case PRIMITIVE_LINES_LOOP:
	{
		glDrawArrays(GL_LINE_LOOP, 0, numVerts);
		break;
	}
	case PRIMITIVE_TRIANGLES:
	{
		glDrawArrays(GL_TRIANGLES, 0, numVerts);
		break;
	}
	case PRIMITIVE_QUADS:
	{
		glDrawArrays(GL_QUADS, 0, numVerts);
		break;
	}
	default:
	{
		glDrawArrays(GL_POINT, 0, numVerts);
		break;
	}
	}
	//glDrawArrays(GL_POINT, 0, numVerts);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Renderer::DrawAABB3Wireframe(AABB3 bounds, Rgba color)
{
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glLineWidth(2.f);
	SetColor(color);
	BindTexture(*g_DefaultImage.m_spriteSheetTexture);

	const int numVerts = 24;

	Vertex3D_PCT verts[numVerts];
	Vector2 texCoords = Vector2(0.f, 1.f);

	for (int i = 0; i < numVerts; i++)
	{
		verts[i].m_color = color;
		verts[i].m_texCoords = texCoords;
	}

	//Bottom Face, Counter Clockwise
	verts[0].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
	verts[1].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);


	verts[2].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
	verts[3].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);


	verts[4].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
	verts[5].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);


	verts[6].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
	verts[7].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);

	//Z-Aligned Lines
	verts[8].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
	verts[9].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);

	verts[10].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
	verts[11].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);

	verts[12].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
	verts[13].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);

	verts[14].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
	verts[15].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);


	//Top Face, Counter Clockwise
	verts[16].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
	verts[17].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);


	verts[18].m_position = Vector3(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
	verts[19].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);

	verts[20].m_position = Vector3(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);
	verts[21].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);

	verts[22].m_position = Vector3(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);
	verts[23].m_position = Vector3(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);

	DrawVertexArray_PCT(verts, numVerts, PRIMITIVE_LINES);

	/*
	glBegin(GL_LINES);
	{
		//Bottom Face, Counter Clockwise
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);

		
		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);

		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);

		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);

		//Z-Aligned Lines
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_mins.z);
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);

		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_mins.z);
		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);

		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_mins.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);

		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_mins.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);

		//Top Face, Counter Clockwise
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);


		glVertex3f(bounds.m_mins.x, bounds.m_maxs.y, bounds.m_maxs.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);
															  
		glVertex3f(bounds.m_maxs.x, bounds.m_maxs.y, bounds.m_maxs.z);
		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);
															  
		glVertex3f(bounds.m_maxs.x, bounds.m_mins.y, bounds.m_maxs.z);
		glVertex3f(bounds.m_mins.x, bounds.m_mins.y, bounds.m_maxs.z);
	}
	glEnd();
	*/
}



void Renderer::GenerateBuffer(int numberofBufferObjects, GLuint* vboID)
{
	glGenBuffers(numberofBufferObjects, vboID);
}

void Renderer::BindBuffer(int vboID)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
}

void Renderer::BufferData(GLsizeiptr size, const GLvoid *  data)
{
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Renderer::DeleteBuffers(int numberofBufferObjects, const GLuint* vboID)
{
	glDeleteBuffers(numberofBufferObjects, vboID);
}

void Renderer::DrawVBO_PCT(unsigned int vboID, int numVerts, PrimitiveType primitiveType)
{
	glPushMatrix();
	//glTranslatef(m_worldMins.x, m_worldMins.y, m_worldMins.z);
	glBindBuffer(GL_ARRAY_BUFFER,vboID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex3D_PCT), (const GLvoid*)offsetof(Vertex3D_PCT, m_position));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex3D_PCT), (const GLvoid*)offsetof(Vertex3D_PCT, m_color));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex3D_PCT), (const GLvoid*)offsetof(Vertex3D_PCT, m_texCoords));

	switch (primitiveType)
	{
	case PRIMITIVE_POINTS:
		break;
	case PRIMITIVE_LINES:
		break;
	case PRIMITIVE_LINES_LOOP:
		break;
	case PRIMITIVE_TRIANGLES:
		break;
	case PRIMITIVE_QUADS:
	{
		glDrawArrays(GL_QUADS, 0, numVerts);
		break;
	}
	case NUM_PRIMITIVE_TYPES:
		break;
	default:
	{
		glDrawArrays(GL_QUADS, 0, numVerts);
		break;
	}
	}
	glDrawArrays(GL_QUADS, 0, numVerts);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glPopMatrix();
}


void Renderer::SetAlphaTest(bool alphaState, float threshold)
{
	if (alphaState)
	{
		glAlphaFunc(GL_GREATER, threshold);
		glEnable(GL_ALPHA_TEST);
	}
	else
	{
		glDisable(GL_ALPHA_TEST);
	}
}

void Renderer::HandleRenderCommands()
{
	//TODO draw command based on type

	for (auto commandIter = g_Commands.begin(); commandIter != g_Commands.end(); ++commandIter)
	{
		RenderCommand* command = *commandIter;
		

		RenderCommand::DrawMode mode = command->m_drawMode;

		switch (mode)
		{
		case RenderCommand::DEPTH_TEST_OFF:
		{
			SetDepthTest(false);
			DrawRenderCommands(command, 1.f);
			break;
		}
		case RenderCommand::DEPTH_TEST_ON:
		{
			SetDepthTest(true);
			DrawRenderCommands(command, 1.f);
			break;
		}
		case RenderCommand::XRAY:
		{
			SetDepthTest(false);
			DrawRenderCommands(command, 1.f);
			SetDepthTest(true);
			DrawRenderCommands(command, 3.f);
			break;
		}
		default:
			break;
		}

		
		
	}
}

void Renderer::DrawRenderCommands(const RenderCommand* command, float lineThickness)
{
	RenderCommand::CommandType type = command->m_type;
	switch (type)
	{
		case RenderCommand::POINT:
		{
			DrawPoint(command->m_startPostion, command->m_color);
			break;
		}
		case RenderCommand::LINE:
		{
			DrawLine(command->m_startPostion, command->m_endPosition, command->m_color, lineThickness);
			break;
		}
		case RenderCommand::ARROW:
		{
			DrawLine(command->m_startPostion, command->m_endPosition, command->m_color, lineThickness);
			DrawPoint(command->m_endPosition, command->m_color);
			break;
		}
		case RenderCommand::AABB3:
		{
			AABB3 bounds;
			bounds.m_mins = command->m_mins;
			bounds.m_maxs = command->m_maxs;
			DrawAABB3Wireframe(bounds, command->m_color);
			break;
		}
		case RenderCommand::SPHERE:
		{
			DrawSphere(command->m_startPostion, command->m_radius, command->m_color);
			break;
		}
	}
}

void Renderer::UpdateRenderCommands(const float deltaSeconds)
{
	std::vector<RenderCommand*>::iterator it = g_Commands.begin();

	while (it != g_Commands.end()) {
		if ((*it)->m_seconds < 0.f) //Run forever if negative value
		{
			++it;
		}
		else
		{
			(*it)->m_seconds -= deltaSeconds;
			if ((*it)->m_seconds <= 0.f)
			{
				delete(*it);
				it = g_Commands.erase(it);
			}
			else
			{
				++it;
			}
		}
		
	}
}

void Renderer::AddDebugPoint(const Vector3& pos, const Rgba& color, const float seconds, const RenderCommand::DrawMode& drawMode)
{
	//TODO Create instance of Point RenderCommand and insert it into the vector
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::POINT;
	command->m_startPostion = pos;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}


void Renderer::AddDebugLine(const Vector3& startPos, const Vector3& endPos, const Rgba& color, const float& seconds, const RenderCommand::DrawMode& drawMode)
{
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::LINE;
	command->m_startPostion = startPos;
	command->m_endPosition = endPos;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}


void Renderer::AddDebugArrow(Vector3 startPos, Vector3 endPos, Rgba color, const float seconds, RenderCommand::DrawMode drawMode)
{
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::ARROW;
	command->m_startPostion = startPos;
	command->m_endPosition = endPos;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}

void Renderer::AddDebugAABB3(Vector3 mins, Vector3 maxs, Rgba color, const float seconds, RenderCommand::DrawMode drawMode)
{
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::AABB3;
	command->m_mins = mins;
	command->m_maxs = maxs;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}

void Renderer::AddDebugSphere(Vector3 pos, float radius, Rgba color, const float seconds, RenderCommand::DrawMode drawMode)
{
	RenderCommand* command = new RenderCommand();
	command->m_type = RenderCommand::SPHERE;
	command->m_startPostion = pos;
	command->m_radius = radius;
	command->m_color = color;
	command->m_seconds = seconds;
	command->m_drawMode = drawMode;
	g_Commands.push_back(command);
}

GLuint Renderer::LoadShader(char const *filename, GLenum shader_type)
{
	std::vector<unsigned char> buffer;
	LoadBufferFromBinaryFile(buffer, filename);
	buffer.push_back('\0');

	GLuint shader_id = glCreateShader(shader_type);
	ASSERT_OR_DIE(shader_id != NULL, "You dun goofed.");

	GLint src_length = buffer.size();
	GLchar* bufferStart = (GLchar*)&buffer[0];
	glShaderSource(shader_id, 1, &bufferStart, &src_length);

	glCompileShader(shader_id);

	GLint status;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	if (GL_FALSE == status)
	{
		GLint length;
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);

		char *errorBuffer = new char[length + 1];
		glGetShaderInfoLog(shader_id, length, &length, errorBuffer);

		errorBuffer[length] = NULL;
		std::string errorBufferStr(errorBuffer);

		std::stringstream errorStream(errorBufferStr);
		std::vector<std::string> errorLines;

		std::string line;
		while (std::getline(errorStream, line))
		{
			errorLines.push_back(line);
		}

		for (auto lineIter = errorLines.begin(); lineIter != errorLines.end(); ++lineIter)
		{
			if (lineIter->find("error") != std::string::npos)
			{
				//Error Found
				std::string prunedStr = "";
				std::string lineNumber = ExtractToken(*lineIter, "(", ")");
				lineNumber = std::to_string(std::stoi(lineNumber) - 1);
				std::string filePathStr(filename);

				std::string errorText = ExtractToken(*lineIter, ":", "");
				errorText = ExtractToken(errorText, ":", "");
				prunedStr += errorText;
				prunedStr += "\n";
				char temp[_MAX_PATH];
				std::string workingDirStr = _getcwd(temp, _MAX_PATH);
				filePathStr = workingDirStr + "\\" + filePathStr;
				DebuggerPrintf("%s(%d): %s", filePathStr.c_str(), std::stoi(lineNumber),
					prunedStr.c_str()); // Use this specific format so Visual Studio users can double-click to jump to file-and-line of error
										//DebuggerPrintf(prunedStr.c_str());
				const char* versionString = (const char*)glGetString(GL_VERSION);
				std::string oglVer = Stringf("OpenGL version: %s\n", versionString);
				const char* glslString = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
				std::string glslVer = Stringf("GLSL version: %s\n", glslString);
				ASSERT_OR_DIE(false, prunedStr + "\n In file: " + filePathStr + "\n\n" + errorBufferStr + "\n" + oglVer + "\n" + glslVer);
			}
			else
			{
				//Warnings
			}

		}

		//__FILE__, prune up to Code, append shader path
		
		delete errorBuffer;

		glDeleteShader(shader_id);
		return 0;
	}
	//ASSERT_OR_DIE(status == GL_TRUE, "You dun goofed.");

	return shader_id;
	
}

int Renderer::GenerateVboID()
{
	GLuint vboID;
	GenerateBuffer(1, &vboID);
	return (int)vboID;
}

GLuint Renderer::RenderBufferCreate(void *data, size_t count, size_t elem_size, GLenum usage)
{
	GLuint  buffer;
	glGenBuffers(1, &buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, count * elem_size, data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	return buffer;
}
/*
GLuint Renderer::UpdateVBO(void *data, size_t elem_size, int vboID)
{
	GLuint buffer = (GLuint)vboID;

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, count * elem_size, data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	return buffer;
}
*/
void Renderer::RenderBufferDestroy(GLuint buffer)
{
	glDeleteBuffers(1, &buffer);
}

GLuint Renderer::CreateAndLinkProgram(std::string vsStr, std::string fsStr)
{
	GLuint program_id = glCreateProgram();

	GLuint vs = LoadShader(vsStr.c_str(), GL_VERTEX_SHADER);
	GLuint fs = LoadShader(fsStr.c_str(), GL_FRAGMENT_SHADER);
	glAttachShader(program_id, vs);
	glAttachShader(program_id, fs);

	glLinkProgram(program_id);

	GLint status;
	glGetProgramiv(program_id, GL_LINK_STATUS, &status);
	if (GL_FALSE == status)
	{
		GLint length;
		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &length);

		char *errorBuffer = new char[length + 1];
		glGetProgramInfoLog(program_id, length, &length, errorBuffer);

		errorBuffer[length] = NULL;
		std::string errorBufferStr(errorBuffer);

		std::stringstream errorStream(errorBufferStr);
		std::vector<std::string> errorLines;

		std::string line;
		while (std::getline(errorStream, line))
		{
			errorLines.push_back(line);
		}

		for (auto lineIter = errorLines.begin(); lineIter != errorLines.end(); ++lineIter)
		{
			if (lineIter->find("error") != std::string::npos)
			{
				//Error Found
				std::string prunedStr = "";
				std::string lineNumber = "0";
				std::string filePathStr(fsStr);

				std::string errorText = ExtractToken(*lineIter, ":", "");
				errorText = ExtractToken(errorText, ":", "");
				prunedStr += errorText;
				prunedStr += "\n";
				char temp[_MAX_PATH];
				std::string workingDirStr = _getcwd(temp, _MAX_PATH);
				filePathStr = workingDirStr + "\\" + filePathStr;
				DebuggerPrintf("%s(%d): %s", filePathStr.c_str(), std::stoi(lineNumber),
					prunedStr.c_str()); // Use this specific format so Visual Studio users can double-click to jump to file-and-line of error
										//DebuggerPrintf(prunedStr.c_str());
				const char* versionString = (const char*)glGetString(GL_VERSION);
				std::string oglVer = Stringf("OpenGL version: %s\n", versionString);
				const char* glslString = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
				std::string glslVer = Stringf("GLSL version: %s\n", glslString);
				ASSERT_OR_DIE(false, prunedStr + "\n In file: " + filePathStr + "\n\n" + errorBufferStr + "\n" + oglVer + "\n" + glslVer);
			}
			else
			{
				//Warnings
			}

		}

		//__FILE__, prune up to Code, append shader path

		delete errorBuffer;

		//glDeleteShader(shader_id);
		return 0;
	}
	else
	{
		glDetachShader(program_id, vs);
		glDetachShader(program_id, fs);
	}

	return program_id;
}

void Renderer::BindMeshToVAO(GLuint vao, GLuint vbo, GLuint ibo, GLuint program)
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	ShaderProgramBindProperty(program, "inPosition",
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex3D_PCT), offsetof(Vertex3D_PCT, m_position));

	ShaderProgramBindProperty(program, "inColor",
		4,
		GL_UNSIGNED_BYTE,
		GL_TRUE,
		sizeof(Vertex3D_PCT), offsetof(Vertex3D_PCT, m_color));
	ShaderProgramBindProperty(program, "inUV0",
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(Vertex3D_PCT), offsetof(Vertex3D_PCT, m_texCoords));

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	if (NULL != ibo) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	}

	glBindVertexArray(NULL);
}

void Renderer::RenderStep(const float deltaSeconds)
{
	static float m_age = 0.f;
	m_age += deltaSeconds;

	g_theRenderer->SetCullFace(true);
	g_theRenderer->SetDepthTest(true);

	//Vector3 lightPos = Vector3(cos(m_age) *2.f, sin(m_age), 0.f);
	Vector3 lightPos = Vector3(cos(m_age) *2.f, sin(m_age), cos(m_age) * sin(m_age));
	//Vector3 lightPos = Vector3(0.f,-1.f,0.f);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	

	mat44_fl model;
	mat44_fl view;
	mat44_fl proj;

	MatrixMakeIdentity(&model);
	MatrixMakeIdentity(&view);
	MatrixMakeIdentity(&proj);


	float aspect = 16.f/9.f;

	//PROJECTION MATRIX
	//MatrixMakeProjOrthogonal(&proj, 50.0f * aspect, 50.0f, 0.0f, 100.0f);
	MatrixMakePerspective(&proj, 70.f, aspect, 0.1f, 100.f);
	
	
	//MODEL MATRIX
	Vector3 trans = Vector3(0.f, 2.f, 0.f);
	MatrixTranslate(&model, trans);
	Vector3 transLight = lightPos + trans;
	//g_theRenderer->DrawSphere(transLight, 0.2f);
	
	

	//VIEW MATRIX
	//MatrixMakeBasis(&view, Vector3(1.f,0.f,0.f), Vector3(0.f,0.f,1.f), Vector3(0.f,1.f,0.f));
	MatrixMakeRotationEuler(&view, g_camera.m_orientation.m_yawDegreesAboutZ, g_camera.m_orientation.m_pitchDegreesAboutX, 
		g_camera.m_orientation.m_rollDegreesAboutY, g_camera.m_position);
	MatrixForsethCOBNick(&view);

	MatrixInvertOrthonormal(&view);
	

	ShaderProgramSetMat4(gShaderProgram, "gModel", model);
	ShaderProgramSetMat4(gShaderProgram, "gView", view);
	ShaderProgramSetMat4(gShaderProgram, "gProj", proj);
	ShaderProgramSetMat4(gShaderTimedProgram, "gModel", model);
	ShaderProgramSetMat4(gShaderTimedProgram, "gView", view);
	ShaderProgramSetMat4(gShaderTimedProgram, "gProj", proj);


	GLint	diffuse_bind = 0;
	GLint	normal_bind = 1;
	GLint	spec_bind = 2;

	glActiveTexture(GL_TEXTURE0 + diffuse_bind);
	glBindTexture(GL_TEXTURE_2D, gDiffuseTex);
	glBindSampler(diffuse_bind, gSamplerId);
	ShaderProgramSetInt(gShaderProgram, "gDiffuseTex", diffuse_bind);
	

	glActiveTexture(GL_TEXTURE0 + normal_bind);
	glBindTexture(GL_TEXTURE_2D, gNormalTex);
	glBindSampler(normal_bind, gSamplerId);
	ShaderProgramSetInt(gShaderProgram, "gNormalTex", normal_bind);

	glActiveTexture(GL_TEXTURE0 + spec_bind);
	glBindTexture(GL_TEXTURE_2D, gSpecularTex);
	glBindSampler(spec_bind, gSamplerId);
	ShaderProgramSetInt(gShaderProgram, "gSpecularTex", spec_bind);
	
	
	unsigned int tex_index = 0;/*
	glActiveTexture(GL_TEXTURE0 + tex_index);
	glBindTexture(GL_TEXTURE_2D, gDiffuseTex);
	glBindSampler(tex_index, gSamplerId);
	*/

	
	

	ShaderProgramSetVec3(gShaderProgram, "gLightPosition", transLight);
	ShaderProgramSetFloat(gShaderProgram, "gLightIntensity", 0.75f);
	ShaderProgramSetVec4(gShaderProgram, "gAmbientLight", vec4_fl(0.f, 0.f, 1.f, 0.5f));
	ShaderProgramSetVec4(gShaderProgram, "gLightColor", vec4_fl(0.f, 1.f, 0.f, 1.f));
	ShaderProgramSetFloat(gShaderProgram, "gSpecularPower", 100.f);
	ShaderProgramSetVec3(gShaderProgram, "g_cameraPosition", g_camera.m_position);




//	ShaderProgramSetInt(gShaderProgram, "gDiffuseTex",tex_index);
	ShaderProgramSetInt(gShaderTimedProgram, "gEffectState", g_effectState);
	ShaderProgramSetInt(gShaderTimedProgram, "gDiffuseTex", tex_index);
	ShaderProgramSetFloat(gShaderTimedProgram, "gEffectTime", m_age);

// 	glBindVertexArray(gVAO);
// 	glUseProgram(gShaderProgram);

	//glBindVertexArray(m_cubeVAO);
	//glUseProgram(gShaderProgram);
	//glDrawElements(GL_TRIANGLES, m_cube->m_indices.size(), GL_UNSIGNED_SHORT, (GLvoid*)0);

	glUseProgram(NULL);
	glBindVertexArray(NULL);

	//glBindVertexArray(m_sphereVAO);
	//glUseProgram(gShaderTimedProgram);
	//glDrawElements(GL_TRIANGLES, m_sphere->m_indices.size(), GL_UNSIGNED_SHORT, (GLvoid*)0);

	glUseProgram(NULL);
	glBindVertexArray(NULL);

	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (GLvoid*)0);
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	glUseProgram(NULL);
	glBindVertexArray(NULL);
	glActiveTexture(GL_TEXTURE0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	//g_theRenderer->DrawSphere(lightPos, 0.2f);
	//g_theRenderer->DrawSphere(lightPos, 0.2f);
}

void Renderer::RenderCleanup()
{
	glDeleteProgram(gShaderProgram);
	glDeleteVertexArrays(1, &gVAO);

	glDeleteBuffers(1, &gVBO);
	//DeleteVertexBuffer(gVBO);
}

void Renderer::ShaderProgramBindProperty(GLuint program, const char *name, GLint count, GLenum type, GLboolean normalize, GLsizei stride, GLsizei offset)
{
	GLint pos_bind = glGetAttribLocation(program, name);
	if (pos_bind >= 0)
	{
		glEnableVertexAttribArray(pos_bind);
		glVertexAttribPointer(pos_bind, count, type, normalize, stride, (GLvoid*) offset);
	}
}

bool Renderer::ShaderProgramSetVec4(GLuint shader, const char *name, const vec4_fl &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniform4fv(loc, 1, (GLfloat*)&v);
		return true;
	}
	return false;
}

void Renderer::MatrixMakeIdentity(mat44_fl *mat)
{
	float data[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	memcpy(mat->data, data, sizeof(data));
}

bool Renderer::ShaderProgramSetMat4(GLuint shader, const char *name, const mat44_fl &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, (GLfloat*)&v);
		return false;
	}

	return false;
}

//------------------------------------------------------------------------
//
void Renderer::MatrixMakeProjOrthogonal(mat44_fl *mat, float nx, float fx, float ny, float fy, float nz, float fz)
{
	float sx = 1.0f / (fx - nx);
	float sy = 1.0f / (fy - ny);
	float sz = 1.0f / (fz - nz);

	float const values[] = {
		2.0f * sx,  0.0f,       0.0f,       -(fx + nx) * sx,
		0.0f,       2.0f * sy,  0.0f,       -(fy + ny) * sy,
		0.0f,       0.0f,       2.0f * sz,  0.0f,
		0.0f,       0.0f,      -(fz + nz) * sz,       1.0f,
	};

	memcpy(mat->data, values, sizeof(values));
}

void Renderer::MatrixMakeProjOrthogonal(mat44_fl *mat, float width, float height, float nz, float fz)
{
	float sz = 1.0f / (fz - nz);

	float const values[] = {
		2.0f / width,  0.0f,       0.0f,        0,
		0.0f,       2.0f / height,  0.0f,        0,
		0.0f,       0.0f,       2.0f * sz,  0.0f,
		0.0f,       0.0f,       -(fz + nz) * sz,       1.0f,
	};

	memcpy(mat->data, values, sizeof(values));
}

bool Renderer::ShaderProgramSetInt(GLuint shader, const char *name, const int &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniform1iv(loc, 1, (GLint*)&v);
		return false;
	}

	return false;
}

GLuint Renderer::CreateSampler(GLenum min_filter, GLenum mag_filter, GLenum u_wrap, GLenum v_wrap)
{
	GLuint id;
	glGenSamplers(1, &id);

	glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, min_filter);
	glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, mag_filter);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_S, u_wrap);
	glSamplerParameteri(id, GL_TEXTURE_WRAP_T, v_wrap);

	return id;
}


void Renderer::MatrixMakePerspective(mat44_fl* mat, float fov_degrees, float aspect, float nz, float fz)
{
	float rads = DegToRad(fov_degrees);
	//float size = atan(rads / 2.f);
	float size = 1.0f / tan(rads / 2.f);

	float w = size;
	float h = size;
	if (aspect > 1.f)
	{
		w /= aspect;
	}
	else {
		h *= aspect;
	}

	float q = 1.f / (fz - nz);

	//ROW MAJOR
	float const values[] = {
		w, 0.f, 0.0f, 0.0f,
		0.0f, h, 0.0f, 0.0f,
		0.0f, 0.0f, (fz + nz) * q, 1.f,
		0.0f, 0.0f, -2.f * nz * fz * q, 0.0f,
	};


	memcpy(mat->data, values, sizeof(values));
}

void Renderer::MatrixTranslate(mat44_fl* mat, Vector3 translation)
{
	mat->data[12] = translation.x;
	mat->data[13] = translation.y;
	mat->data[14] = translation.z;

	//memcpy(mat->data, values, sizeof(values));
}

void Renderer::MatrixInvertOrthonormal(mat44_fl *mat)
{
	//implement
	mat44_fl matRotation = *mat;

	//Clearing the sides
	matRotation.data[3] = 0.f;
	matRotation.data[7] = 0.f;
	matRotation.data[11] = 0.f;
	
	matRotation.data[12] = 0.f;
	matRotation.data[13] = 0.f;
	matRotation.data[14] = 0.f;
	matRotation.data[15] = 1.f;

	//Invert Rotation

	MatrixTransposeRotation(&matRotation);

	//Get Translation and Invert
	Vector3 translation = MatrixGetOffset(mat);
	translation = -translation;
	mat44_fl matTrans = mat44_fl::identity;
	MatrixSetOffset(&matTrans, translation);

	//(R * T)^-1 = T^-1 * R^1
	MatrixMultiply(mat, &matTrans, &matRotation);
	//*mat = matTrans * matRotation;

// 	std::swap(mat->data[1], mat->data[4]);
// 	std::swap(mat->data[2], mat->data[8]);
// 	std::swap(mat->data[6], mat->data[9]);
// 
// 	
// 	mat->data[12] = -mat->data[12];
// 	mat->data[13] = -mat->data[13];
// 	mat->data[14] = -mat->data[14];
}

void Renderer::MatrixMakeRotationEuler(mat44_fl *mat, float const yaw, float const pitch, float const roll, Vector3 const &o)
{
	// yaw = y
	// pitch = x
	// roll = z

	float sx = sin(DegToRad(pitch));
	float cx = cos(DegToRad(pitch));

	float sy = sin(DegToRad(yaw));
	float cy = cos(DegToRad(yaw));

	float sz = sin(DegToRad(roll));
	float cz = cos(DegToRad(roll));

	/*
	float const values[] = {
		cy*cz + sx*sy*sz,	cy*sz - cz*sx*sy, cx*sy, o.x,
		-cx*sz,				cx*cz,			  sx,	 o.y,
		-cz*sy + cy*sx*sz, -sy*sz - cy*cz*sx, cx*cy, o.z,
		0.f,0.f, 0.f, 1.f
	};*/

	float const values[] = {
		cy*cz + sx*sy*sz,	-cx*sz,	-cz*sy + cy*sx*sz , 0.f,
		cy*sz - cz*sx*sy,	cx*cz,	-sy*sz - cy*cz*sx,	0.f,
		cx*sy,				sx ,	cx*cy,				0.f,
		o.x,				o.z,	o.y,				1.f
	};

	/*
	float const values[] = {
	cy*cz,	cz * sx * sy - cx * sz ,cx * cz * sy + sx * sz , 0.f,
	cy * sz ,	cx * cz + sx * sy * sz,	-cz * sx + cx * sy * sz,	0.f,
	-sy,				cy * sx ,	cx * cy,				0.f,
	o.x,				o.z,	o.y,				1.f
	};
	*/
	memcpy(mat->data, values, sizeof(values));
}

void Renderer::MatrixMakeBasis(mat44_fl* mat, const Vector3& right, const Vector3& up, const Vector3& forward)
{
	mat->data[0] = right.x;
	mat->data[1] = right.y;
	mat->data[2] = right.z;

	mat->data[4] = up.x;
	mat->data[5] = up.y;
	mat->data[6] = up.z;

	mat->data[8] = forward.x;
	mat->data[9] = forward.y;
	mat->data[10] = forward.z;
}

void Renderer::MatrixMakeLookAt(mat44_fl* mat, const Vector3& pos, const Vector3& up, const Vector3& target)
{
	Vector3 dir = target - pos;
	dir.SetNormalized();
	Vector3 normUp = up.GetNormalized();
	Vector3 right = CrossProduct(dir, normUp);
	right.SetNormalized();
	Vector3 lup = CrossProduct(right, dir);//local up


	mat->data[0] = right.x;
	mat->data[1] = right.y;
	mat->data[2] = right.z;

	mat->data[4] = lup.x;
	mat->data[5] = lup.y;
	mat->data[6] = lup.z;

	mat->data[8]  = -dir.x;
	mat->data[9]  = -dir.y;
	mat->data[10] = -dir.z;


// 	mat->data[4] = right.x;
// 	mat->data[5] = right.y;

// 	mat->data[12] = pos.x;
// 	mat->data[13] = pos.y;
// 	mat->data[14] = pos.z;
}

void Renderer::MatrixForsethCOBNick(mat44_fl* mat)
{
	//temp = y
	float yCol0 = mat->data[1];
	float yCol1 = mat->data[5];
	float yCol2 = mat->data[9];
	float yCol3 = mat->data[13];

	//y = z
	mat->data[1] = mat->data[2];
	mat->data[5] = mat->data[6];
	mat->data[9] = mat->data[10];
	mat->data[13] = mat->data[14];

	//z = temp
	mat->data[2] = yCol0;
	mat->data[6] = yCol1;
	mat->data[10] = yCol2;
	mat->data[14] = yCol3;
}

void Renderer::MatrixTransposeRotation(mat44_fl* mat)
{
	//temp = y
	float item0 = mat->data[1];
	float item1 = mat->data[2];
	float item2 = mat->data[6];

	//y = z
	mat->data[1] = mat->data[4];
	mat->data[2] = mat->data[8];
	mat->data[6] = mat->data[9];

	//z = temp
	mat->data[4] = item0;
	mat->data[8] = item1;
	mat->data[9] = item2;
}
bool Renderer::ShaderProgramSetFloat(GLuint shader, const char* name, const float &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniform1fv(loc, 1, (GLfloat*)&v);
		return false;
	}

	return false;
}


bool Renderer::ShaderProgramSetVec3(GLuint shader, const char* name, Vector3 &v)
{
	glUseProgram(shader);
	GLint loc = glGetUniformLocation(shader, name);
	if (loc >= 0)
	{
		glUniform3fv(loc, 1, (GLfloat*)&v);
		return true;
	}
	return false;
}
