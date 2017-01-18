//-----------------------------------------------------------------------------------------------
#pragma once


//-----------------------------------------------------------------------------------------------
class TheGame;


//-----------------------------------------------------------------------------------------------
extern TheGame* g_theGame;


//-----------------------------------------------------------------------------------------------
class TheGame
{
public:
	TheGame();
	void Input();
	void Update( float deltaSeconds );
	void Render() const;

	int m_litTextureID;

	Texture* m_surfaceColorMapTexture;
	Texture* m_normalDisplacementMapTexture;

private:
};