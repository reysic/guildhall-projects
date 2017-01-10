#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Tools/Parsers/XMLUtilities.hpp"
#include "Game/Entities/Item.hpp"


//-----------------------------------------------------------------------------------------------
Item::Item( const XMLNode& itemBlueprintNode, GameEntityType type )
	: GameEntity( type )
	, m_itemType( NUM_ITEM_TYPES )
	, m_damageAmount( 0 )
	, m_armorAmount( 0 )
	, m_healAmount( 0 )
{
	std::string itemTypeString = ReadXMLAttribute( itemBlueprintNode, "type", "" );
	m_itemType = GetItemTypeFromString( itemTypeString );

	m_name = ReadXMLAttribute( itemBlueprintNode, "name", "NO ITEM NAME" );

	m_itemType = GetItemTypeFromString( itemTypeString );

	m_damageAmount = ReadXMLAttribute( itemBlueprintNode, "damageAmount", m_damageAmount );

	m_armorAmount = ReadXMLAttribute( itemBlueprintNode, "armorAmount", m_armorAmount );

	m_healAmount = ReadXMLAttribute( itemBlueprintNode, "healAmount", m_healAmount );

	if ( itemBlueprintNode.getAttribute( "glyph" ) )
	{
		std::string glyphStr = itemBlueprintNode.getAttribute( "glyph" );
		m_glyph = glyphStr;
	}
	else
	{
		// No glyph attribute found
		m_glyph = "?";
	}

	if ( itemBlueprintNode.getAttribute( "color" ) )
	{
		std::string colorStr = itemBlueprintNode.getAttribute( "color" );

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
}


//-----------------------------------------------------------------------------------------------
Item::Item( const Item& copyFrom )
	: GameEntity( copyFrom )
	, m_itemType( copyFrom.m_itemType )
	, m_damageAmount( copyFrom.m_damageAmount )
	, m_armorAmount( copyFrom.m_armorAmount )
	, m_healAmount( copyFrom.m_healAmount )
{

}


//-----------------------------------------------------------------------------------------------
float Item::Update()
{
	return 1.0f;
}


//-----------------------------------------------------------------------------------------------
void Item::Render() const
{
	Cell& cellItemIsIn = m_map->GetCellAtPosition( m_position );

	// Don't render item if item's cell contains an agent
	if ( cellItemIsIn.m_containsAgent )
		return;

	// Don't render item if cell item is in is not visible and map is not in debug render mode
	if ( !cellItemIsIn.m_isVisible && !m_map->m_debugRender )
		return;

	if ( cellItemIsIn.m_containedItems.size() > 1 )
	{
		// Render item as * if cell contains multiple items
		static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );

		// Calculate offset based on map size
		Vector2 offset = m_map->m_renderingOffset;

		Vector2 entityPosition( ( float ) m_position.x * 20.0f, ( float ) m_position.y * 20.0f );
		g_theRenderer->DrawText2D( offset + entityPosition, "*", 15.0f, Rgba( 56, 200, 244, 255 ), fixedFont );
	}
	else
	{
		// No agent in item's cell, cell does not contain multiple items, call parent class Render()
		GameEntity::Render();
	}
}


//-----------------------------------------------------------------------------------------------
ItemType Item::GetItemTypeFromString( const std::string& itemTypeString )
{
	if ( itemTypeString == "Weapon" )
	{
		return ITEM_TYPE_WEAPON;
	}

	if ( itemTypeString == "Armor" )
	{
		return ITEM_TYPE_ARMOR;
	}

	if ( itemTypeString == "Potion" )
	{
		return ITEM_TYPE_POTION;
	}

	return NUM_ITEM_TYPES;
}


//-----------------------------------------------------------------------------------------------
std::string Item::GetStringFromItemType( ItemType type )
{
	switch ( type )
	{
	case ITEM_TYPE_WEAPON:
		return "Weapon";
		break;
	case ITEM_TYPE_ARMOR:
		return "Armor";
		break;
	case ITEM_TYPE_POTION:
		return "Potion";
		break;
	case NUM_ITEM_TYPES:
		return "?";
		break;
	default:
		return "?";
		break;
	}
}
