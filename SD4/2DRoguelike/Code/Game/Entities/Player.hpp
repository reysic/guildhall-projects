#pragma once

#include "Game/Entities/Agent.hpp"


//-----------------------------------------------------------------------------------------------
enum PlayerAction
{
	PLAYER_ACTION_UNSPECIFIED = 0,
	PLAYER_ACTION_MOVE,
	PLAYER_ACTION_PICK_UP_ITEM,
	PLAYER_ACTION_QUAFF,
	PLAYER_ACTION_ACTIVATE_FEATURE,
	NUM_PLAYER_ACTIONS
};


//-----------------------------------------------------------------------------------------------
class Player : public Agent
{
public:
	Player( GameEntityType type );
	virtual bool IsPlayer() const { return true; }
	virtual float Update();
	virtual void Render() const;
	void RenderEquipment() const;
	void RenderStats() const;
	virtual bool IsReadyToUpdate() const;
	void ProcessInput();
	void UpdateMapCellVisibilityBasedUponPlayerFoV();
	void ExamineFloorForItems();
	virtual XMLNode SaveToXML();

public:
	PlayerAction m_nextAction;
	Direction m_moveDirection;
	int m_numTurns;
	int m_numKills;
};