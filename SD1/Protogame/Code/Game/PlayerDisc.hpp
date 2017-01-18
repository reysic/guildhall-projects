#pragma once


//-----------------------------------------------------------------------------------------------
class Vector2; // Forward declaration of Vector2


//-----------------------------------------------------------------------------------------------
const float PLAYER_DISC_SPEED = 160.0f;
const std::string PLAYER_DISC_TEXTURE_FILE = "Data/Images/Disc";
const int PLAYER_DISC_STARTING_HEALTH = 3;


//-----------------------------------------------------------------------------------------------
class PlayerDisc : public Entity
{
public:
	PlayerDisc();
	void Update( float deltaSeconds );
	void Render() const;
	void CheckForScreenEdge();

private:
};