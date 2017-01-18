#include "Game/Cells/Cell.hpp"
#include "Game/Entities/Agent.hpp"
#include "Game/Entities/Item.hpp"


//-----------------------------------------------------------------------------------------------
bool Cell::DoesCellTypeBlockMovement( CellType type )
{
	switch ( type )
	{
		case CELL_TYPE_INVALID:
		{
			return true;
			break;
		}
		case CELL_TYPE_AIR:
		{
			return false;
			break;
		}
		case CELL_TYPE_STONE:
		{
			return true;
			break;
		}
		case CELL_TYPE_WATER:
		{
			return false;
			break;
		}
		case NUM_CELL_TYPES:
		{
			return true;
			break;
		}
		default:
		{
			return true;
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Cell::DoesCellTypeBlockLineOfSight( CellType type )
{
	switch ( type )
	{
		case CELL_TYPE_INVALID:
		{
			return true;
			break;
		}
		case CELL_TYPE_AIR:
		{
			return false;
			break;
		}
		case CELL_TYPE_STONE:
		{
			return true;
			break;
		}
		case CELL_TYPE_WATER:
		{
			return false;
			break;
		}
		case NUM_CELL_TYPES:
		{
			return true;
			break;
		}
		default:
		{
			return true;
			break;
		}
	}
}