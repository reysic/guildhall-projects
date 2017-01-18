#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/Entities/Player.hpp"
#include "Game/Core/TheGame.hpp"
#include "Game/Entities/Feature.hpp"


//-----------------------------------------------------------------------------------------------
Player::Player( GameEntityType type )
	: Agent( type )
{
	m_name = "Player";
	m_glyph = '@';
	m_color = Rgba::MAGENTA;
	m_numTurns = 0;
	m_numKills = 0;
	m_faction = *Faction::CreateOrGetFactionByName( "Player" );
	m_health = 20;
	m_maxHealth = 20;
	m_damage = 1;
}


//-----------------------------------------------------------------------------------------------
float Player::Update()
{
	// Call Agent::Update() to populate visible agents vector
	Agent::Update();

	ProcessInput();

	if ( m_nextAction != PLAYER_ACTION_UNSPECIFIED )
	{
		++m_numTurns;
		g_theGame->m_playerTurns++;
	}

	switch ( m_nextAction )
	{
		case PLAYER_ACTION_MOVE:
		{
			IntVector2 intendedPosition = m_position;

			switch ( m_moveDirection )
			{
				case DIRECTION_WEST:
				{
					intendedPosition = IntVector2( m_position.x - 1, m_position.y );
					break;
				}
				case DIRECTION_SOUTH:
				{
					intendedPosition = IntVector2( m_position.x, m_position.y - 1 );
					break;
				}
				case DIRECTION_NORTH:
				{
					intendedPosition = IntVector2( m_position.x, m_position.y + 1 );
					break;
				}
				case DIRECTION_EAST:
				{
					intendedPosition = IntVector2( m_position.x + 1, m_position.y );
					break;
				}
				case DIRECTION_NORTHWEST:
				{
					intendedPosition = IntVector2( m_position.x - 1, m_position.y + 1 );
					break;
				}
				case DIRECTION_NORTHEAST:
				{
					intendedPosition = IntVector2( m_position.x + 1, m_position.y );
					break;
				}
				case DIRECTION_SOUTHWEST:
				{
					intendedPosition = IntVector2( m_position.x - 1, m_position.y - 1 );
					break;
				}
				case DIRECTION_SOUTHEAST:
				{
					intendedPosition = IntVector2( m_position.x + 1, m_position.y - 1 );
					break;
				}
			}

			if ( m_map->IsPositionValid( intendedPosition ) )
			{
				if ( TestOneStep( intendedPosition ) )
				{
					m_map->SetAllCellsAsNotVisible();

					MoveOneStep( intendedPosition );

					UpdateMapCellVisibilityBasedUponPlayerFoV();

					ExamineFloorForItems();
				}
				else
				{
					if ( m_map->DoesCellPositionContainAgent( intendedPosition ) == false )
					{
						// Cell does not contain agent, movement is blocked
						g_theGame->PrintMessage( "Your movement is blocked!" );
					}
				}
			}

			break;
		}
		case PLAYER_ACTION_PICK_UP_ITEM:
		{
			// Add item to inventory
			Item* pickedUpItem = m_map->GetFirstItemAtCellPosition( m_position );
			if ( pickedUpItem != nullptr )
			{
				std::string itemName = pickedUpItem->m_name;
				// Equip item if it's better, excluding potions
				g_theGame->PrintMessage( "You pick up " + itemName + ".", Rgba::CYAN );
				AddItemToInventory( pickedUpItem );
				m_map->RemoveFirstItemAtCellPosition( m_position );
				g_theGame->RemoveGameEntity( pickedUpItem );
				EquipBestItemOfType( pickedUpItem->GetItemType() );
			}
			break;
		}
		case PLAYER_ACTION_QUAFF:
		{
			// Quaff potion if you have one
			if ( m_equipment[ EQUIPMENT_SLOT_POTION] != nullptr )
			{
				std::string potionName = m_equipment[ EQUIPMENT_SLOT_POTION ]->m_name;
				g_theGame->PrintMessage( "You drink " + potionName + ".", Rgba::CYAN );
				UseEquippedPotion();
				// Equip another potion if you have one
				EquipBestItemOfType( ITEM_TYPE_POTION );

			}
			else
			{
				g_theGame->PrintMessage( "You try to quaff but have no potion!" );
			}
			break;
		}
		case PLAYER_ACTION_ACTIVATE_FEATURE:
		{
			std::vector< Cell > nearbyCells = m_map->GetNeighborsInRange( m_position, 1 );

			for ( Cell& cell : nearbyCells )
			{
				if ( cell.m_containsFeature )
				{
					cell.m_containedFeature->ToggleState();
					m_map->SetAllCellsAsNotVisible();
					UpdateMapCellVisibilityBasedUponPlayerFoV();
				}
			}

			break;
		}
		default:
		{
			// Do nothing this step
			break;
		}
	}

	return m_turnDuration;
}


//-----------------------------------------------------------------------------------------------
void Player::Render() const
{
	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	Vector2 offset = m_map->m_renderingOffset;

	IntVector2 currentCellPosition = m_position;

	Vector2 cellPosition( ( float ) currentCellPosition.x * 20.0f, ( float ) currentCellPosition.y * 20.0f );

	g_theRenderer->DrawText2D( offset + cellPosition, m_glyph, 15.0f, m_color, fixedFont );

	RenderEquipment();

	RenderStats();
}


//-----------------------------------------------------------------------------------------------
void Player::RenderEquipment() const
{
	const float leftShift = 300.f;
	const float upShift = 540.f;
	const float nextLineShift = 50.f;
	const float headerTextScale = 17.0f;
	const float equipmentTextScale = 15.0f;

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	Vector2 offset = m_map->m_renderingOffset;
	offset.x -= leftShift;
	offset.y += upShift;

	g_theRenderer->DrawText2D( offset, "Equipment", headerTextScale, Rgba::YELLOW, fixedFont );

	std::string weaponStr = "None";
	std::string armorStr = "None";
	std::string potionStr = "None";

	if ( m_equipment[ EQUIPMENT_SLOT_WEAPON ] )
	{
		weaponStr = m_equipment[ EQUIPMENT_SLOT_WEAPON ]->m_name;
	}

	if ( m_equipment[ EQUIPMENT_SLOT_CHEST ] )
	{
		armorStr = m_equipment[ EQUIPMENT_SLOT_CHEST ]->m_name;
	}

	if ( m_equipment[ EQUIPMENT_SLOT_POTION ] )
	{
		potionStr = m_equipment[ EQUIPMENT_SLOT_POTION ]->m_name;
	}

	Vector2 weaponOffset = Vector2( offset.x, offset.y );
	weaponOffset.y -= nextLineShift;

	Vector2 armorOffset = Vector2( offset.x, offset.y );
	armorOffset.y -= nextLineShift * 2;

	Vector2 potionOffset = Vector2( offset.x, offset.y );
	potionOffset.y -= nextLineShift * 3;

	g_theRenderer->DrawText2D( weaponOffset, "Weapon: " + weaponStr, equipmentTextScale, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( armorOffset, "Armor: " + armorStr, equipmentTextScale, Rgba::WHITE, fixedFont );
	g_theRenderer->DrawText2D( potionOffset, "Potion: " + potionStr, equipmentTextScale, Rgba::WHITE, fixedFont );
}


//-----------------------------------------------------------------------------------------------
void Player::RenderStats() const
{
	const float leftShift = 300.0f;
	const float upShift = 300.0f;
	const float nextLineShift = 50.f;
	const float headerTextScale = 17.0f;
	const float statsTextScale = 15.0f;

	static BitmapFont* fixedFont = BitmapFont::CreateOrGetFont( "Data/Fonts/SquirrelFixedFont.png" );
	Vector2 offset = m_map->m_renderingOffset;
	offset.x -= leftShift;
	offset.y += upShift;

	g_theRenderer->DrawText2D( offset, "Stats", headerTextScale, Rgba::YELLOW, fixedFont );

	Vector2 healthOffset = Vector2( offset.x, offset.y );
	healthOffset.y -= nextLineShift;

	Vector2 turnOffset = Vector2( offset.x, offset.y );
	turnOffset.y -= nextLineShift * 2;

	Vector2 attackOffset = Vector2( offset.x, offset.y );
	attackOffset.y -= nextLineShift * 3;

	Vector2 armorOffset = Vector2( offset.x, offset.y );
	armorOffset.y -= nextLineShift * 4;

	Vector2 godModeOffset = Vector2( offset.x, offset.y );
	godModeOffset.y -= nextLineShift * 5;

	std::string playerHealthString = Stringf( "Health: %i/%i", m_health, m_maxHealth );
	g_theRenderer->DrawText2D( healthOffset, playerHealthString, statsTextScale, Rgba::WHITE, fixedFont );
	std::string playerTurnsString = Stringf( "Turn: %i", m_numTurns );
	g_theRenderer->DrawText2D( turnOffset, playerTurnsString, statsTextScale, Rgba::WHITE, fixedFont );
	std::string playerAttackString = Stringf( "Attack: %i", GetWeaponDamage() );
	g_theRenderer->DrawText2D( attackOffset, playerAttackString, statsTextScale, Rgba::WHITE, fixedFont );
	std::string playerDefenseString = Stringf( "Defense: %i", GetArmorAmount() );
	g_theRenderer->DrawText2D( armorOffset, playerDefenseString, statsTextScale, Rgba::WHITE, fixedFont );
	if ( g_theGame->m_godModeEnabled )
	{
		g_theRenderer->DrawText2D( godModeOffset, "God Mode: ON", statsTextScale, Rgba::WHITE, fixedFont );
	}
	else
	{
		g_theRenderer->DrawText2D( godModeOffset, "God Mode: OFF", statsTextScale, Rgba::WHITE, fixedFont );
	}
}


//-----------------------------------------------------------------------------------------------
bool Player::IsReadyToUpdate() const
{
	if ( m_nextAction == PLAYER_ACTION_UNSPECIFIED )
	{
		return false;
	}
	else
	{
		return true;
	}
}


//-----------------------------------------------------------------------------------------------
void Player::ProcessInput()
{
	// Left
	if ( g_theInputSystem->IsKeyPressed( 'H' ) || g_theInputSystem->IsKeyPressed( VK_LEFT ) )
	{
		m_nextAction = PLAYER_ACTION_MOVE;
		m_moveDirection = DIRECTION_WEST;
	}
	// Down
	else if ( g_theInputSystem->IsKeyPressed( 'J' ) || g_theInputSystem->IsKeyPressed( VK_DOWN ) )
	{
		m_nextAction = PLAYER_ACTION_MOVE;
		m_moveDirection = DIRECTION_SOUTH;
	}
	// Up
	else if ( g_theInputSystem->IsKeyPressed( 'K' ) || g_theInputSystem->IsKeyPressed( VK_UP ) )
	{
		m_nextAction = PLAYER_ACTION_MOVE;
		m_moveDirection = DIRECTION_NORTH;
	}
	// Right
	else if ( g_theInputSystem->IsKeyPressed( 'L' ) || g_theInputSystem->IsKeyPressed( VK_RIGHT ) )
	{
		m_nextAction = PLAYER_ACTION_MOVE;
		m_moveDirection = DIRECTION_EAST;
	}
	// Up + Left (Northwest)
	else if ( g_theInputSystem->IsKeyPressed( 'Y' ) )
	{
		m_nextAction = PLAYER_ACTION_MOVE;
		m_moveDirection = DIRECTION_NORTHWEST;
	}
	// Up + Right (Northeast)
	else if ( g_theInputSystem->IsKeyPressed( 'U' ) )
	{
		m_nextAction = PLAYER_ACTION_MOVE;
		m_moveDirection = DIRECTION_NORTHEAST;
	}
	// Down + Left (Southwest)
	else if ( g_theInputSystem->IsKeyPressed( 'B' ) )
	{
		m_nextAction = PLAYER_ACTION_MOVE;
		m_moveDirection = DIRECTION_SOUTHWEST;
	}
	// Down + Right (Southeast)
	else if ( g_theInputSystem->IsKeyPressed( 'N' ) )
	{
		m_nextAction = PLAYER_ACTION_MOVE;
		m_moveDirection = DIRECTION_SOUTHEAST;
	}
	else if ( g_theInputSystem->IsKeyPressed( VK_OEM_COMMA ) )
	{
		m_nextAction = PLAYER_ACTION_PICK_UP_ITEM;
	}
	else if ( g_theInputSystem->IsKeyPressed( 'Q' ) )
	{
		m_nextAction = PLAYER_ACTION_QUAFF;
	}
	else if ( g_theInputSystem->IsKeyPressed( 'A' ) )
	{
		m_nextAction = PLAYER_ACTION_ACTIVATE_FEATURE;
	}
	else
	{
		m_nextAction = PLAYER_ACTION_UNSPECIFIED;
	}
}


//-----------------------------------------------------------------------------------------------
void Player::UpdateMapCellVisibilityBasedUponPlayerFoV()
{
	std::vector< Agent* > agentsVisibleLastUpdate = m_visibleAgents;
	m_visibleAgents.clear();

	std::vector< Cell > cellsInRadius = m_map->GetNeighborsInRange( m_position, 15 );

	// Mark cells player can see as visible
	for ( Cell& cell : cellsInRadius )
	{
		if ( m_map->IsPositionValid( cell.m_position ) )
		{
			if ( HasAnyLineOfSight( Vector2( ( float ) m_position.x, ( float ) m_position.y ), 
				Vector2( ( float ) cell.m_position.x, ( float ) cell.m_position.y ), true ) )
			{
				int index = m_map->GetVectorIndexForCellLocation( cell.m_position );
				m_map->m_cells[ index ].m_isVisible = true;
				m_map->m_cells[ index ].m_isKnown = true;

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
			}
		}
	}

 	if ( m_visibleAgents.size() > 0 )
 	{
		for ( Agent* newAgent : m_visibleAgents )
		{
			bool agentAlreadySeen = false;

			for ( Agent* oldAgent : agentsVisibleLastUpdate )
			{
				if ( oldAgent == newAgent )
				{
					agentAlreadySeen = true;
					break;
				}
			}
			if ( !agentAlreadySeen )
			{
				g_theGame->PrintMessage( "You see " + newAgent->m_name + "!" );
			}
		}
 	}
}


//-----------------------------------------------------------------------------------------------
void Player::ExamineFloorForItems()
{
	Cell& thisCell = m_map->GetCellAtPosition( m_position );
	if ( thisCell.m_containsItem )
	{
		std::vector<Item*> cellItems = thisCell.m_containedItems;
		std::string firstItemStr = cellItems[ 0 ]->m_name;
		if ( cellItems.size() > 1 )
		{
			std::string remainingItemsStr = std::to_string( cellItems.size() - 1 );
			g_theGame->PrintMessage( "You find a " + firstItemStr + ", and " + remainingItemsStr + " more items.", Rgba::YELLOW );
		}
		else
		{
			g_theGame->PrintMessage( "You find a " + firstItemStr + ".", Rgba::YELLOW );
		}
	}
}


//-----------------------------------------------------------------------------------------------
XMLNode Player::SaveToXML()
{
	XMLNode playerDataNode = XMLNode();
	playerDataNode = playerDataNode.createXMLTopNode( "PlayerData" );

	// Attributes
	playerDataNode.addAttribute( "health", std::to_string( m_health ).c_str() );
	playerDataNode.addAttribute( "maxHealth", std::to_string( m_maxHealth ).c_str() );
	playerDataNode.addAttribute( "damage", std::to_string( m_damage ).c_str() );
	playerDataNode.addAttribute( "defense", std::to_string( m_defense ).c_str() );
	playerDataNode.addAttribute( "position", ( m_position.ToString() ).c_str() );
	playerDataNode.addAttribute( "savedID", std::to_string( m_id ).c_str() );

	// Inventory
	XMLNode inventoryDataNode = XMLNode();
	inventoryDataNode = inventoryDataNode.createXMLTopNode( "Inventory" );

	for ( Item* item : m_inventory )
	{
		if ( item == nullptr )
		{
			continue;
		}
		XMLNode itemDataNode = XMLNode();
		itemDataNode = itemDataNode.createXMLTopNode( "Item" );

		itemDataNode.addAttribute( "name", item->m_name.c_str() );
		itemDataNode.addAttribute( "type", item->GetStringFromItemType( item->GetItemType() ).c_str() );
		inventoryDataNode.addChild( itemDataNode );
	}

	// Equipment
	XMLNode equipmentDataNode = XMLNode();
	equipmentDataNode = equipmentDataNode.createXMLTopNode( "Equipment" );

	if ( m_equipment[ EQUIPMENT_SLOT_WEAPON ] )
	{
		XMLNode weaponDataNode = XMLNode();
		weaponDataNode = weaponDataNode.createXMLTopNode( "Weapon" );

		weaponDataNode.addAttribute( "name", m_equipment[ EQUIPMENT_SLOT_WEAPON ]->m_name.c_str() );
		equipmentDataNode.addChild( weaponDataNode );
	}

	if ( m_equipment[ EQUIPMENT_SLOT_CHEST ] )
	{
		XMLNode armorDataNode = XMLNode();
		armorDataNode = armorDataNode.createXMLTopNode( "Armor" );

		armorDataNode.addAttribute( "name", m_equipment[ EQUIPMENT_SLOT_WEAPON ]->m_name.c_str() );
		equipmentDataNode.addChild( armorDataNode );
	}

	if ( m_equipment[ EQUIPMENT_SLOT_POTION ] )
	{
		XMLNode potionDataNode = XMLNode();
		potionDataNode = potionDataNode.createXMLTopNode( "Potion" );

		potionDataNode.addAttribute( "name", m_equipment[ EQUIPMENT_SLOT_POTION ]->m_name.c_str() );
		equipmentDataNode.addChild( potionDataNode );
	}

	playerDataNode.addChild( inventoryDataNode );
	playerDataNode.addChild( equipmentDataNode );

	return playerDataNode;
}
