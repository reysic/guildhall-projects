#include "MapCell.hpp"


//-----------------------------------------------------------------------------------------------
MapCell::MapCell( CellType cellType )
	: m_type( cellType)
	, m_isClosed( false )
{
}


//-----------------------------------------------------------------------------------------------
MapCell::~MapCell()
{
}


//-----------------------------------------------------------------------------------------------
float MapCell::GetGCostForCellType( EntityType antType, CellType cellType )
{
	switch ( antType )
	{
		case ENTITY_TYPE_SOLDIER:
		{
			switch ( cellType )
			{
				case CELL_TYPE_AIR:			return 1.0f;
				case CELL_TYPE_DIRT:		return 0.5f;
				case CELL_TYPE_STONE:		return 999.0f;
				case CELL_TYPE_FOOD:		return 1.0f;
				case CELL_TYPE_UNKNOWN:		return 0.5f;
				default:					return 999.0f;
			}
			break;
		}
		case ENTITY_TYPE_SCOUT:
		{
			switch ( cellType )
			{
				case CELL_TYPE_AIR:			return 1.0f;
				case CELL_TYPE_DIRT:		return 0.5f;
				case CELL_TYPE_STONE:		return 999.0f;
				case CELL_TYPE_FOOD:		return 1.0f;
				case CELL_TYPE_UNKNOWN:		return 0.5f;
				default:					return 999.0f;
			}
			break;
		}
		case ENTITY_TYPE_WORKER:
		{
			switch ( cellType )
			{
				case CELL_TYPE_AIR:			return 1.0f;
				case CELL_TYPE_DIRT:		return 2.0f;
				case CELL_TYPE_STONE:		return 999.0f;
				case CELL_TYPE_FOOD:		return 2.0f;
				case CELL_TYPE_UNKNOWN:		return 2.0f;
				default:					return 999.0f;
			}
			break;
		}
		case ENTITY_TYPE_QUEEN:
		{
			switch ( cellType )
			{
				case CELL_TYPE_AIR:			return 1.0f;
				case CELL_TYPE_DIRT:		return 2.0f;
				case CELL_TYPE_STONE:		return 999.0f;
				case CELL_TYPE_FOOD:		return 2.0f;
				case CELL_TYPE_UNKNOWN:		return 2.0f;
				default:					return 999.0f;
			}
			break;
		}
		default:
		{
			switch ( cellType )
			{
				case CELL_TYPE_AIR:			return 1.0f;
				case CELL_TYPE_DIRT:		return 2.0f;
				case CELL_TYPE_STONE:		return 999.0f;
				case CELL_TYPE_FOOD:		return 2.0f;
				case CELL_TYPE_UNKNOWN:		return 2.0f;
				default:					return 999.0f;
			}
			break;
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool MapCell::IsCellTypeSolid( CellType cellType )
{
	switch ( cellType )
	{
		// #TODO: Verify these are the right values
		case CELL_TYPE_AIR:			return false;
		case CELL_TYPE_DIRT:		return false;
		case CELL_TYPE_STONE:		return true;
		case CELL_TYPE_FOOD:		return false;
		case CELL_TYPE_UNKNOWN:		return false;
		default:					return true;
	}
}
