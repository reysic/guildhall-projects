#pragma once

#include "Game/Entities/GameEntity.hpp"


//-----------------------------------------------------------------------------------------------
enum ItemType
{
	ITEM_TYPE_WEAPON = 0,
	ITEM_TYPE_ARMOR,
	ITEM_TYPE_POTION,
	NUM_ITEM_TYPES
};


//-----------------------------------------------------------------------------------------------
struct XMLNode;


//-----------------------------------------------------------------------------------------------
class Item : public GameEntity
{
public:
	Item( const XMLNode& itemBlueprintNode, GameEntityType type );
	Item( const Item& copyFrom );

	virtual float Update();
	virtual void Render() const;

public:
	static ItemType GetItemTypeFromString( const std::string& itemTypeString );
	static std::string GetStringFromItemType( ItemType type );
	ItemType GetItemType() const { return m_itemType; }
	int GetDamageAmount() const { return m_damageAmount; }
	int GetArmorAmount() const { return m_armorAmount; }
	int GetHealAmount() const { return m_healAmount; }

private:
	ItemType m_itemType;
	int m_damageAmount;
	int m_armorAmount;
	int m_healAmount;
};