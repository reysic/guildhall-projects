#pragma once


//-----------------------------------------------------------------------------------------------
const int MAX_ASTEROID_COUNT = 100;
const int MAX_BULLET_COUNT = 200;


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
class Ship; // Forward declaration of Ship
class Asteroid; // Forward declaration of Asteroid
class Bullet; // Forward declaration of Bullet


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
	void SpawnAsteroid();
	void SpawnAsteroid( const Vector2& spawnPosition, int asteroidSize );
	void DestroyRandomAsteroid();
	void DestroySpecificAsteroid( Asteroid* asteroid );
	void CheckAsteroidsForCollisions();
	void SpawnBullet();
	void DestroyDeadBullets();
	bool GetDebugModeStatus();
	void ShakeScreen();

	Ship* m_ship;
	Asteroid* m_asteroids[ MAX_ASTEROID_COUNT ];
	Bullet* m_bullets[ MAX_BULLET_COUNT ];

private:
	bool m_waitToSpawnNewAsteroid;
	bool m_waitToDestroyRandomAsteroid;
	bool m_waitToSpawnNewBullet;
	bool m_waitToSpawnNewBulletXbox;
	bool m_debugModeOn;
	bool m_waitToToggleDebugMode;
	int m_numAliveAsteroids;
	int m_numAsteroidsInWave;
};