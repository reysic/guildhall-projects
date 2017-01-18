#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Game/Generators/Generator.hpp"
#include "Game/NPCS/NPCFactory.hpp"
#include "Game/Entities/ItemFactory.hpp"
#include "Game/Core/TheGame.hpp"


//-----------------------------------------------------------------------------------------------
std::map< std::string, GeneratorRegistration* >* GeneratorRegistration::s_generatorRegistrationMap = nullptr;


//-----------------------------------------------------------------------------------------------
Map* Generator::CreateInitializedMap( const IntVector2& size, const std::string& name )
{
	Map* initializedMap = new Map( size, name );

	int numCells = size.x * size.y;
	initializedMap->m_cells.resize( numCells );

	// Build the map out of cells
	int cellIndex = 0;

	for ( int cellY = 0; cellY < size.y; cellY++ )
	{
		for ( int cellX = 0; cellX < size.x; cellX++ )
		{
			// Set cell position
			initializedMap->m_cells[ cellIndex ].m_position = IntVector2( cellX, cellY );

			// Set cell type
			initializedMap->m_cells[ cellIndex ].m_currentType = CELL_TYPE_STONE;

			cellIndex++;
		}
	}

	initializedMap->SetSize( size );

	return initializedMap;
}


//-----------------------------------------------------------------------------------------------
void Generator::FinalizeMap( Map* map )
{
	// Make edge cells stone
	for ( Cell& cell : map->m_cells )
	{
		if ( cell.m_position.x == 0 || cell.m_position.x == ( map->m_mapWidth - 1 ) ||
			cell.m_position.y == 0 || cell.m_position.y == ( map->m_mapHeight - 1 ) )
		{
			cell.m_currentType = CELL_TYPE_STONE;
		}
	}

	// Hide non-visible cells
	std::vector< Cell > neighborStoneCells;
	std::vector< Cell > neighborInvalidCells;

	for ( Cell& cell : map->m_cells )
	{
		neighborStoneCells = map->GetNeighborsInRangeOfType( cell.m_position, 1, CELL_TYPE_STONE );
		neighborInvalidCells = map->GetNeighborsInRangeOfType( cell.m_position, 1, CELL_TYPE_INVALID );

		unsigned int totalStoneInvalidCells = neighborStoneCells.size() + neighborInvalidCells.size();
		if ( totalStoneInvalidCells == ( unsigned char ) 8 )
		{
			cell.m_isHidden = true;
		}
		else
		{
			cell.m_isHidden = false;
		}

		neighborStoneCells.clear();
		neighborInvalidCells.clear();
	}

	PopulateMap( map );
}


//-----------------------------------------------------------------------------------------------
IntVector2 Generator::FindValidStartingLocation( Map* map )
{
	UNUSED( map );
	// #TODO: Check if this function is created elsewhere
	// Start on bottom left and move up columns
	return IntVector2( 0, 0 );
}


//-----------------------------------------------------------------------------------------------
void Generator::PopulateMap( Map* map )
{
	SpawnNPCs( map );

	SpawnItems( map );
}


//-----------------------------------------------------------------------------------------------
void Generator::SpawnNPCs( Map* map )
{
	int npcCount = 0;
	int numberToSpawn = 8;
	while ( npcCount < numberToSpawn )
	{
		int npcFactoryIndex = RandomInt( NPCFactory::s_npcFactories->size(), 0 );
		auto npcFactoryIter = NPCFactory::s_npcFactories->begin();
		std::advance( npcFactoryIter, npcFactoryIndex );

		NPC* npc = ( npcFactoryIter )->second->Create();
		npc->m_map = map;
		ASSERT_OR_DIE( SetNPCStartPosition( npc ), "Unable to find valid NPC start position!" );
		g_theGame->m_turnOrderMap.insert( TurnOrderMapPair( g_theGame->m_simulationClock, npc ) );
		g_theGame->m_entities.push_back( npc );
		npcCount++;
	}
}


//-----------------------------------------------------------------------------------------------
void Generator::SpawnItems( Map* map )
{
	std::vector< Item* > potions = ItemFactory::MakeItemsOfType( ITEM_TYPE_POTION, 5 );
	std::vector< Item* > armors = ItemFactory::MakeItemsOfType( ITEM_TYPE_ARMOR, 5 );
	std::vector< Item* > weapons = ItemFactory::MakeItemsOfType( ITEM_TYPE_WEAPON, 5 );

	for ( Item* potion : potions )
	{
		potion->m_map = map;
		ASSERT_OR_DIE( SetItemStartPosition( potion ), "Unable to find valid item start position!" );
		g_theGame->m_entities.push_back( potion );
	}

	for ( Item* armor : armors )
	{
		armor->m_map = map;
		ASSERT_OR_DIE( SetItemStartPosition( armor ), "Unable to find valid item start position!" );
		g_theGame->m_entities.push_back( armor );
	}

	for ( Item* weapon : weapons )
	{
		weapon->m_map = map;
		ASSERT_OR_DIE( SetItemStartPosition( weapon ), "Unable to find valid item start position!" );
		g_theGame->m_entities.push_back( weapon );
	}
}


//-----------------------------------------------------------------------------------------------
// Finds starting position for NPC, picking random cells on the map until one is found that is 
// air and does not contain an agent, item, or feature. Stops searching after 10,000 tries
bool Generator::SetNPCStartPosition( NPC* npc )
{
	bool foundCell = false;
	int loopIterations = 0; // Don't want to search indefinitely

	int xPosition;
	int yPosition;
	IntVector2 cellPosition;

	while ( !foundCell && ( loopIterations < 10000 ) )
	{
		xPosition = RandomInt( npc->m_map->m_mapWidth, 0 );
		yPosition = RandomInt( npc->m_map->m_mapHeight, 0 );
		cellPosition = IntVector2( xPosition, yPosition );

		if ( npc->m_map->GetCellAtPosition( cellPosition ).m_currentType == CELL_TYPE_AIR )
		{
			unsigned int cellIndex = npc->m_map->GetVectorIndexForCellLocation( cellPosition );
			if ( npc->m_map->m_cells[ cellIndex ].m_containsAgent == false &&
				npc->m_map->m_cells[ cellIndex ].m_containsItem == false && 
				npc->m_map->m_cells[ cellIndex ].m_containsFeature == false )
			{
				npc->m_position = npc->m_map->m_cells[ cellIndex ].m_position;
				npc->m_map->m_cells[ cellIndex ].m_containsAgent = true;
				npc->m_map->m_cells[ cellIndex ].m_containedAgent = npc;
				foundCell = true;
			}
		}

		++loopIterations;
	}

	if ( foundCell )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
// Finds starting position for item, picking random cells on the map until one is found that is
// both air and does not contain an agent or feature. Stops searching after 10,000 tries
bool Generator::SetItemStartPosition( Item* item)
{
	bool foundCell = false;
	int loopIterations = 0; // Don't want to search indefinitely

	int xPosition;
	int yPosition;
	IntVector2 cellPosition;

	while ( !foundCell && ( loopIterations < 10000 ) )
	{
		xPosition = RandomInt( item->m_map->m_mapWidth, 0 );
		yPosition = RandomInt( item->m_map->m_mapHeight, 0 );
		cellPosition = IntVector2( xPosition, yPosition );

		if ( item->m_map->GetCellAtPosition( cellPosition ).m_currentType == CELL_TYPE_AIR )
		{
			unsigned int cellIndex = item->m_map->GetVectorIndexForCellLocation( cellPosition );
			if ( item->m_map->m_cells[ cellIndex ].m_containsAgent == false &&
				item->m_map->m_cells[ cellIndex ].m_containsFeature == false )
			{
				item->m_position = item->m_map->m_cells[ cellIndex ].m_position;
				item->m_map->m_cells[ cellIndex ].m_containsItem = true;
				item->m_map->m_cells[ cellIndex ].m_containedItems.push_back( item );
				foundCell = true;
			}
		}

		++loopIterations;
	}

	if ( foundCell )
	{
		return true;
	}
	else
	{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
GeneratorRegistration::GeneratorRegistration( const std::string& name, GeneratorCreationFunc* creationFunc, EnvironmentGenerationProcessCreationFunc environmentGenerationProcessCreationFunc )
	: m_name( name )
	, m_creationFunc( creationFunc )
	, m_environmentGenerationProcessCreationFunc( environmentGenerationProcessCreationFunc )
{
	if ( s_generatorRegistrationMap == nullptr )
	{
		s_generatorRegistrationMap = new std::map< std::string, GeneratorRegistration* >;
	}

	if ( s_generatorRegistrationMap->find( name ) == s_generatorRegistrationMap->end() )
	{
		s_generatorRegistrationMap->insert( std::pair< std::string, GeneratorRegistration* >( name, this ) );
	}
}


//-----------------------------------------------------------------------------------------------
Generator* GeneratorRegistration::CreateGeneratorByName( const std::string& name )
{
	Generator* generator = nullptr;
	GeneratorRegistration* generatorRegistration = nullptr;

	ASSERT_OR_DIE( s_generatorRegistrationMap != nullptr, "Map is empty." );

	std::map< std::string, GeneratorRegistration* >::iterator generationRegistrationIter = s_generatorRegistrationMap->find( name );

	if ( generationRegistrationIter != s_generatorRegistrationMap->end() )
	{
		generatorRegistration = generationRegistrationIter->second;
		generator = ( *generatorRegistration->m_creationFunc )( generatorRegistration->m_name );
	}
	else
	{
		DebuggerPrintf( "Did not find generation function of name %s\n", name );
	}

	return generator;
}


//-----------------------------------------------------------------------------------------------
EnvironmentGenerationProcess* GeneratorRegistration::CreateEnvironmentGenerationProcessByName( const std::string& name, const XMLNode& node )
{
	node;
	name;
	return nullptr;
}
