#pragma once

#include <map>
#include <string>

#include "Game/Entities/Item.hpp"


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class ItemFactory
{
public:
	ItemFactory( const XMLNode& itemBlueprintNode );

	Item* Create() { return new Item( *m_templateItem ); };

	static void LoadItemBlueprints();
	static Item* CreateByName( const std::string& factoryName, ItemType type );

	static std::map< std::string, ItemFactory* >* s_itemFactories[ NUM_ITEM_TYPES ];
	static std::vector< Item* > MakeItemsOfType( ItemType type, int amount = 1 );

private:
	void PopulateFromXMLNode( const XMLNode& itemBlueprintNode );

private:
	Item* m_templateItem;
	std::string m_name;
};