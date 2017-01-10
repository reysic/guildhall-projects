#pragma once

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Rgba.hpp"


//-----------------------------------------------------------------------------------------------
enum EntityType
{
	ENTITY_TYPE_PLAYER = 0,
	ENTITY_TYPE_SEEK_ENEMY,
	ENTITY_TYPE_PATH_ENEMY,
	ENTITY_TYPE_WANDER_ENEMY,
	ENTITY_TYPE_PURSUE_ENEMY,
	ENTITY_TYPE_PLAYER_BULLET,
	ENTITY_TYPE_ENEMY_BULLET,
	ENTITY_TYPE_PICKUP,
	ENTITY_TYPE_STAR,
	NUM_ENTITY_TYPES
};


//-----------------------------------------------------------------------------------------------
enum BulletType
{
	BULLET_TYPE_PLAYER = 0,
	BULLET_TYPE_ENEMY,
	NUM_BULLET_TYPES
};


//-----------------------------------------------------------------------------------------------
typedef Vector2 WorldCoords;
class Entity
{
public:
	// Constructors & Destructors
	Entity();
	~Entity();

	// Methods
	virtual void Update( float deltaSeconds );
	void Move( float deltaSeconds );
	virtual void Render() const {}
	virtual bool UsesCorrectivePhysics() const { return false; }
	virtual bool IsProjectile() const { return false; }

	// Accessors
	Vector2 GetPosition() const { return m_position; }
	Vector2 GetVelocity() const { return m_velocity; }
	float GetOrientationDegrees() const { return m_orientationDegrees; }
	float GetAngularVelocityDegreesPerSecond() const { return m_angularVelocityDegreesPerSecond; }
	float GetPhysicsRadius() const { return m_physicsRadius; }
	float GetCosmeticRadius() const { return m_cosmeticRadius; }
	AABB2 GetBoundingBox() const { return m_bounds; }
	Texture* GetTexture() const { return m_texture; }
	Rgba GetTint() const { return m_tint; }
	int GetHealth() const { return m_health; }
	bool GetIsDead() const { return m_isDead; }

	// Mutators
	void SetPosition( Vector2 newPosition ) { m_position = newPosition; }
	void SetVelocity( Vector2 newVelocity ) { m_velocity = newVelocity; }
	void SetOrientationDegrees( float newOrientationDegrees )
	{
		m_orientationDegrees = newOrientationDegrees;
	}
	void SetAngularVelocityDegreesPerSecond( float newAngularVelocityDegreesPerSecond )
	{
		m_angularVelocityDegreesPerSecond = newAngularVelocityDegreesPerSecond;
	}
	void SetPhysicsRadius( float newPhysicsRadius ) { m_physicsRadius = newPhysicsRadius; }
	void SetCosmeticRadius( float newCosmeticRadius ) { m_cosmeticRadius = newCosmeticRadius; }
	void SetBoundingBoxMins( Vector2 newBoundingBoxMins ) { m_bounds.mins = newBoundingBoxMins; }
	void SetBoundingBoxMaxs( Vector2 newBoundingBoxMaxs ) { m_bounds.maxs = newBoundingBoxMaxs; }
	void SetTexture( Texture* newTexture ) { m_texture = newTexture; }
	void SetTint( Rgba newTint ) { m_tint = newTint; }
	void SetHealth( int newHealth ) { m_health = newHealth; }
	void SetIsDead( bool newIsDead ) { m_isDead = newIsDead; }

	// Members
	EntityType m_type;
	bool	   m_correctivePhysicsAppliedThisFrame;

protected:
	Vector2		m_position;
	Vector2		m_velocity;
	float		m_orientationDegrees;
	float		m_angularVelocityDegreesPerSecond;
	float		m_physicsRadius;
	float		m_cosmeticRadius;
	float		m_ageInSeconds;
	AABB2		m_bounds;
	Texture*	m_texture;
	Rgba		m_tint;
	int			m_health;
	bool		m_isDead;
};