#include "Engine/Tools/Parsers/xmlParser.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Game/Entities/ItemFactory.hpp"


//-----------------------------------------------------------------------------------------------
std::map< std::string, ItemFactory* >* ItemFactory::s_itemFactories[ NUM_ITEM_TYPES ];


//-----------------------------------------------------------------------------------------------
ItemFactory::ItemFactory( const XMLNode& ItemBlueprintNode )
	: m_name( "" )
{
	if ( ItemBlueprintNode.getAttribute( "name" ) )
	{
		std::string nameStr = ItemBlueprintNode.getAttribute( "name" );
		m_name = nameStr;

		PopulateFromXMLNode( ItemBlueprintNode );
	}
}


//-----------------------------------------------------------------------------------------------
void ItemFactory::LoadItemBlueprints()
{
	if ( s_itemFactories[ 0 ] == nullptr )
	{
		for ( int itemTypeIndex = 0; itemTypeIndex < NUM_ITEM_TYPES; ++itemTypeIndex )
		{
			s_itemFactories[ itemTypeIndex ] = new std::map< std::string, ItemFactory* >;
		}
	}

	std::vector< std::string > ItemFiles = EnumerateFilesInFolder( "Data/Items",
		"*.Item.xml" );

	for ( std::string str : ItemFiles )
	{
		XMLNode node = XMLNode::openFileHelper( str.c_str() );
		XMLNode rootNode = node.getChildNode( 0 ); // ItemBlueprints

		if ( !rootNode.IsContentEmpty() )
		{
			int ItemCount = rootNode.nChildNode(); // # of ItemBlueprint nodes

			for ( int i = 0; i < ItemCount; i++ )
			{
				XMLNode thisNode = rootNode.getChildNode( i );
				std::string name = thisNode.getName();
				if ( name != "ItemBlueprint" )
				{
					// Not a valid node,skip
					continue;
				}

				ItemType nodeType = NUM_ITEM_TYPES;
				if ( thisNode.getAttribute( "type" ) )
				{
					nodeType = Item::GetItemTypeFromString( thisNode.getAttribute( "type" ) );
				}
				else
				{
					continue;
				}

				ItemFactory* newFactory = new ItemFactory( thisNode );
				if ( newFactory != nullptr && newFactory->m_name != "" )
				{
					s_itemFactories[ nodeType ]->insert( std::pair< std::string, ItemFactory* >( newFactory->m_name, newFactory ) );
				}

			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
Item* ItemFactory::CreateByName( const std::string& factoryName, ItemType type )
{
	Item* item = nullptr;
	ItemFactory* itemFactory = nullptr;

	ASSERT_OR_DIE( s_itemFactories != nullptr, "No Item Factories." );

	std::map< std::string, ItemFactory* >::iterator itemFactoryIter = s_itemFactories[ type ]->find( factoryName );

	if ( itemFactoryIter != s_itemFactories[ type ]->end() )
	{
		itemFactory = itemFactoryIter->second;
		item = itemFactory->Create();
		return item;
	}
	else
	{
		DebuggerPrintf( "Did not find factory of name %s\n", factoryName );
	}
	return nullptr;
}


//-----------------------------------------------------------------------------------------------
void ItemFactory::PopulateFromXMLNode( const XMLNode& ItemBlueprintNode )
{
	m_templateItem = new Item( ItemBlueprintNode, GAME_ENTITY_ITEM );
}


//-----------------------------------------------------------------------------------------------
std::vector< Item* > ItemFactory::MakeItemsOfType( ItemType type, int amount /*= 1*/ )
{
	// Makes specified amount of items of a particular type. Picks randomly from the array.
	std::vector<Item*> outputVector;

	int itemCount = 0;
	while ( itemCount < amount )
	{
		int itemFactoryIndex = RandomInt( s_itemFactories[ type ]->size(), 0 );

		auto itemFactoryIter = s_itemFactories[ type ]->begin();
		std::advance( itemFactoryIter, itemFactoryIndex );
		Item* item = ( itemFactoryIter )->second->Create();
		outputVector.push_back( item );
		itemCount++;
	}

	return outputVector;
}