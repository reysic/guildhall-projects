#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include "Game/Tile.hpp"


//-----------------------------------------------------------------------------------------------
Tile::Tile( int numHitsRemaining, const IntVector2& gridPosition )
	: m_numHitsRemaining( numHitsRemaining )
	, m_gridPosition( gridPosition )
	, m_tint( Rgba::RED )
	, m_isAnimating( false )
	, m_animationStartTime( 0.0f )
	, m_quadRenderOffset( 0.05f )
{
	m_worldPosition = Vector2( gridPosition.x * 100.0f + 50.0f, gridPosition.y * 100.0f + 50.0f );
}


//-----------------------------------------------------------------------------------------------
void Tile::Update( float deltaSeconds )
{
	UpdateTint();
	UpdateHitAnimation( deltaSeconds );
}


//-----------------------------------------------------------------------------------------------
void Tile::UpdateTint()
{
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
		hue = RangeMap( ( float ) m_numHitsRemaining, 1.0f, 30.0f, 0.0f, 1.0f );
	}
	else if ( m_numHitsRemaining < 61 )
	{
		hue = RangeMap( ( float ) m_numHitsRemaining, 31.0f, 60.0f, 0.0f, 1.0f );
	}
	else if ( m_numHitsRemaining < 91 )
	{
		hue = RangeMap( ( float ) m_numHitsRemaining, 61.0f, 90.0f, 0.0f, 1.0f );
	}
	else
	{
		// Need to support higher scores
		hue = RangeMap( ( float ) m_numHitsRemaining, 1.0f, 1000.0f, 0.0f, 1.0f );
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
	m_tint = Rgba( ( unsigned char ) ( red * 255.0f ), ( unsigned char ) ( green * 255.0f ), ( unsigned char ) ( blue * 255.0f ), 255U );
}


//-----------------------------------------------------------------------------------------------
void Tile::UpdateHitAnimation( float deltaSeconds )
{
	deltaSeconds;

	if ( GetCurrentTimeSeconds() > ( m_animationStartTime + HIT_ANIMATION_LENGTH_SEC ) )
	{
		m_isAnimating = false;
		m_quadRenderOffset = 0.05f;
	}

	if ( m_isAnimating )
	{
		m_quadRenderOffset = RangeMap( sin( ( float ) GetCurrentTimeSeconds() * 20.0f ), -1.0f, 1.0f, 0.05f, 0.10f );
	}
}


//-----------------------------------------------------------------------------------------------
void Tile::Render()
{
	Vector2 screenSize( 7.0f, 9.0f );
	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), screenSize );

	g_theRenderer->DepthTestingEnabled( false );

	g_theRenderer->DrawQuad( Vector2( ( float ) m_gridPosition.x + m_quadRenderOffset, ( float ) m_gridPosition.y + m_quadRenderOffset ), Vector2( ( float ) m_gridPosition.x + ( 1.0f - m_quadRenderOffset ), ( float ) m_gridPosition.y + ( 1.0f - m_quadRenderOffset ) ), m_tint );

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	float fontCellHeight = 0.25f;
	float textWidth = BitmapFont::GetTextWidth( std::to_string( m_numHitsRemaining ), fontCellHeight );
	Vector2 textDrawPosition = Vector2( m_gridPosition.x + 0.5f - ( textWidth / 2.0f ), m_gridPosition.y + 0.5f - ( fontCellHeight / 2.0f ) );
	g_theRenderer->DrawText2D( textDrawPosition, std::to_string( m_numHitsRemaining ), fontCellHeight, Rgba::BLACK, fixedFont );

	g_theRenderer->SetOrtho( Vector2( 0.0f, 0.0f ), Vector2( 700.0f, 900.0f ) );

	//g_theRenderer->DrawFilledPolygon( 20, m_worldPosition.x, m_worldPosition.y, 10.0f, Rgba::RED );
}


//-----------------------------------------------------------------------------------------------
void Tile::StartHitAnimation()
{
	m_isAnimating = true;
	m_animationStartTime = ( float ) GetCurrentTimeSeconds();
}