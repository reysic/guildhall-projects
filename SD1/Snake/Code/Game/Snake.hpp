#pragma once

#include <vector>

//-----------------------------------------------------------------------------------------------
class Vector2; // Forward declaration of Vector2


//-----------------------------------------------------------------------------------------------
const float SNAKE_DEATH_ANIM_SECONDS = 1.5f;


//-----------------------------------------------------------------------------------------------
class Snake
{
public:
	Snake();
	void Update( float deltaSeconds );
	void Move();
	void CheckForAppleCollision();
	Vector2 PickNewApplePosition();
	bool CheckForTailCollision( const Vector2& position);
	void Render() const;
	void RenderDeathAnim() const;
	float GetFractionDeathAnimComplete() const;
	bool NextMoveKills();
	void SetAgeAtDeath();

	Vector2 m_position;
	Vector2 m_applePosition;
	bool m_movingUp;
	bool m_movingDown;
	bool m_movingLeft;
	bool m_movingRight;
	bool m_hasDied;
	bool m_waitToAcceptUpArrow;
	bool m_waitToAcceptDownArrow;
	bool m_waitToAcceptLeftArrow;
	bool m_waitToAcceptRightArrow;
	float m_timeOfLastMoveInSeconds;
	float m_timeBetweenMovesInSeconds;
	float m_timeSnakeDied;
	float m_ageInSeconds;
	float m_ageAtDeath;
	bool m_ageAtDeathHasBeenSet;

	unsigned int m_maxTailSize;

	std::vector< Vector2 > m_tailPositions;

private:
};