#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2;


//-----------------------------------------------------------------------------------------------
class AIPlayer
{
public:
	AIPlayer();
	Vector2 GetShotDirection();

public:
	bool m_stillPlaying;
};