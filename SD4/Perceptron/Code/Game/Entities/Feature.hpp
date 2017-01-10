#pragma once

#include <vector>
#include <string>

#include "Game/Entities/GameEntity.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
enum FeatureState
{
	FEATURE_STATE_DEACTIVATED = 0,
	FEATURE_STATE_CLOSED = 0,
	FEATURE_STATE_OFF = 0,
	FEATURE_STATE_ACTIVATED = 1,
	FEATURE_STATE_OPEN = 1,
	FEATURE_STATE_ON = 1
};


//-----------------------------------------------------------------------------------------------
enum FeatureProperties
{
	FEATURE_PROPERTY_OFF_BLOCK_MOVEMENT = 1 << 0,
	FEATURE_PROPERTY_OFF_BLOCK_LOS		= 1 << 1,
	FEATURE_PROPERTY_ON_BLOCK_MOVEMENT	= 1 << 2,
	FEATURE_PROPERTY_ON_BLOCK_LOS		= 1 << 3,
	FEATURE_PROPERTY_CAN_TURN_ON		= 1 << 4,
	FEATURE_PROPERTY_CAN_TURN_OFF		= 1 << 5,
};


//-----------------------------------------------------------------------------------------------
enum FeatureType
{
	FEATURE_TYPE_DOOR, // OFF - CLOSED, ON - OPEN
	NUM_FEATURE_TYPES
};


//-----------------------------------------------------------------------------------------------
class Feature : public GameEntity
{
public:
	Feature( const XMLNode& featureBlueprintNode, GameEntityType type );
	Feature( const Feature& copySource );

	virtual float Update();

	FeatureState m_state;
	FeatureType m_featureType;
	int m_featureProperties;
	std::string m_offGlyph;
	std::string m_onGlyph;
	bool IsState( FeatureState state ) { return m_state == state; };
	bool IsBlockingMovement();
	bool IsBlockingLineOfSight();
	bool CanToggleActivate();
	void ChangeState( FeatureState newState );
	void ToggleState();
	void SetGlyphForCurrentState();

	static FeatureType GetFeatureTypeFromString( const std::string& typeAsString );
};