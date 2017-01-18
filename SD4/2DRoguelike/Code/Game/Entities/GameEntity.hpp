#pragma once

#include <string>

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Tools/Parsers/xmlParser.h"
#include "Game/Maps/Map.hpp"


//-----------------------------------------------------------------------------------------------
enum GameEntityType
{
	GAME_ENTITY_ITEM = 0,
	GAME_ENTITY_NPC,
	GAME_ENTITY_PLAYER,
	GAME_ENTITY_FEATURE,
	NUM_GAME_ENTITIES
};


//-----------------------------------------------------------------------------------------------
class GameEntity
{
public:
	GameEntity( GameEntityType type );
	GameEntity( const GameEntity& copyFrom );
	virtual bool IsPlayer() const { return false; }
	virtual float Update();
	bool IsAlive() { return m_health > 0; };
	virtual void Render() const;
	virtual std::string GetGlyph() const { return m_glyph; }
	int GetEntityID() const { return m_id; }
	virtual void DropItemsOnDeath() {}
	virtual XMLNode SaveToXML() { return XMLNode(); }

public:
	static int s_nextID;

	IntVector2 m_position; // Don't impose that entities must have this
	std::string m_glyph;
	Rgba m_color;
	std::string m_name;
	int m_id;
	GameEntityType m_type;
	Map* m_map; // Don't impose that entities must have this
	int m_health;
};