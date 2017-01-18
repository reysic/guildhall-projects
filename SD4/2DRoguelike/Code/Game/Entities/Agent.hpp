#pragma once

#include "Game/Entities/GameEntity.hpp"
#include "Game/Entities/Item.hpp"
#include "Game/Pathfinding/Pathfinder.hpp"
#include "Game/Core/Faction.hpp"


//-----------------------------------------------------------------------------------------------
enum EquipmentSlot
{
	EQUIPMENT_SLOT_WEAPON = 0,
	EQUIPMENT_SLOT_CHEST,
	EQUIPMENT_SLOT_POTION,
	NUM_EQUIPMENT_SLOTS
};


//-----------------------------------------------------------------------------------------------
enum MovementProperties
{
	MOVE_PROP_SLOWED_BY_WATER = 1 << 0,
	MOVE_PROP_SLOWED_BY_LAVA = 1 << 1,
	MOVE_PROP_BLOCKED_BY_WATER = 1 << 2,
	MOVE_PROP_BLOCKED_BY_LAVA = 1 << 3,
};


//-----------------------------------------------------------------------------------------------
const int DEFAULT_MOVEMENT_PROPERTIES = MOVE_PROP_SLOWED_BY_WATER | MOVE_PROP_SLOWED_BY_LAVA;


//-----------------------------------------------------------------------------------------------
class Agent : public GameEntity
{
public:
	Agent( GameEntityType type );
	Agent( const Agent& copyFrom );
	~Agent();
	virtual float Update();
	virtual void Render() const;
	virtual bool IsReadyToUpdate() const { return true; };
	bool TestOneStep( const IntVector2& pos );
	bool MoveOneStep( const IntVector2& pos );
	bool TestAndMoveOneStep( Direction direction );
	bool HasAnyLineOfSight( const Vector2& startPosition, const Vector2& endPosition,
		bool updateCellVisibility );
	void CalculateFoV();
	virtual XMLNode SaveToXML() { return XMLNode(); }

	std::vector< Agent* > GetVisibleAgents() const { return m_visibleAgents; }
	std::vector< Item* > GetVisibleItems() const { return m_visibleItems;  }
	int GetFactionID() const { return m_faction.GetFactionID(); }
	void AdjustFactionStatus( Agent* instigator, FactionAction instigatorAction ) { m_faction.AdjustFactionStatus( instigator, instigatorAction ); }
	Agent* GetTargetEnemy() { return m_targetEnemy; }
	Agent* GetTargetAlly() { return m_targetAlly;  }
	int GetAgentMaxHealthInt() { return m_maxHealth; }
	int GetAgentAttackInt() { return m_damage; }
	Faction GetFaction() { return m_faction; }

	// Items
	virtual void EquipBestItemOfType( ItemType type );
	virtual bool IsItemBetterThanEquipped( Item* targetItem );
	virtual Item* GetBestWeapon( std::vector<Item*> weapons );
	virtual Item* GetBestArmor( std::vector<Item*> armors );
	virtual std::vector<Item*> GetAllInventoryItemsOfType( ItemType type );
	virtual void EquipItem( Item* itemToEquip );
	virtual void AddItemToInventory( Item* itemToAdd );
	virtual void RemoveItemFromInventory( Item* itemToRemove );
	virtual void DropItemsOnDeath();
	virtual void DropItemFromInventory( Item* item );
	virtual void DropAllInventory();
	virtual void DropAllEquipment();
	virtual EquipmentSlot GetEquipmentSlotForItem( Item* item );
	int GetWeaponDamage() const;
	int GetArmorAmount() const;
	int TakeDamage( int damageAmount );
	virtual void UseEquippedPotion();
	virtual void AddHealth( int healAmount );

	// Terrain
	int TakeLavaDamage();
	void SetTurnDuration();
	int GetMovementProperties() const { return m_movementProperties; }

public:
	Pathfinder* m_pathfinder;

protected:
	int m_maxHealth;
	int m_damage;
	int m_defense;
	std::vector< Agent* > m_visibleAgents;
	std::vector< Item* > m_visibleItems;
	Faction m_faction;
	Agent* m_targetEnemy;
	Agent* m_targetAlly;
	float m_initialTurnDuration;
	float m_turnDuration;
	std::vector< Item* > m_inventory;
	std::vector< Item* > m_equipment;
	int m_movementProperties = 0;
};