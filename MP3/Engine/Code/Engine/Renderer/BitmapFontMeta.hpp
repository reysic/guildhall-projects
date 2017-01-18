#pragma once
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>
#include <vector>
/*
Purpose: Loads a font that comes with a metadata file generated from BMFont
*/


struct Glyph
{
	int m_charId;
	int m_xPos;
	int m_yPos;
	int m_width;
	int m_height;
	int m_xOffset;
	int m_yOffset;
	int m_xAdvance;

	//GetUV
	//vec2 offset
	//vec2 size
};

class BitmapFontMeta
{
public:
	static BitmapFontMeta* CreateOrGetFont(const std::string& BitmapFontMetaName);
//	AABB2 GetTexCoordsForGlyph(int glyphUnicode) const;

	Texture* BitmapFontMeta::GetTexture() const;
	Glyph* GetGlyph(const char c);
	const bool GetKerning(const int first, const int second, int& out_val) const;
	float CalcTextWidth(const std::string& text, const float scale);
	int m_textureHeight;
	int m_textureWidth;

	std::map<int, Glyph*> m_glyphRegistry;
	std::map<std::pair<int, int>, int> m_kerningRegistry;

private:
	BitmapFontMeta(const std::string& BitmapFontMetaName);
	bool ReadMetaFile(const std::string& metaFileName);

private:
	Texture* m_fontSheet;
	static std::map< std::string, BitmapFontMeta* > s_fontRegistry;
	

};

