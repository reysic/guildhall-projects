#pragma once
#pragma warning( disable : 4201 )  // nonstandard extension used: nameless struct/union
#include "Engine/MathUtils.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Rgba.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/VertexPCT.hpp"
#include "Engine/Renderer/VertexPC.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Renderer/RenderCommand.hpp"
#include "Engine/Renderer/BitmapFontMeta.hpp"
#include <vector>
#include "Engine/Renderer/OpenGLExtensions.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Shaders/Mesh.hpp"


class Renderer;
extern Renderer* g_theRenderer;
extern std::vector<RenderCommand*> g_Commands;







/*
void Renderer::ClearScreen( float red, float green, float blue );
void Renderer::SetOrtho( const Vector2& bottomLeft, const Vector2& topRight );
void Renderer::DrawLine( const Vector2& start, const Vector2& end, float red, float green, float blue, float alpha, float lineThickness );
glBegin, glVertex, glOrtho
*/
struct vec4_fl
{
	union {
		float data[4];
		struct {
			float x, y, z, w;
		};
		struct {
			float r, g, b, a;
		};
	};

	vec4_fl(float v) : x(v), y(v), z(v), w(v) {}
	vec4_fl(float xv, float yv, float zv) : x(xv), y(yv), z(zv), w(1.f) {}
	vec4_fl(float xv, float yv, float zv, float wv):
	x(xv), y(yv), z(zv), w(wv) {}
};
/*
struct mat44_fl
{
	union {
		float data[16];
		vec4_fl column[4];
	};
	mat44_fl() {}
};
*/

enum PrimitiveType
{
	PRIMITIVE_POINTS = 0,
	PRIMITIVE_LINES,
	PRIMITIVE_LINES_LOOP,
	PRIMITIVE_TRIANGLES,
	PRIMITIVE_QUADS,
	NUM_PRIMITIVE_TYPES
};

class Renderer
{
public:
	SpriteSheet g_DefaultImage;

	mat44_fl m_view;
	mat44_fl m_model;
	mat44_fl m_projection;
	Mesh* m_cube;
	Mesh* m_sphere;
	GLuint m_cubeVAO;
	GLuint m_cubeVBO;
	GLuint m_cubeIBO;
	GLuint m_sphereVAO;
	GLuint m_sphereVBO;
	GLuint m_sphereIBO;

	Renderer::Renderer();
	void CreateScreen();
	void ClearScreen(float red, float green, float blue);
	void ClearScreenDepth(float red, float green, float blue);
	void SetOrtho(const Vector2& leftRight, const Vector2& bottomTop);
	void DrawLine(const Vector2& start, const Vector2& end, float red, float green, float blue, float alpha, float lineThickness);
	void DrawLoop(Vector2* vecArray[], int vertCount, float red, float green, float blue, float alpha, float lineThickness);
	void DrawLoop(AABB2 bounds, Rgba color, float lineThickness);
	void SetColor(float red, float green, float blue, float alpha);
	void SetColor(Rgba color);
	void RenderPolygon(float centerX, float centerY, float radius, float numSides, float degreesOffset);
	void DrawAABB(const AABB2& bounds, const Rgba& color);
	void DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint);
	void DrawTexturedAABB(const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, const Vector2& texCoordMaxs);
	void DrawTexturedAABB(const AABB2& bounds, const int& textureID, const Vector2& texCoordMins, const Vector2& texCoordMaxs, const Rgba& tint = Rgba::WHITE);
	void RotateView(const float& rotation);
	void TranslateView(const Vector2& position);
	void ScaleView(const Vector2& scale);
	void PushMatrix();
	void PopMatrix();
	void DrawGrid(int xCells, int yCells);
	void SetAlphaBlend();
	void SetAdditiveBlend();
	void SetPointSize(float size);

	//3D
	void SetPerspective(float fovDegreesY, float aspect, float nearDist, float farDist);
	void TranslateView(const Vector3& translation); //TODO make Vector3
	void TranslateView(float x, float y, float z);
	void DrawPoint(const Vector3& pos, const Rgba& tint);
	void DrawLine(const Vector3& start, const Vector3& end, float red, float green, float blue, float alpha, float lineThickness);
	void DrawLine(const Vector3& start, const Vector3& end, Rgba color, float lineThickness);
	void DrawLineLoop(const std::vector< Vertex3D_PCT >& vertexes, float lineThickness);
	void DrawTexturedQuad3D(const Texture& tex, const Vector3& vertex1, const Vector3& vertex2, const Vector3& vertex3, const Vector3& vertex4,
		const AABB2& texCoords, const bool mirrored = false, const Rgba& tint = Rgba::WHITE);
	void DrawSphere(const Vector3& pos, const float radius, const Rgba& color = Rgba::WHITE);
	void RotateView(float degrees, float x, float y, float z);
	void RotateView(float degrees, Vector3 axis);
	void SetCullFace(bool enable);
	void SetDepthTest(bool enable);
// 	void RotateViewMatrix();
// 	void ScaleViewMatrix();
// 	void ScaleViewMatrix();
	void DrawAxes();
	void DrawText2D(const Vector2& startBottomLeft, const std::string& asciiText,
		float cellHeight, const Rgba& tint = Rgba::WHITE, const BitmapFont* font = nullptr);
	void DrawTextMeta2D(const Vector3& position, float scale, const std::string& asciiText, const Rgba& color, 
		BitmapFontMeta* font = nullptr, const Vector3& up = Vector3(0.f,1.f,0.f), const Vector3& right = Vector3(1.f, 0.f, 0.f));
	void SetInverseDestBlend();
	// 	void Renderer::DrawCubeFace(CubeDirection face, const AABB2& bounds, const Texture& texture, const Vector2& texCoordMins, 
// 		const Vector2& texCoordMaxs, const Rgba& tint = Rgba::WHITE);
	void Renderer::BindTexture(const Texture& texture);
	void DrawVertexArray(const std::vector< Vertex3D_PCT >& vertexes);
	void DrawVertexArrayPoints(const std::vector< Vertex3D_PC >& vertexes);
	void DrawVertexArray_PCT(const Vertex3D_PCT* verts, int numVerts, PrimitiveType primitiveType);
	void DrawAABB3Wireframe(AABB3 bounds, Rgba color);
	void GenerateBuffer(int numberofBufferObjects, GLuint* vboID);
	void BindBuffer(int vboID);
	void BufferData(GLsizeiptr size, const GLvoid *  data);
	void DeleteBuffers(int numberofBufferObjects, const GLuint* vboID);
	void DrawVBO_PCT(unsigned int vboID, int numVerts, PrimitiveType primitiveType);
	void SetAlphaTest(bool alphaState, float threshold = 0.5f);

	//3D Debug Stuff
	void HandleRenderCommands();
	void DrawRenderCommands(const RenderCommand* command, float lineThickness);
	void UpdateRenderCommands(const float deltaSeconds);
	void AddDebugPoint(const Vector3& pos,  const Rgba& color, const float seconds, const RenderCommand::DrawMode& drawMode);
	void AddDebugLine(const Vector3& startPos, const Vector3& endPos, const Rgba& color,const float& seconds, const RenderCommand::DrawMode& drawMode);
	void AddDebugArrow(Vector3 startPos, Vector3 endPos, Rgba color, const float seconds, RenderCommand::DrawMode drawMode);
	void AddDebugAABB3(Vector3 mins, Vector3 maxs, Rgba color, const float seconds, RenderCommand::DrawMode drawMode);
	void AddDebugSphere(Vector3 pos, float radius, Rgba color, const float seconds, RenderCommand::DrawMode drawMode);

	//Shader stuff
	GLuint LoadShader(char const *filename, GLenum shader_type);
	int GenerateVboID();

	//void LogProgramError ()
	GLuint	RenderBufferCreate(void *data, size_t count, size_t elem_size, GLenum usage = GL_STATIC_DRAW);
	void RenderBufferDestroy(GLuint buffer);
	GLuint CreateAndLinkProgram(std::string vsStr, std::string fsStr);
	void BindMeshToVAO(GLuint gVAO, GLuint gVBO, GLuint ibo, GLuint gShaderProgram);
	void RenderStep(const float deltaSeconds);
	void RenderCleanup();
	void ShaderProgramBindProperty(GLuint program, const char *name, GLint count, GLenum type, GLboolean normalize, GLsizei stride, GLsizei offset);
	bool ShaderProgramSetVec4(GLuint shader, const char *name, const vec4_fl &v);
	void MatrixMakeIdentity(mat44_fl *mat);

	bool ShaderProgramSetMat4(GLuint shader, const char *name, const mat44_fl &v);
	void MatrixMakeProjOrthogonal(mat44_fl *mat, float nx, float fx, float ny, float fy, float nz, float fz);
	void MatrixMakeProjOrthogonal(mat44_fl *mat, float width, float height, float nz, float fz);
	bool ShaderProgramSetInt(GLuint shader, const char *name, const int &v);
	GLuint CreateSampler(GLenum min_filter, GLenum mag_filter, GLenum u_wrap, GLenum v_wrap);

	void MatrixMakePerspective(mat44_fl* mat, float fov_degrees, float aspect, float nz, float fz);
	void MatrixTranslate(mat44_fl* mat, Vector3 translation);
	void MatrixInvertOrthonormal(mat44_fl *mat);
	void MatrixMakeRotationEuler(mat44_fl *mat, float const yaw, float const pitch, float const roll, Vector3 const &o);
	void MatrixMakeBasis(mat44_fl* mat, const Vector3& right, const Vector3& up, const Vector3& forward);
	void MatrixMakeLookAt(mat44_fl* mat, const Vector3& pos, const Vector3& up, const Vector3& target);
	void MatrixForsethCOBNick(mat44_fl* view);
	void MatrixTransposeRotation(mat44_fl* matRotation);
	bool ShaderProgramSetFloat(GLuint shader, const char* name, const float &v);
	bool ShaderProgramSetVec3(GLuint shader, const char* name, Vector3 &v);
private:
};