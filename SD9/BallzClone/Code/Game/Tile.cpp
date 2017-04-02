#include "Engine/Renderer/Renderer.hpp"

#include "Game/Tile.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( int numHitsRemaining, const IntVector2& gridPosition )
	: m_numHitsRemaining( numHitsRemaining )
	, m_gridPosition( gridPosition )
	, m_tint( Rgba::RED )
{
	m_worldPosition = Vector2( gridPosition.x * 100.0f + 50.0f, gridPosition.y * 100.0f + 50.0f );
}


//-----------------------------------------------------------------------------------------------
void Tile::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	// Convert current RGB tint to HSL
	Vector4 floatRGBA = m_tint.FloatRepresentation();
	float v, m, vm;
	float hue = 0.0f;
	float saturation = 0.0f;
	float lightness = 0.0f;
	float r2, g2, b2;

	v = max( floatRGBA.x, floatRGBA.y );
	v = max( v, floatRGBA.z );
	m = min( floatRGBA.x, floatRGBA.y );
	m = min( m, floatRGBA.z );

	lightness = ( m + v ) / 2.0f;
	vm = v - m;
	saturation = vm;
	if ( lightness <= 0.5f )
	{
		saturation /= v + m;
	}
	else
	{
		saturation /= 2.0f - v - m;
	}

	r2 = ( v - floatRGBA.x ) / vm;
	g2 = ( v - floatRGBA.y ) / vm;
	b2 = ( v - floatRGBA.z ) / vm;

	if ( floatRGBA.x == v )
	{
		if ( floatRGBA.y == m )
		{
			hue = 5.0f + b2;
		}
		else
		{
			hue = 1.0f - g2;
		}
	}
	else if ( floatRGBA.y == v )
	{
		if ( floatRGBA.z == m )
		{
			hue = 1.0f + r2;
		}
		else
		{
			hue = 3.0f - b2;
		}
	}
	else
	{
		if ( floatRGBA.x == m )
		{
			hue = 3.0f + g2;
		}
		else
		{
			hue = 5.0f - r2;
		}
	}

	hue /= 6.0f;

	// Interpolate to new HSL
	if ( m_numHitsRemaining < 31 )
	{
		hue = RangeMap( m_numHitsRemaining, 1, 30, 0.0f, 1.0f );
	}
	else if ( m_numHitsRemaining < 61 )
	{
		hue = RangeMap( m_numHitsRemaining, 31, 60, 0.0f, 1.0f );
	}
	else if ( m_numHitsRemaining < 91 )
	{
		hue = RangeMap( m_numHitsRemaining, 61, 90, 0.0f, 1.0f );
	}
	else
	{
		// Need to support higher scores
		hue = RangeMap( m_numHitsRemaining, 1, 1000, 0.0f, 1.0f );
	}
	saturation = 0.99f;
	lightness = 0.4f;

	// Convert new HSL to RGB tint
	float red = lightness;
	float green = lightness;
	float blue = lightness;
	int sextant;
	float sv, fract, vsf, mid1, mid2;

	if ( lightness <= 0.5f )
	{
		v = lightness * ( 1.0f + saturation );
	}
	else
	{
		v = lightness + saturation - lightness * saturation;
	}

	m = lightness + lightness - v;
	sv = ( v - m ) / v;
	hue *= 6.0f;
	sextant = ( int ) hue;
	fract = hue - sextant;
	vsf = v * sv * fract;
	mid1 = m + vsf;
	mid2 = v - vsf;

	switch ( sextant )
	{
		case 0:
		{
			red = v;
			green = mid1;
			blue = m;
			break;
		}
		case 1:
		{
			red = mid2;
			green = v;
			blue = m;
			break;
		}
		case 2:
		{
			red = m;
			green = v;
			blue = mid1;
			break;
		}
		case 3:
		{
			red = m;
			green = mid2;
			blue = v;
			break;
		}
		case 4:
		{
			red = mid1;
			green = m;
			blue = v;
			break;
		}
		case 5:
		{
			red = v;
			green = m;
			blue = mid2;
			break;
		}
	}

	// Holy shit we finally have an interpolated RGB color
	m_tint = Rgba( red * 255.0f, green * 255.0f, blue * 255.0f, 255 );
}


//-----------------------------------------------------------------------------------------------
void Tile::Render()
{
	Vector2 screenSize( 7.0f, 9.0f );
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), screenSize );

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	float fontCellHeight = 0.25f;

	g_theRenderer->DepthTestingEnabled( false );

	g_theRenderer->DrawQuad( Vector2( ( float ) m_gridPosition.x + 0.05f, ( float ) m_gridPosition.y + 0.05f ), Vector2( ( float ) m_gridPosition.x + 0.95f, ( float ) m_gridPosition.y + 0.95f ), m_tint );
	if ( m_numHitsRemaining < 10 )
	{
		g_theRenderer->DrawText2D( Vector2( ( float ) m_gridPosition.x + 0.35f, ( float ) m_gridPosition.y + 0.33f ), std::to_string( m_numHitsRemaining ), fontCellHeight, Rgba::BLACK, fixedFont );
	}
	else
	{
		g_theRenderer->DrawText2D( Vector2( ( float ) m_gridPosition.x + 0.2f, ( float ) m_gridPosition.y + 0.33f ), std::to_string( m_numHitsRemaining ), fontCellHeight, Rgba::BLACK, fixedFont );
	}

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	//g_theRenderer->DrawFilledPolygon( 20, m_worldPosition.x, m_worldPosition.y, 10.0f, Rgba::RED );
}