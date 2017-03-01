#pragma once


//-----------------------------------------------------------------------------------------------
const int MAX_ENEMY_COUNT = 1000;
const int MAX_BULLET_COUNT = 1000;
const float PLAYER_DISC_FIRING_RATE_BULLETS_PER_SECOND = 10.0f;

//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
class PlayerDisc;
class PlayerBullet;
class EnemyDisc;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	TheGame();
	void Input();
	void Update( float deltaSeconds );
	void Render() const;
	void SpawnPlayerBullet();
	void DestroyDeadBullets();
	void SpawnEnemyDisc();
	void CheckEnemiesForCollisions();
	void DestroyEnemy( EnemyDisc* enemy );
	bool GetDebugModeStatus();
	bool IsPlayerDiskAlive() const;
	Vector2 GetPlayerDiskPosition() const;
	bool IsQuitting();

	PlayerDisc* m_player;
	PlayerBullet* m_bullets[ MAX_BULLET_COUNT ];
	EnemyDisc* m_enemies[ MAX_ENEMY_COUNT ];

private:
	bool m_waitToSpawnNewPlayerBullet;
	float m_timeBetweenPlayerBullets;
	float m_timeLastPlayerBulletFired;
	bool m_screenIsFlashing;
	float m_screenFlashStartTime;
};