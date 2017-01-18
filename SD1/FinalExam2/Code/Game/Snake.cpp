#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>

#include <math.h>
#include <stdlib.h>

#include "Engine/Math/Vector2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Game/TheApp.hpp"
#include "Game/TheGame.hpp"
#include "Game/Snake.hpp"


//-----------------------------------------------------------------------------------------------
Snake::Snake()
{
	m_position.x = 450.f;
	m_position.y = 450.f - 23.0f * 8.0f;

	m_movingUp = true;
	m_movingLeft = false;
	m_movingRight = false;
	m_movingDown = false;

	m_hasDied = false;

	m_waitToAcceptUpArrow = false;
	m_waitToAcceptDownArrow = false;
	m_waitToAcceptLeftArrow = false;
	m_waitToAcceptRightArrow = false;

	m_timeOfLastMoveInSeconds = ( float ) GetCurrentTimeSeconds();
	m_timeBetweenMovesInSeconds = 0.1f;

	m_maxTailSize = 10;

	m_applePosition.x = 450.0f + 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
	m_applePosition.y = 450.0f + 23.0f * ( ( float ) ( rand() % 18 + 1 ) );

	m_ageInSeconds = 0.0f;
	m_ageAtDeath = 0.0f;

	m_ageAtDeathHasBeenSet = false;
}


//-----------------------------------------------------------------------------------------------
void Snake::Update( float deltaSeconds )
{
	m_ageInSeconds += deltaSeconds;

	if ( m_hasDied && !m_ageAtDeathHasBeenSet )
	{
		m_ageAtDeath = m_ageInSeconds;
		m_ageAtDeathHasBeenSet = true;
	}

	if ( m_tailPositions.size() > m_maxTailSize )
		m_tailPositions.pop_back();

	// Arrow key input
	if ( g_theApp->GetKeyDownStatus( VK_UP ) && !m_waitToAcceptUpArrow )
	{
		m_movingUp = true;
		m_movingLeft = false;
		m_movingRight = false;
		m_movingDown = false;
		m_waitToAcceptUpArrow = true;
	}

	if ( g_theApp->GetKeyDownStatus( VK_DOWN ) && !m_waitToAcceptDownArrow )
	{
		m_movingUp = false;
		m_movingLeft = false;
		m_movingRight = false;
		m_movingDown = true;
		m_waitToAcceptDownArrow = true;
	}

	if ( g_theApp->GetKeyDownStatus( VK_LEFT ) && !m_waitToAcceptLeftArrow )
	{
		m_movingUp = false;
		m_movingLeft = true;
		m_movingRight = false;
		m_movingDown = false;
		m_waitToAcceptLeftArrow = true;
	}

	if ( g_theApp->GetKeyDownStatus( VK_RIGHT ) && !m_waitToAcceptRightArrow )
	{
		m_movingUp = false;
		m_movingLeft = false;
		m_movingRight = true;
		m_movingDown = false;
		m_waitToAcceptRightArrow = true;
	}


	if ( m_waitToAcceptUpArrow && !( g_theApp->GetKeyDownStatus( VK_UP ) ) )
		m_waitToAcceptUpArrow = false;

	if ( m_waitToAcceptDownArrow && !( g_theApp->GetKeyDownStatus( VK_DOWN ) ) )
		m_waitToAcceptDownArrow = false;

	if ( m_waitToAcceptLeftArrow && !( g_theApp->GetKeyDownStatus( VK_LEFT ) ) )
		m_waitToAcceptLeftArrow = false;

	if ( m_waitToAcceptRightArrow && !( g_theApp->GetKeyDownStatus( VK_RIGHT ) ) )
		m_waitToAcceptRightArrow = false;

	CheckForAppleCollision();

	Move();
}


void Snake::Move()
{
	if ( m_hasDied )
		return;

	std::vector< Vector2 >::iterator vectorIterator;

	float timeOfNextMove = m_timeOfLastMoveInSeconds + m_timeBetweenMovesInSeconds;
	float currentTime = ( float ) GetCurrentTimeSeconds();

	if ( currentTime >= timeOfNextMove )
	{
		if ( m_movingUp )
		{
			if ( NextMoveKills() )
			{
				return;
			}
			if ( CheckForTailCollision( Vector2( m_position.x, m_position.y + 23.0f ) ) )
			{
				m_hasDied = true;
				return;
			}
			m_position.y += 23.0f;
			vectorIterator = m_tailPositions.begin();
			vectorIterator = m_tailPositions.insert( vectorIterator, Vector2( m_position.x, m_position.y - 23.0f ) );
		}
		if ( m_movingDown )
		{
			if ( NextMoveKills() )
			{
				return;
			}
			if ( CheckForTailCollision( Vector2( m_position.x, m_position.y - 23.0f ) ) )
			{
				m_hasDied = true;
				return;
			}
			m_position.y -= 23.0f;
			vectorIterator = m_tailPositions.begin();
			vectorIterator = m_tailPositions.insert( vectorIterator, Vector2( m_position.x, m_position.y + 23.0f ) );
		}
		if ( m_movingLeft )
		{
			if ( NextMoveKills() )
			{
				return;
			}
			if ( CheckForTailCollision( Vector2( m_position.x - 23.0f, m_position.y ) ) )
			{
				m_hasDied = true;
				return;
			}
			m_position.x -= 23.0f;
			vectorIterator = m_tailPositions.begin();
			vectorIterator = m_tailPositions.insert( vectorIterator, Vector2( m_position.x + 23.0f, m_position.y ) );
		}
		if ( m_movingRight )
		{
			if ( NextMoveKills() )
			{
				return;
			}
			if ( CheckForTailCollision( Vector2( m_position.x + 23.0f, m_position.y ) ) )
			{
				m_hasDied = true;
				return;
			}
			m_position.x += 23.0f;
			vectorIterator = m_tailPositions.begin();
			vectorIterator = m_tailPositions.insert( vectorIterator, Vector2( m_position.x - 23.0f, m_position.y ) );
		}

		m_timeOfLastMoveInSeconds = ( float ) GetCurrentTimeSeconds();
	}
}

void Snake::CheckForAppleCollision()
{
	if ( m_position.x == m_applePosition.x && m_position.y == m_applePosition.y )
	{
		m_maxTailSize += 5;

		Vector2 newApplePosition = PickNewApplePosition();
		while ( newApplePosition == m_position || CheckForTailCollision( newApplePosition ) )
		{
			newApplePosition = PickNewApplePosition();
		}

		m_applePosition.x = newApplePosition.x;
		m_applePosition.y = newApplePosition.y;
	}
}


//-----------------------------------------------------------------------------------------------
Vector2 Snake::PickNewApplePosition()
{
	int oneThroughFour = rand() % 4 + 1;

	Vector2 newApplePosition;

	if ( oneThroughFour == 1 )
	{
		newApplePosition.x = 450.0f - 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
		newApplePosition.y = 450.0f - 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
	}
	else if ( oneThroughFour == 2 )
	{
		newApplePosition.x = 450.0f + 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
		newApplePosition.y = 450.0f + 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
	}
	else if ( oneThroughFour == 3 )
	{
		newApplePosition.x = 450.0f + 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
		newApplePosition.y = 450.0f - 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
	}
	else if ( oneThroughFour == 4 )
	{
		newApplePosition.x = 450.0f + 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
		newApplePosition.y = 450.0f - 23.0f * ( ( float ) ( rand() % 18 + 1 ) );
	}

	return newApplePosition;
}


//-----------------------------------------------------------------------------------------------
bool Snake::CheckForTailCollision( const Vector2& position )
{
	for ( unsigned int vectorIndex = 0; vectorIndex < m_tailPositions.size(); vectorIndex++ )
	{
		if ( position == m_tailPositions[ vectorIndex ] )
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------------------------
void Snake::Render() const
{
	if ( m_hasDied && m_ageAtDeathHasBeenSet )
	{
		RenderDeathAnim();
		return;
	}

	// Render apple
	glBegin( GL_QUADS );
	{
		glColor3f( 0.06f, 0.61f, 0.06f );
		glVertex3f( m_applePosition.x - 11.5f, m_applePosition.y - 11.5f, 0.0f ); // bottom left
		glColor3f( 0.02f, 0.22f, 0.02f );
		glVertex3f( m_applePosition.x + 11.5f, m_applePosition.y - 11.5f, 0.0f ); // bottom right
		glColor3f( 0.06f, 0.61f, 0.06f );
		glVertex3f( m_applePosition.x + 11.5f, m_applePosition.y + 11.5f, 0.0f ); // top right
		glColor3f( 0.10f, 1.0f, 0.10f );
		glVertex3f( m_applePosition.x - 11.5f, m_applePosition.y + 11.5f, 0.0f ); // top left
	}
	glEnd();

	glBegin( GL_QUADS );
	{
		glColor3f( 0.70f, 0.70f, 0.70f );
		glVertex3f( m_position.x - 11.5f, m_position.y - 11.5f, 0.0f ); // bottom left
		glColor3f( 0.33f, 0.33f, 0.33f );
		glVertex3f( m_position.x + 11.5f, m_position.y - 11.5f, 0.0f ); // bottom right
		glColor3f( 0.71f, 0.71f, 0.71f );
		glVertex3f( m_position.x + 11.5f, m_position.y + 11.5f, 0.0f ); // top right
		glColor3f( 0.99f, 0.99f, 0.99f );
		glVertex3f( m_position.x - 11.5f, m_position.y + 11.5f, 0.0f ); // top left
	}
	glEnd();

	// Render tail segments
	for ( unsigned int vectorIndex = 0; vectorIndex < m_tailPositions.size(); vectorIndex++ )
	{
		glBegin( GL_QUADS );
		{
			glColor3f( 0.10f, 0.40f, 1.0f );
			glVertex3f( m_tailPositions[ vectorIndex ].x - 11.5f, m_tailPositions[ vectorIndex ].y - 11.5f, 0.0f ); // bottom left
			glVertex3f( m_tailPositions[ vectorIndex ].x + 11.5f, m_tailPositions[ vectorIndex ].y - 11.5f, 0.0f ); // bottom right
			glVertex3f( m_tailPositions[ vectorIndex ].x + 11.5f, m_tailPositions[ vectorIndex ].y + 11.5f, 0.0f ); // top right
			glVertex3f( m_tailPositions[ vectorIndex ].x - 11.5f, m_tailPositions[ vectorIndex ].y + 11.5f, 0.0f ); // top left
		}
		glEnd();
	}
}


//-----------------------------------------------------------------------------------------------
void Snake::RenderDeathAnim() const
{
	if ( GetFractionDeathAnimComplete() == 1.0f )
	{
		g_theGame->Shutdown();
		return;
	}

	// Render apple
	glBegin( GL_QUADS );
	{
		glColor3f( 0.06f, 0.61f, 0.06f );
		glVertex3f( m_applePosition.x - 11.5f, m_applePosition.y - 11.5f, 0.0f ); // bottom left
		glColor3f( 0.02f, 0.22f, 0.02f );
		glVertex3f( m_applePosition.x + 11.5f, m_applePosition.y - 11.5f, 0.0f ); // bottom right
		glColor3f( 0.06f, 0.61f, 0.06f );
		glVertex3f( m_applePosition.x + 11.5f, m_applePosition.y + 11.5f, 0.0f ); // top right
		glColor3f( 0.10f, 1.0f, 0.10f );
		glVertex3f( m_applePosition.x - 11.5f, m_applePosition.y + 11.5f, 0.0f ); // top left
	}
	glEnd();

	glBegin( GL_QUADS );
	{
		glColor4f( 0.71f, 0.00f, 0.00f, 1.0f - GetFractionDeathAnimComplete() );
		glVertex3f( m_position.x - 11.5f, m_position.y - 11.5f, 0.0f ); // bottom left
		glColor4f( 0.33f, 0.00f, 0.00f, 1.0f - GetFractionDeathAnimComplete() );
		glVertex3f( m_position.x + 11.5f, m_position.y - 11.5f, 0.0f ); // bottom right
		glColor4f( 0.71f, 0.00f, 0.00f, 1.0f - GetFractionDeathAnimComplete() );
		glVertex3f( m_position.x + 11.5f, m_position.y + 11.5f, 0.0f ); // top right
		glColor4f( 0.99f, 0.00, 0.00f, 1.0f - GetFractionDeathAnimComplete() );
		glVertex3f( m_position.x - 11.5f, m_position.y + 11.5f, 0.0f ); // top left
	}
	glEnd();

	// Render tail segments
	for ( unsigned int vectorIndex = 0; vectorIndex < m_tailPositions.size(); vectorIndex++ )
	{
		glBegin( GL_QUADS );
		{
			glColor4f( 1.00f, 0.00f, 0.00f, 1.0f - GetFractionDeathAnimComplete() );
			glVertex3f( m_tailPositions[ vectorIndex ].x - 11.5f, m_tailPositions[ vectorIndex ].y - 11.5f, 0.0f ); // bottom left
			glVertex3f( m_tailPositions[ vectorIndex ].x + 11.5f, m_tailPositions[ vectorIndex ].y - 11.5f, 0.0f ); // bottom right
			glVertex3f( m_tailPositions[ vectorIndex ].x + 11.5f, m_tailPositions[ vectorIndex ].y + 11.5f, 0.0f ); // top right
			glVertex3f( m_tailPositions[ vectorIndex ].x - 11.5f, m_tailPositions[ vectorIndex ].y + 11.5f, 0.0f ); // top left
		}
		glEnd();
	}
}


//-----------------------------------------------------------------------------------------------
float Snake::GetFractionDeathAnimComplete() const
{
	float secondsSinceDeath = m_ageInSeconds - m_ageAtDeath;
	float fractionComplete = secondsSinceDeath / SNAKE_DEATH_ANIM_SECONDS;
	fractionComplete = ClampFloatZeroToOne( fractionComplete );

	return fractionComplete;
}


//-----------------------------------------------------------------------------------------------
bool Snake::NextMoveKills()
{
	if ( ( m_position.x - 23.0 ) <= 0.0f )
		m_hasDied = true;

	if ( ( m_position.x + 23.0f ) >= 900.0f )
		m_hasDied = true;

	if ( ( m_position.y - 23.0f ) <= 0.0f )
		m_hasDied = true;

	if ( ( m_position.y + 23.0f ) >= 900.0f )
		m_hasDied = true;

	if ( m_hasDied )
		return true;
	else
		return false;
}
