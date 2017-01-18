#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Feature.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Maps/Map.hpp"


//-----------------------------------------------------------------------------------------------
Feature::Feature( const XMLNode& featureBlueprintNode, GameEntityType type )
	: GameEntity( type )
	, m_featureProperties( 0 )
	, m_state( FEATURE_STATE_OFF )
	, m_featureType( NUM_FEATURE_TYPES)
	, m_offGlyph( "-" )
	, m_onGlyph( "+" )
{
	if ( featureBlueprintNode.getAttribute( "name" ) )
	{
		std::string nameStr = featureBlueprintNode.getAttribute( "name" );
		m_name = nameStr;
	}

	if ( featureBlueprintNode.getAttribute( "glyph" ) )
	{
		std::string glyphStr = featureBlueprintNode.getAttribute( "glyph" );
		m_glyph = glyphStr;
	}

	if ( featureBlueprintNode.getAttribute( "color" ) )
	{
		std::string colorStr = featureBlueprintNode.getAttribute( "color" );

		size_t pos = colorStr.find( "," );
		std::string redStr = colorStr.substr( 0, pos );
		colorStr.erase( 0, pos + 1 );

		pos = colorStr.find( "," );
		std::string greenStr = colorStr.substr( 0, pos );
		colorStr.erase( 0, pos + 1 );

		std::string blueStr = colorStr;

		m_color = Rgba( ( unsigned char ) stoi( redStr ), ( unsigned char ) stoi( greenStr ),
			( unsigned char ) stoi( blueStr ), ( unsigned char ) 255 );
	}

	// Get the rest of the attributes
	std::string featureType = ReadXMLAttribute( featureBlueprintNode, "type", "" );
	if ( featureType == "Door" || featureType == "door" )
	{
		m_featureType = FEATURE_TYPE_DOOR;
	}
	else
	{
		m_featureType = NUM_FEATURE_TYPES;
	}

	std::string startState = ReadXMLAttribute( featureBlueprintNode, "startState", "" );


	// #TODO: Add other states
	if ( startState == "open" || startState == "true" )
	{
		m_state = FEATURE_STATE_OPEN;
	}
	else if ( startState == "close" || startState == "closed" || startState == "false" )
	{
		m_state = FEATURE_STATE_CLOSED;
	}
	else
	{
		m_state = FEATURE_STATE_OFF;
	}

	m_offGlyph = ReadXMLAttribute( featureBlueprintNode, "offGlyph", m_offGlyph );
	m_onGlyph = ReadXMLAttribute( featureBlueprintNode, "onGlyph", m_onGlyph );

	bool offBlocksMovement = ReadXMLAttribute( featureBlueprintNode, "offBlocksMovement", true );
	bool onBlocksMovement = ReadXMLAttribute( featureBlueprintNode, "onBlocksMovement", true );

	bool offBlocksLOS = ReadXMLAttribute( featureBlueprintNode, "offBlocksLOS", true );
	bool onBlocksLOS = ReadXMLAttribute( featureBlueprintNode, "onBlocksLOS", true );

	bool canTurnOff = ReadXMLAttribute( featureBlueprintNode, "canTurnOff", false );
	bool canTurnOn = ReadXMLAttribute( featureBlueprintNode, "canTurnOn", false );


	// Movement
	if ( offBlocksMovement )
		m_featureProperties |= FEATURE_PROPERTY_OFF_BLOCK_MOVEMENT;
	if ( onBlocksMovement )
		m_featureProperties |= FEATURE_PROPERTY_ON_BLOCK_MOVEMENT;

	// LOS
	if ( offBlocksLOS )
		m_featureProperties |= FEATURE_PROPERTY_OFF_BLOCK_LOS;
	if ( onBlocksLOS )
		m_featureProperties |= FEATURE_PROPERTY_ON_BLOCK_LOS;
	// Activation
	if ( canTurnOff )
		m_featureProperties |= FEATURE_PROPERTY_CAN_TURN_OFF;
	if ( canTurnOn )
		m_featureProperties |= FEATURE_PROPERTY_CAN_TURN_ON;

	SetGlyphForCurrentState();
}


//-----------------------------------------------------------------------------------------------
Feature::Feature( const Feature& copySource )
	: GameEntity( copySource )
	, m_state( copySource.m_state )
	, m_featureProperties( copySource.m_featureProperties )
	, m_featureType( copySource.m_featureType )
	, m_offGlyph( copySource.m_offGlyph )
	, m_onGlyph( copySource.m_onGlyph )
{
	SetGlyphForCurrentState();
}


//-----------------------------------------------------------------------------------------------
float Feature::Update()
{
	return GameEntity::Update();
}


//-----------------------------------------------------------------------------------------------
bool Feature::IsBlockingMovement()
{
	// Get State
	if ( m_state == 0 )
	{
		// Check bits
		if ( ( m_featureProperties & FEATURE_PROPERTY_OFF_BLOCK_MOVEMENT ) != 0 )
		{
			return true;
		}
	}
	else
	{
		// Check bits
		if ( ( m_featureProperties & FEATURE_PROPERTY_ON_BLOCK_MOVEMENT ) != 0 )
		{
			return true;
		}
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool Feature::IsBlockingLineOfSight()
{
	// Get State
	if ( m_state == 0 )
	{
		// Check bits
		if ( ( m_featureProperties & FEATURE_PROPERTY_OFF_BLOCK_LOS ) != 0 )
		{
			return true;
		}

	}
	else
	{
		// Check bits
		if ( ( m_featureProperties & FEATURE_PROPERTY_ON_BLOCK_LOS ) != 0 )
		{
			return true;
		}

	}
	
	return false;
}


//-----------------------------------------------------------------------------------------------
bool Feature::CanToggleActivate()
{
	// Get State
	if ( m_state == 0 ) // Feature is off, check if can turn on
	{
		// Check bits
		if ( ( m_featureProperties & FEATURE_PROPERTY_CAN_TURN_ON ) != 0 )
		{
			return true;
		}
	}
	else
	{
		// Check bits
		if ( ( m_featureProperties & FEATURE_PROPERTY_CAN_TURN_OFF ) != 0 )
		{
			return true;
		}
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
void Feature::ChangeState( FeatureState newState )
{
	m_state = newState;
	SetGlyphForCurrentState();
}


//-----------------------------------------------------------------------------------------------
void Feature::ToggleState()
{
	if ( CanToggleActivate() )
	{
		if ( m_state == 0 )
		{
			ChangeState( FEATURE_STATE_ON );
		}
		else
		{
			ChangeState( FEATURE_STATE_OFF );
		}
	}

}


//-----------------------------------------------------------------------------------------------
void Feature::SetGlyphForCurrentState()
{
	if ( m_state == 0 )
	{
		m_glyph = m_offGlyph;
	}
	else
	{
		m_glyph = m_onGlyph;
	}
}


//-----------------------------------------------------------------------------------------------
FeatureType Feature::GetFeatureTypeFromString( const std::string& typeAsString )
{
	if ( typeAsString == "Door" || typeAsString == "door" )
	{
		return FEATURE_TYPE_DOOR;
	}

	return NUM_FEATURE_TYPES;
}