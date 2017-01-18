#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2; // Forward declaration of Vector2


//-----------------------------------------------------------------------------------------------
const float SHIP_DEATH_ANIM_SECONDS = 1.0f;
const float SHIP_THRUST_STRENGTH = 500.0f;
const float SHIP_DEGREES_TO_TURN_PER_SECOND = 200.0f;


//-----------------------------------------------------------------------------------------------
class Ship : public Entity
{
public:
	Ship();
	void Update( float deltaSeconds );
	void ThrustForward( float deltaSeconds );
	void Turn( float deltaSeconds );
	void Render() const;
	void RenderDeathAnim() const;
	void DestroyShip();
	void Reset();
	float GetFractionDeathAnimComplete() const;
	int GetBombCount() const;
	void SetBombCount( int newBombCount );

private:
	float m_ageAtDeath;
	bool m_isThrusting;
	bool m_stopDeathAnim;
	int m_bombCount;
};