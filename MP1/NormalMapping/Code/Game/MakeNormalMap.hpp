//-----------------------------------------------------------------------------------------------
// MakeNormalMap.hpp
// Takes a Vector3 light vector as input and computes lighting for a provided surface color map
// texture and normal displacement map texture


//-----------------------------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------------------------
const std::string SURFACE_COLOR_MAP_TEXTURE_FILE = "Data/Images/SurfaceColorMap.png";
const std::string NORMAL_DISPLACEMENT_MAP_TEXTURE_FILE = "Data/Images/NormalDisplacementMap.png";


//-----------------------------------------------------------------------------------------------
class MakeNormalMap
{
public:
	static int GetTextureIDOfNormalMappedTexture( const Vector3& lightVector);
};