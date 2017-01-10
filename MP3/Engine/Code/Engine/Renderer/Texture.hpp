#pragma  once
#include <map>
#include <string>
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#define STATIC // Do-nothing indicator that method/member is static in class definition


class Texture
{
public:
	Texture(const std::string& imageFilePath);

	int m_openglTextureID;
	IntVector2 m_texelSize;

	static std::map< std::string, Texture* >	Texture::s_textureRegistry;

	static Texture* GetTextureByName(const std::string& imageFilePath);
	static Texture* CreateOrGetTexture(const std::string& imageFilePath);
};