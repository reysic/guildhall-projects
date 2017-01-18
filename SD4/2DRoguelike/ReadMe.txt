 _____                        _                   
|  __ \                      | |                  
| |__) |__ _ __ ___ ___ _ __ | |_ _ __ ___  _ __  
|  ___/ _ \ '__/ __/ _ \ '_ \| __| '__/ _ \| '_ \ 
| |  |  __/ | | (_|  __/ |_) | |_| | | (_) | | | |
|_|   \___|_|  \___\___| .__/ \__|_|  \___/|_| |_|
                       | |                        
                       |_|                        

by Jeremy Hicks


//-----------------------------------------------------------------------------------------------
Known Issues
- Continue (C) from the main menu is correctly displayed when a save file is found, but the
  functionality behind it is not implemented, so it does nothing.
- Map data, visibility data, and player data are saved when F6 is pressed, but nothing else.
- Messages haven't been culled based upon player visibility.
- Game has not been balanced, so item attributes and agent attack/defense values are not where
  they should be.
- XML parsing is fragile and will likely fail if formatting differs from my own files.
- Dungeon generator needs improvement to produce more interesting maps and ensure the player is not
  spawned in an inescapable room. I have not had a chance to revisit this generator and improve it
  yet. If you spawn in an inescapable room or trapped by NPCs, press ESC to return to the Pick Map
  Screen and select the Dungeon generator again.
- NPCs attack the player immediately on being in an adjacent cells, which seems unfair.
- Fade to black not implemented.
- Simulation does not continue during Game Over.
- Have encountered infinite loops when multiple enemies surround the player and try to attack.


//-----------------------------------------------------------------------------------------------
Notes
- Worked closely with Nick on this assignment, so implementation is similar or the same in some
  cases. That being said, I feel my understanding of the systems for this assignment is good.
- Features include doors (+), both wooden and portcullis. Features can be activated by pressing A.


//-----------------------------------------------------------------------------------------------
How to Use
Run Perceptron.exe to start the game.

Pressing M toggles showing the entire map.

Pressing G toggles God Mode.

Press F6 saves the current game state.

Standard roguelike keys work for player movement.

Some monsters will use melee attacks on the player (adjacent cells), while other will use ranged
attacks. The player can attack an entity by attempting to move into the entity's cell.

Pressing Q when on the Main Menu closes the game.