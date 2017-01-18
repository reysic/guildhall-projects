#include "Game/Entities/Agent.hpp"
#include "Game/Core/CombatSystem.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Entities/Feature.hpp"


//-----------------------------------------------------------------------------------------------
Agent::Agent( GameEntityType type )
	: GameEntity( type )
	, m_pathfinder( nullptr )
	, m_maxHealth( 1 )
	, m_damage( 1 )
	, m_defense( 0 )
	, m_targetEnemy( nullptr )
	, m_targetAlly( nullptr )
	, m_initialTurnDuration( 1.0f )
	, m_turnDuration( 1.0f )
	, m_movementProperties( DEFAULT_MOVEMENT_PROPERTIES )
{
	m_health = m_maxHealth;
	m_equipment.resize( NUM_EQUIPMENT_SLOTS );
	for ( Item* item : m_equipment )
	{
		item = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
Agent::Agent( const Agent& copyFrom )
	: GameEntity( copyFrom )
	, m_pathfinder( copyFrom.m_pathfinder )
	, m_maxHealth( copyFrom.m_maxHealth )
	, m_damage( copyFrom.m_damage )
	, m_defense( copyFrom.m_defense )
	, m_faction( copyFrom.m_faction )
	, m_targetEnemy( copyFrom.m_targetEnemy )
	, m_targetAlly( copyFrom.m_targetAlly )
	, m_initialTurnDuration( copyFrom.m_initialTurnDuration )
	, m_turnDuration( copyFrom.m_turnDuration )
	, m_equipment( copyFrom.m_equipment )
	, m_movementProperties( copyFrom.m_movementProperties )
{
	m_health = m_maxHealth;
}


//-----------------------------------------------------------------------------------------------
Agent::~Agent()
{
	delete m_pathfinder;
	m_pathfinder = nullptr;
	m_visibleAgents.clear();
}


//-----------------------------------------------------------------------------------------------
float Agent::Update()
{
	m_visibleAgents.clear();
	m_visibleItems.clear();
	CalculateFoV();

	return m_turnDuration;
}


//-----------------------------------------------------------------------------------------------
void Agent::Render() const
{
	if ( m_pathfinder )
	{
		m_pathfinder->Render();
	}

	GameEntity::Render();
}


//-----------------------------------------------------------------------------------------------
bool Agent::TestOneStep( const IntVector2& pos )
{
	if ( m_map == nullptr )
	{
		// Agent does not have an associated map
		return false;
	}

	// Is cell attempting to move into stone?
	CellType cellTypeAtPosition = m_map->GetCellTypeAtPosition( pos.x, pos.y );
	if ( cellTypeAtPosition == CELL_TYPE_STONE )
	{
		return false;
	}
	// Is cell attempting to move into water and agent is blocked by water?
	else if ( cellTypeAtPosition == CELL_TYPE_WATER )
	{
		if ( ( m_movementProperties & MOVE_PROP_BLOCKED_BY_WATER ) != 0 )
		{
			return false;
		}
	}
	// Is cell attempting to move into lava and agent is blocked by lava?
	else if ( cellTypeAtPosition == CELL_TYPE_LAVA )
	{
		if ( ( m_movementProperties & MOVE_PROP_BLOCKED_BY_LAVA ) != 0 )
		{
			return false;
		}
	}

	// Does cell attempting to move into contain a feature that blocks movement?
	Cell& cell = m_map->GetCellAtPosition( IntVector2( pos.x, pos.y ) );
	Feature* feature = cell.m_containedFeature;
	if ( feature != nullptr )
	{
		if ( feature->IsBlockingMovement() )
		{
			if ( IsPlayer() )
			{
				g_theGame->PrintMessage( "You run into " + feature->m_name + ".", Rgba::WHITE );
			}
			return false;
		}
	}

	// Does cell attempting to move into already contain another agent?
	int nextCellIndex = m_map->GetVectorIndexForCellLocation( pos );
	if ( m_map->m_cells[ nextCellIndex ].m_containsAgent )
	{
		if ( IsPlayer() )
		{
			AttackData attData;
			attData.attacker = this;
			attData.target = m_map->m_cells[ nextCellIndex ].m_containedAgent;
			attData.chanceToHit = 0.9f; // #TODO: Make this a constant somewhere
			attData.damageDealt = GetWeaponDamage();
			if ( m_damage >= m_map->m_cells[ nextCellIndex ].m_containedAgent->m_health )
			{
				g_theGame->m_playerKills++;
			}
			g_theCombatSystem->PerformAttack( attData );
		}
		return false;
	}
	
	return true;
}


//-----------------------------------------------------------------------------------------------
bool Agent::MoveOneStep( const IntVector2& pos )
{
	// Attempts to do the move

	if ( m_map == nullptr )
	{
		// Agent does not have an associated map
		return false;
	}

	// Is cell attempting to move into stone?
	if ( m_map->GetCellTypeAtPosition( pos.x, pos.y ) == CELL_TYPE_STONE )
	{
		return false;
	}

	// Does cell attempting to move into already contain another agent?
	int nextCellIndex = m_map->GetVectorIndexForCellLocation( pos );
	if ( m_map->m_cells[ nextCellIndex ].m_containsAgent )
	{
		return false;
	}

	// Old cell no longer contains agent
	int currentCellIndex = m_map->GetVectorIndexForCellLocation( m_position );
	m_map->m_cells[ currentCellIndex ].m_containsAgent = false;
	m_map->m_cells[ currentCellIndex ].m_containedAgent = nullptr;

	// Move agent
	m_position = IntVector2( pos.x, pos.y );

	// New cell now contains agent
	m_map->m_cells[ nextCellIndex ].m_containsAgent = true;
	m_map->m_cells[ nextCellIndex ].m_containedAgent = this;

	// Take lava damage if moving into lava cell
	if ( m_map->m_cells[ nextCellIndex ].m_currentType == CELL_TYPE_LAVA )
	{
		TakeLavaDamage();
	}

	SetTurnDuration();

	return true;
}


//-----------------------------------------------------------------------------------------------
bool Agent::TestAndMoveOneStep( Direction direction )
{
	IntVector2 nextPosition = m_position;

	switch ( direction )
	{
		case DIRECTION_NORTH:
		{
			nextPosition.y += 1;
			break;
		}
		case DIRECTION_SOUTH:
		{
			nextPosition.y -= 1;
			break;
		}
		case DIRECTION_EAST:
		{
			nextPosition.x += 1;
			break;
		}
		case DIRECTION_WEST:
		{
			nextPosition.x -= 1;
			break;
		}
		case DIRECTION_NORTHEAST:
		{
			nextPosition.x += 1;
			nextPosition.y += 1;
			break;

		}
		case DIRECTION_NORTHWEST:
		{
			nextPosition.x -= 1;
			nextPosition.y += 1;
			break;
		}
		case DIRECTION_SOUTHEAST:
		{
			nextPosition.x += 1;
			nextPosition.y -= 1;
			break;
		}
		case DIRECTION_SOUTHWEST:
		{
			nextPosition.x -= 1;
			nextPosition.y -= 1;
			break;
		}
		case DIRECTION_INVALID:
		{
			break;
		}
		default:
		{
			break;
		}
	}

	// Test next intended position
	if ( TestOneStep( nextPosition ) )
	{
		// Move to next intended position
		return MoveOneStep( nextPosition );
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
bool Agent::HasAnyLineOfSight( const Vector2& startPosition, const Vector2& endPosition, 
	bool updateCellVisibility )
{
	// Center of agent to center of cell
	if ( m_map->HasLineOfSight( Vector2( startPosition.x + 0.5f, startPosition.y + 0.5f ), 
		Vector2( endPosition.x + 0.5f, endPosition.y + 0.5f ), updateCellVisibility ) )
	{
		return true;
	}

	// Center of agent to bottom left corner of cell
	if ( m_map->HasLineOfSight( Vector2( startPosition.x + 0.5f, startPosition.y + 0.5f ), 
		Vector2( endPosition.x, endPosition.y ), updateCellVisibility ) )
	{
		return true;
	}

	// Center of agent to top left corner of cell
	if ( m_map->HasLineOfSight( Vector2( startPosition.x + 0.5f, startPosition.y + 0.5f ), 
		Vector2( endPosition.x, endPosition.y + 1.0f ), updateCellVisibility ) )
	{
		return true;
	}

	// Center of agent to bottom right corner of cell
	if ( m_map->HasLineOfSight( Vector2( startPosition.x + 0.5f, startPosition.y + 0.5f ), 
		Vector2( endPosition.x + 1.0f, endPosition.y ), updateCellVisibility ) )
	{
		return true;
	}

	// Center of agent to top right corner of cell
	if ( m_map->HasLineOfSight( Vector2( startPosition.x + 0.5f, startPosition.y + 0.5f ), 
		Vector2( endPosition.x + 1.0f, endPosition.y + 1.0f ), updateCellVisibility ) )
	{
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
void Agent::CalculateFoV()
{
	std::vector< Cell > cellsInRadius = m_map->GetNeighborsInRange( m_position, 15 );

	for ( Cell& cell : cellsInRadius )
	{
		if ( m_map->IsPositionValid( cell.m_position ) )
		{
			if ( HasAnyLineOfSight( Vector2( ( float ) m_position.x, ( float ) m_position.y ), 
				Vector2( ( float ) cell.m_position.x, ( float ) cell.m_position.y ), false ) )
			{
				int index = m_map->GetVectorIndexForCellLocation( cell.m_position );

				// Visible agents
				if ( m_map->m_cells[ index ].m_containsAgent )
				{
					Agent* containedAgent = m_map->m_cells[ index ].m_containedAgent;

					// This should never be true. Complain if it is.
					ASSERT_OR_DIE( containedAgent != nullptr, "Contained agent pointer is nullptr!" );
				
					// Don't add yourself to the list of visible agents
					if ( containedAgent != this )
					{
						m_visibleAgents.push_back( containedAgent );
					}
				}

				// Visible items
				if ( m_map->m_cells[ index ].m_containsItem )
				{
					// #TODO: Change to see all items in the cell
					Item* containedItem = m_map->GetFirstItemAtCellPosition( cell.m_position );

					ASSERT_OR_DIE( containedItem != nullptr, "Contained item pointer is nullptr!" );

					m_visibleItems.push_back( containedItem );
				}
			}
		}
	}

	// Cleanup
	cellsInRadius.clear();
}


//-----------------------------------------------------------------------------------------------
void Agent::EquipBestItemOfType( ItemType type )
{
	std::vector<Item*> inventoryItemsOfType = GetAllInventoryItemsOfType( type );

	// Potions
	if ( type == ITEM_TYPE_POTION )
	{
		if ( m_equipment[ EQUIPMENT_SLOT_POTION ] == nullptr )
		{
			// If we have random effect potions eventually, we can't determine a "best" potion. 
			// Just equip one if we don't have one.
			if ( inventoryItemsOfType.size() > 0 )
			{
				Item* itemToEquip = inventoryItemsOfType[ 0 ];
				EquipItem( itemToEquip );
			}

		}
	}
	// Weapons
	else if ( type == ITEM_TYPE_WEAPON )
	{
		Item* bestInventoryWeapon = GetBestWeapon( inventoryItemsOfType );

		if ( bestInventoryWeapon != nullptr )
		{
			if ( m_equipment[ EQUIPMENT_SLOT_WEAPON ] != nullptr )
			{
				// Is this a better weapon than what we're holding?
				if ( m_equipment[ EQUIPMENT_SLOT_WEAPON ]->GetDamageAmount() < bestInventoryWeapon->GetDamageAmount() )
				{
					EquipItem( bestInventoryWeapon );
				}
			}
			else
			{
				EquipItem( bestInventoryWeapon );
			}
		}
	}
	// Armor
	else if ( type == ITEM_TYPE_ARMOR )
	{
		Item* bestInventoryArmor = GetBestArmor( inventoryItemsOfType );

		if ( bestInventoryArmor != nullptr )
		{
			if ( m_equipment[ EQUIPMENT_SLOT_CHEST ] != nullptr )
			{
				// Is this a better armor than what we're wearing?
				if ( m_equipment[ EQUIPMENT_SLOT_CHEST ]->GetArmorAmount() < bestInventoryArmor->GetArmorAmount() )
				{
					EquipItem( bestInventoryArmor );
				}
			}
			else
			{
				EquipItem( bestInventoryArmor );
			}
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Agent::IsItemBetterThanEquipped( Item* targetItem )
{
	ItemType type = targetItem->GetItemType();

	if ( type == ITEM_TYPE_WEAPON )
	{
		if ( m_equipment[ EQUIPMENT_SLOT_WEAPON ] != nullptr )
		{
			// Compare
			return ( targetItem->GetDamageAmount() > m_equipment[ EQUIPMENT_SLOT_WEAPON ]->GetDamageAmount() );
		}
		else
		{
			// No weapon. Target is better.
			return true;
		}
	}
	else if ( type == ITEM_TYPE_ARMOR )
	{
		if ( m_equipment[ EQUIPMENT_SLOT_CHEST ] != nullptr )
		{
			// Compare
			return ( targetItem->GetArmorAmount() > m_equipment[ EQUIPMENT_SLOT_CHEST ]->GetArmorAmount() );
		}
		else
		{
			// No armor. Target is better.
			return true;
		}
	}
	else if ( type == ITEM_TYPE_POTION )
	{
		if ( m_equipment[ ITEM_TYPE_POTION ] != nullptr )
		{
			// Compare
			return ( targetItem->GetHealAmount() > m_equipment[ ITEM_TYPE_POTION ]->GetHealAmount() );
		}
		else
		{
			// No potion. Target is better.
			return true;
		}
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
Item* Agent::GetBestWeapon( std::vector<Item*> weapons )
{
	Item* bestWeapon = nullptr;

	for ( Item* weapon : weapons )
	{
		if ( weapon->GetItemType() == ITEM_TYPE_WEAPON ) // Sanity check
		{
			if ( bestWeapon == nullptr ) // We aren't holding a weapon. Equip what we have.
			{
				bestWeapon = weapon;
			}
			else
			{
				// Is this a better weapon than what we're holding?
				if ( bestWeapon->GetDamageAmount() < weapon->GetDamageAmount() )
				{
					bestWeapon = weapon;
				}
			}
		}
	}

	return bestWeapon;
}


//-----------------------------------------------------------------------------------------------
Item* Agent::GetBestArmor( std::vector<Item*> armors )
{
	Item* bestArmor = nullptr;

	for ( Item* armor : armors )
	{
		if ( armor->GetItemType() == ITEM_TYPE_ARMOR ) // Sanity check
		{
			if ( bestArmor == nullptr )
			{
				bestArmor = armor;
			}
			else
			{
				if ( bestArmor->GetArmorAmount() < armor->GetArmorAmount() )
				{
					bestArmor = armor;
				}
			}
		}
	}

	return bestArmor;
}


//-----------------------------------------------------------------------------------------------
std::vector<Item*> Agent::GetAllInventoryItemsOfType( ItemType type )
{
	std::vector<Item*> outputItems;

	for ( Item* item : m_inventory )
	{
		if ( item->GetItemType() == type )
		{
			outputItems.push_back( item );
		}
	}

	return outputItems;
}


//-----------------------------------------------------------------------------------------------
void Agent::EquipItem( Item* itemToEquip )
{
	// Put this item in that one spot and take that thing and put it back in the holdy bit
	EquipmentSlot thisSlot = GetEquipmentSlotForItem( itemToEquip );

	// Invalid slot, can't equip
	if ( thisSlot == NUM_EQUIPMENT_SLOTS )
	{
		return;
	}

	Item* swappedItem = m_equipment[ thisSlot ];
	if ( swappedItem != nullptr )
	{
		AddItemToInventory( swappedItem );
	}

	m_equipment[ thisSlot ] = itemToEquip;

	if ( m_type == GAME_ENTITY_PLAYER )
	{
		g_theGame->PrintMessage( "You equip " + itemToEquip->m_name + ".", Rgba::BLUE );
	}

	RemoveItemFromInventory( itemToEquip );
}


//-----------------------------------------------------------------------------------------------
void Agent::AddItemToInventory( Item* itemToAdd )
{
	// #TODO: Perhaps enforce inventory capacity
	if ( itemToAdd != nullptr ) // Sanity check
	{
		m_inventory.push_back( itemToAdd );
	}
}


//-----------------------------------------------------------------------------------------------
void Agent::RemoveItemFromInventory( Item* itemToRemove )
{
	if ( itemToRemove == nullptr )
		return;

	for ( unsigned int i = 0; i < m_inventory.size(); i++ )
	{
		if ( m_inventory[ i ] == itemToRemove )
		{
			m_inventory.erase( m_inventory.begin() + i );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Agent::DropItemsOnDeath()
{
	DropAllInventory();
	DropAllEquipment();
}


//-----------------------------------------------------------------------------------------------
void Agent::DropItemFromInventory( Item* item )
{
	m_map->AddItemToCellPosition( m_position, item );
	g_theGame->m_entities.push_back( item );
	RemoveItemFromInventory( item );
}


//-----------------------------------------------------------------------------------------------
void Agent::DropAllInventory()
{
	for ( Item* item : m_inventory )
	{
		m_map->AddItemToCellPosition( m_position, item );
		g_theGame->m_entities.push_back( item );
	}

	m_inventory.clear();
}


//-----------------------------------------------------------------------------------------------
void Agent::DropAllEquipment()
{
	for ( Item* item : m_equipment )
	{
		if ( item == nullptr )
		{
			continue;
		}

		m_map->AddItemToCellPosition( m_position, item );
		g_theGame->m_entities.push_back( item );
		EquipmentSlot slot = GetEquipmentSlotForItem( item );
		m_equipment[ slot ] = nullptr;
	}
}


//-----------------------------------------------------------------------------------------------
EquipmentSlot Agent::GetEquipmentSlotForItem( Item* item )
{
	ItemType type = item->GetItemType();

	switch ( type )
	{
		case ITEM_TYPE_WEAPON:
		{
			return EQUIPMENT_SLOT_WEAPON;
			break;
		}
		case ITEM_TYPE_ARMOR:
		{
			// #TODO: Expand this to support other armor slots
			return EQUIPMENT_SLOT_CHEST;
			break;
		}
		case ITEM_TYPE_POTION:
		{
			return EQUIPMENT_SLOT_POTION;
			break;
		}
		case NUM_ITEM_TYPES:
		{
			return NUM_EQUIPMENT_SLOTS;
			break;
		}
		default:
		{
			return NUM_EQUIPMENT_SLOTS;
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
int Agent::GetWeaponDamage() const
{
	if ( m_equipment[ EQUIPMENT_SLOT_WEAPON ] == nullptr )
	{
		return m_damage;
	}
	else
	{
		return m_equipment[ EQUIPMENT_SLOT_WEAPON ]->GetDamageAmount();
	}
}


//-----------------------------------------------------------------------------------------------
int Agent::GetArmorAmount() const
{
	if ( m_equipment[ EQUIPMENT_SLOT_CHEST ] == nullptr )
	{
		return m_defense;
	}
	else
	{
		return m_equipment[ EQUIPMENT_SLOT_CHEST ]->GetArmorAmount();
	}
}


//-----------------------------------------------------------------------------------------------
// Returns total damage dealt after calculations
int Agent::TakeDamage( int damageAmount )
{
	int totalDamage;

	if ( IsPlayer() && g_theGame->m_godModeEnabled )
	{
		totalDamage = 0;
		return totalDamage;
	}

	totalDamage = damageAmount - GetArmorAmount();

	if ( totalDamage < 0 )
	{
		totalDamage = 0;
	}

	m_health -= totalDamage;
	return totalDamage;
}


//-----------------------------------------------------------------------------------------------
void Agent::UseEquippedPotion()
{
	if ( m_equipment[ EQUIPMENT_SLOT_POTION ] == nullptr )
	{
		return;
	}

	AddHealth( m_equipment[ EQUIPMENT_SLOT_POTION ]->GetHealAmount() );

	delete m_equipment[ EQUIPMENT_SLOT_POTION ];
	m_equipment[ EQUIPMENT_SLOT_POTION ] = nullptr;
}


//-----------------------------------------------------------------------------------------------
void Agent::AddHealth( int healAmount )
{
	// #TODO: Print heal message
	m_health += healAmount;

	if ( m_health > m_maxHealth )
	{
		m_health = m_maxHealth;
	}
}


//-----------------------------------------------------------------------------------------------
int Agent::TakeLavaDamage()
{
	int damageToTake = m_maxHealth / 10;

	if ( IsPlayer() && g_theGame->m_godModeEnabled )
	{
		// Don't take damage
	}
	else
	{
		m_health -= damageToTake;
	}

	if ( IsPlayer() )
	{
		g_theGame->PrintMessage( "Lava burns you for " + std::to_string( damageToTake ) + " damage!", Rgba::REDORANGE );
	}
	else
	{
		g_theGame->PrintMessage( "Lava burns " + m_name + "for " + std::to_string( damageToTake ) + " damage!", Rgba::REDORANGE );
	}

	return damageToTake;
}


//-----------------------------------------------------------------------------------------------
void Agent::SetTurnDuration()
{
	CellType type = m_map->GetCellTypeAtPosition( m_position.x, m_position.y );

	if ( type == CELL_TYPE_WATER && ( m_movementProperties & MOVE_PROP_SLOWED_BY_WATER ) != 0 )
	{
		g_theGame->PrintMessage( m_name + " is slowed by water." );
		m_turnDuration = m_initialTurnDuration + 0.5f;
	}
	else if ( type == CELL_TYPE_LAVA && ( m_movementProperties & MOVE_PROP_SLOWED_BY_LAVA ) != 0 )
	{
		g_theGame->PrintMessage( m_name + " is slowed by lava." );
		m_turnDuration = m_initialTurnDuration + 0.8f;
	}
	else
	{
		m_turnDuration = m_initialTurnDuration;
	}
}
