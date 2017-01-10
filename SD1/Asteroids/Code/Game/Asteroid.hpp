#pragma once


//-----------------------------------------------------------------------------------------------
class Asteroid : public Entity
{
public:
	Asteroid();
	Asteroid( const Vector2& spawnPosition, int asteroidSize );
	void Update( float deltaSeconds );
	void Turn( float deltaSeconds );
	void Render() const;

	int m_asteroidSize; // 3 - large, 2 - medium, 1 - small

private:
	int m_asteroidType;
};