#include "Game/ScoringNodes.hpp"
#include "Game/Cloth.hpp"
#include "Game/Node.hpp"
#include "Game/Ball.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB3.hpp"


const int SCORE_MAX = 50;
//-----------------------------------------------------------------------------------------------
ScoringNodes::ScoringNodes( const Cloth& cloth )
{
	int clothWidth = cloth.m_width;
	int clothHeight = cloth.m_height;
	Vector3Double clothTopLeftCornerPosition = cloth.m_topLeftClothPosition;

	int numLanes = clothWidth - 1;
	ASSERT_OR_DIE( numLanes > 0, "Cloth width must be >1." );

	//Vector3Double currentScoringNodePosition = clothTopLeftCornerPosition + Vector3Double( ( clothTopLeftCornerPosition.x + 0.5 ), clothTopLeftCornerPosition.y - clothHeight - 0.5f, 0.0 );
	Vector3Double currentScoringNodePosition = Vector3Double((clothTopLeftCornerPosition.x + 0.5), clothTopLeftCornerPosition.y - clothHeight + 0.5f, 1.2);
	Node* newScoringNode = new Node( currentScoringNodePosition, 1.0 );
	newScoringNode->m_isFixed = true;
	newScoringNode->m_scoreValue = (rand() % SCORE_MAX) + 1;
	m_scoringNodes.push_back( newScoringNode );

	for ( int laneIndex = 2; laneIndex <= numLanes; ++laneIndex )
	{
		currentScoringNodePosition.x += 1.0f;
		newScoringNode = new Node( currentScoringNodePosition, 1.0 );
		newScoringNode->m_isFixed = true;
		newScoringNode->m_scoreValue = (rand() % SCORE_MAX) + 1;
		m_scoringNodes.push_back( newScoringNode );
	}

}


//-----------------------------------------------------------------------------------------------
void ScoringNodes::Update( float deltaSeconds )
{
	deltaSeconds;

	// Check for collisions with any balls
	for ( auto scoringNodeIterator = m_scoringNodes.begin(); scoringNodeIterator != m_scoringNodes.end(); ++scoringNodeIterator )
	{
		for ( auto ballIterator = g_theGame->m_balls.begin(); ballIterator != g_theGame->m_balls.end(); )
		{
			if ( IsColliding( *(*ballIterator), ( *scoringNodeIterator )->m_position, 0.5f ) > 0.00001f )
			{
				// Ball is colliding with scoring node
				ballIterator = g_theGame->m_balls.erase( ballIterator );
				g_theGame->m_score += (*scoringNodeIterator)->m_scoreValue;
			}
			else
			{
				// Ball is not colliding with scoring node
				++ballIterator;
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
void ScoringNodes::Render() const
{
	for ( Node* scoringNode : m_scoringNodes )
	{
		Vector3Double scoringNodePositionDouble = scoringNode->m_position;
		float scoringNodeXPosition = ( float ) scoringNodePositionDouble.x;
		float scoringNodeYPosition = ( float ) scoringNodePositionDouble.y;
		float scoringNodeZPosition = ( float ) scoringNodePositionDouble.z;
		Vector3 scoringNodePosition( scoringNodeXPosition, scoringNodeYPosition, scoringNodeZPosition );
		g_theRenderer->DrawSphere( scoringNodePosition, 0.5f, Rgba::PINK );
		Vector3 mins( scoringNodeXPosition - 0.5f, scoringNodeYPosition - 0.5f, scoringNodeZPosition - 0.5f );
		Vector3 maxs( scoringNodeXPosition + 0.5f, scoringNodeYPosition + 0.5f, scoringNodeZPosition + 0.5f );
		AABB3 bounds( mins, maxs );
		g_theRenderer->DrawAABB3Wireframe( bounds, Rgba::PINK );
		g_theRenderer->DrawTexturedQuad3D( *g_theRenderer->g_DefaultImage.m_spriteSheetTexture, Vector3( maxs.x, maxs.y, maxs.z ), Vector3( mins.x, maxs.y, maxs.z ),
			Vector3( mins.x, mins.y, maxs.z ), Vector3( maxs.x, mins.y, maxs.z ), AABB2( Vector2( 0.0f, 0.0f ), Vector2( 1.0f, 1.0f ) ) );
		static BitmapFontMeta* bitmapFontMeta = BitmapFontMeta::CreateOrGetFont( "Data/Fonts/ArialFont.fnt" );

		g_theRenderer->DrawTextMeta2D( Vector3( mins.x + 0.2f, mins.y + 0.8f, maxs.z + 0.01f ), 0.01f, std::to_string(scoringNode->m_scoreValue), Rgba::BLACK, bitmapFontMeta, Vector3( 0.0f, 1.0f, 0.0f ), Vector3( 1.0f, 0.0f, 0.0 ) );
	}
}


//-----------------------------------------------------------------------------------------------
float ScoringNodes::IsColliding( const Ball& ball, const Vector3Double& scoringNodePosition, float scoringNodeRadius )
{
	Vector3Double vecPrevPos = Vector3Double( ball.m_prevState.m_position.x, ball.m_prevState.m_position.y, ball.m_prevState.m_position.z );
	Vector3Double vecPos = Vector3Double( ball.m_state.m_position.x, ball.m_state.m_position.y, ball.m_state.m_position.z );

	Vector3Double x0 = scoringNodePosition - vecPrevPos;
	Vector3Double e = ( ( scoringNodePosition - vecPos ) - ( x0 ) );
	float R = ball.m_radius + scoringNodeRadius;
	double d1 = DotProduct( x0, e );
	double d2 = DotProduct( e, e );
	double d3 = DotProduct( x0, x0 );

	float Dover4 = ( float ) ( ( d1 * d1 ) - d2 * ( d3 - R * R ) );
	if ( Dover4 > 0 )
	{
		float root1 = ( float ) ( ( -DotProduct( x0, e ) - sqrt( Dover4 ) ) / ( DotProduct( e, e ) ) );
		float root2 = ( float ) ( ( -DotProduct( x0, e ) + sqrt( Dover4 ) ) / ( DotProduct( e, e ) ) );
		if ( root1 <= 1 && root2 >= 0 )
		{
			root1 = root1 > 0.f ? root1 : 0.f;
			root2 = root2 < 1.f ? root2 : 1.f;

			return root1; //TRUE
		}
	}
	return -1.f; //FALSE
}