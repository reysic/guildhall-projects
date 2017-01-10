//-----------------------------------------------------------------------------------------------
// MakeNormalMap.cpp
// Takes a Vector3 light vector as input and computes lighting for a provided surface color map
// texture and normal displacement map texture


//-----------------------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <gl/gl.h>


#define STBI_HEADER_FILE_ONLY
#include "ThirdParty/stbi/stb_image.c"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/MakeNormalMap.hpp"


//-----------------------------------------------------------------------------------------------
int MakeNormalMap::GetTextureIDOfNormalMappedTexture( const Vector3& lightVector)
{
	Vector3 lightVectorToDot( lightVector.x * -1.0f, lightVector.y * -1.0f, lightVector.z * -1.0f );

	IntVector2 inputTexelSize;
	int numComponents = 0; // Filled in for us to indicate how many color/alpha components the image had (e.g. 3=RGB, 4=RGBA)
	int numComponentsRequested = 0; // don't care; we support 3 (RGB) or 4 (RGBA)

	// Load surface color map texture file
	unsigned char* surfaceColorMapTexture = stbi_load( SURFACE_COLOR_MAP_TEXTURE_FILE.c_str(), &inputTexelSize.x, &inputTexelSize.y, &numComponents, numComponentsRequested );

	int numOfTexels = inputTexelSize.x * inputTexelSize.y;

	std::vector< Rgba > vectorOfColors;

	// Pull texel data from surface color map texture file
	for ( int texelIndex = 0; texelIndex < numOfTexels; texelIndex++ )
	{
		int rIndex = texelIndex * numComponents;
		float r = surfaceColorMapTexture[ rIndex ];
		float g = surfaceColorMapTexture[ rIndex + 1 ];
		float b = surfaceColorMapTexture[ rIndex + 2 ];
		float a = 255.0f; // Hard coding alpha to be 255
		Rgba texel( r, g, b, a );
		vectorOfColors.push_back( texel );
	}

	// Load normal displacement map texture file
	unsigned char* normalDisplacementMapTexture = stbi_load( NORMAL_DISPLACEMENT_MAP_TEXTURE_FILE.c_str(), &inputTexelSize.x, &inputTexelSize.y, &numComponents, numComponentsRequested );

	std::vector< Rgba > vectorOfNormals;

	// Pull texel data from normal displacement map texture file
	for ( int texelIndex = 0; texelIndex < numOfTexels; texelIndex++ )
	{
		int rIndex = texelIndex * numComponents;
		float r = normalDisplacementMapTexture[ rIndex ];
		float g = normalDisplacementMapTexture[ rIndex + 1 ];
		float b = normalDisplacementMapTexture[ rIndex + 2 ];
		float a = 255.0f; // Hard coding alpha to be 255

		// Range map RGB values from -1.0f to +1.0f
		r = RangeMap( r, 0.0f, 255.0f, -1.0f, 1.0f );
		g = RangeMap( g, 0.0f, 255.0f, -1.0f, 1.0f );
		b = RangeMap( b, 0.0f, 255.0f, -1.0f, 1.0f );

		Rgba texel( r, g, b, a );
		vectorOfNormals.push_back( texel );
	}

	// Normalize the light vector
	lightVectorToDot.Normalize();

	std::vector< unsigned char > outputData;

	// Calculate and store texel data for lit texture
	for ( int texelIndex = 0; texelIndex < numOfTexels; texelIndex++ )
	{
		Rgba normalTexel = vectorOfNormals.at( texelIndex );

		Vector3 normalTexelRGB( normalTexel.redValue, normalTexel.blueValue, normalTexel.greenValue );

		float unclampedDotProduct = DotProductVector3( lightVectorToDot, normalTexelRGB );
		float clampedDotProduct = ClampFloatZeroToOne( unclampedDotProduct );

		float r = vectorOfColors[ texelIndex ].redValue * clampedDotProduct;
		float g = vectorOfColors[ texelIndex ].greenValue * clampedDotProduct;
		float b = vectorOfColors[ texelIndex ].blueValue * clampedDotProduct;
		float a = 255.0f; // Hard coding alpha to be 255

		Rgba texel( r, g, b, a );
		outputData.push_back( ( unsigned char ) r );
		outputData.push_back( ( unsigned char ) g );
		outputData.push_back( ( unsigned char ) b );
		outputData.push_back( ( unsigned char ) a );
	}

	// Enable texturing
	glEnable( GL_TEXTURE_2D );

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	int openglTextureID;

	// Ask OpenGL for an unused texName (ID number) to use for this texture
	glGenTextures( 1, ( GLuint* ) &openglTextureID );

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture( GL_TEXTURE_2D, openglTextureID );

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP ); // one of: GL_CLAMP or GL_REPEAT
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP ); // one of: GL_CLAMP or GL_REPEAT

	// Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	/*if ( numComponents == 3 )
		bufferFormat = GL_RGB;
	bufferFormat = GL_RGB;
	*/
	// #FIXME: What happens if numComponents is neither 3 nor 4?

	GLenum internalFormat = bufferFormat; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

	
	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		512,				// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
		512,				// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
		&outputData[0] );	// Location of the actual pixel data bytes/buffer
	

	//stbi_image_free( imageData );

	return openglTextureID;
}