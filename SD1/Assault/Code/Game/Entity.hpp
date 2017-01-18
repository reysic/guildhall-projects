//-----------------------------------------------------------------------------------------------
// Entity.hpp
// Base class for in-game entities


//-----------------------------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2; // forward declaration of Vector2
class AABB2;
class Texture;


//-----------------------------------------------------------------------------------------------
enum EntityType
{
	ENTITY_TYPE_PLAYER_TANK, // 0
	ENTITY_TYPE_PLAYER_BULLET, // 1
	ENTITY_TYPE_ENEMY_TANK, // 2
	ENTITY_TYPE_ENEMY_BULLET, // 3
	ENTITY_TYPE_TURRET, // 4
	NUM_ENTITY_TYPES
};


//-----------------------------------------------------------------------------------------------
class Entity
{
public:
	// Constructors
	Entity();

	// Methods
	virtual void Update( float deltaSeconds );
	void Move( float deltaSeconds );
	virtual void Render() const;
	virtual bool UsesCorrectivePhysics() const;
	virtual bool IsProjectile() const;

	// Accessors
	Vector2 GetPosition() const;
	float GetOrientationDegrees() const;
	float GetPhysicsRadius() const;
	float GetCosmeticRadius() const;
	bool GetIsDead() const;
	AABB2 GetBoundingBox() const;
	Texture* GetTexture() const;
	Rgba GetTint() const;
	int GetHealth() const;

	// Mutators
	void SetPosition( Vector2 newPosition );
	void SetVelocity( Vector2 newVelocity );
	void SetOrientationDegrees( float newOrientationDegrees );
	void SetAngularVelocityDegreesPerSecond( float newAngularVelocityDegreesPerSecond );
	void SetPhysicsRadius( float newPhysicsRadius );
	void SetCosmeticRadius( float newCosmeticRadius );
	void SetIsDead( bool newIsDead );
	void SetBoundingBoxMins( Vector2 newBoundingBoxMins );
	void SetBoundingBoxMaxs( Vector2 newBoundingBoxMaxs );
	void SetTexture( Texture* newTexture );
	void SetTint( Rgba newTint );
	void SetHealth( int newHealth );

	// Members
	EntityType m_type;
	bool	   m_correctivePhysicsAppliedThisFrame;

private:
	// Members
	Vector2		m_position;
	Vector2		m_velocity;
	float		m_orientationDegrees;
	float		m_angularVelocityDegreesPerSecond;
	float		m_physicsRadius;
	float		m_cosmeticRadius;
	float		m_ageInSeconds;
	bool		m_isDead;
	AABB2		m_bounds;
	Texture*	m_texture;
	Rgba		m_tint;
	int			m_health;
};