// Header File
// Created 11/12/2017; 4:26:37 PM

/*
	Game
	----
	
	This header file defines the various modes and global settings for the Game.
	
	Including the Game_update() function which updates Game Logic for everything in the Main loop

*/

/*
	Defining the game modes.
	
	0 - Worm Select Mode
			In this mode, either an spinning choose-arrow or a downward-pointing arrow appears above
			the currently selected worm. This is the beginning of a turn, and the user can select the
			worm they wish to playwith, if worm selection is enabled.
			
			A grace-period timer counts down at the beginning of this mode, before the real turn-timer starts.
			
			If a worm is selected before the grace timer finishes, the turn-timer starts immediately.
			
	1 - Turn Mode
			In this mode, the main-turn timer is counting down, if it expires the turn is over.
			
			During this mode, the user can put the game into alternate modes, such as cursor, weapon selecet, etc.
			
			During this mode, weapons can be active.
			
	2 - Weapon Select Mode
			If it's a worms turn mode, and the keyWeaponsMenu is pushed, it will show a weapons menu instead of the game.
			While the weapons menu is open, the timer is still decreasing, and physics are still happening, but
			the scene is not drawn.
			
	3 - Pause Mode
			A pause menu is drawn.
			
			Pause menu DOES stop the clock / physics, and clears the screen until unpaused.
			
			The pause menu allows the user to quit the game as well.
			
	4 - Cursor Mode
			Certain weapons require the user to move a cursor around, such as air-strike, teleport, concrete donkey, etc.
			
			This mode still renders everything and keeps time/physics running.
			
			The user can press 2ND to place a cursor, ending this mode. Depending on the weapon selected, ending
			this mode will either activate the weapon and move to Turn End mode or, if it's a weapon that requires further
			action on the users part, such as a homing missle, return to Turn Mode.
			
			If the user presses ESCAPE during this mode, it will return to Turn Mode
			
	5 - Turn End Mode
			Turn end mode is avtivated by a number of conditions:
				- A worm fires a single shot-weapon
				- A worm uses a weapon such as air-strike or something else
				- A worm fires the last shot in a multi-shot weapon
				- The worm in control is hurt during it's turn (fall damage, mines, etc)
				- The turn timer runes out
				
			Depending on the way the turn was ended, there may-or-may-not be a "retreat" timer set,
			allowing the worm to continue to move, but not select or activate weapons. If the worm
			is hurt during retreat, the retreat timer is cleared and the worm can no longer move.
			
			Turn end mode has a number of complicated conditions for it to exit, which should all be automatic,
			no input from the user.
			
			In order for turn-end to exit, the following conditions must be met:
				- No active weapons objects
				- No active explosions
				- No mines counting down
				- Retreat timer at zero
				- No moving / falling / sliding worms
				- Everything should be "at-rest", meaning no falling / sliding objects anywhere
				- No active fire particles
				
			Once all the above conditions are met, Turn End mode will transition into Death Mode
			
		6 - Death Mode
				After a turn is ended, any non-drown worms with 0 health will take turns exploding into
				their gravestones, from left-most to right-most.
				
				Because worm-death explosions can kill other worms, move them, drown them, etc this mode
				also doesn't end until everything is settled:
				- No active mines
				- No moving / falling / sliding worms or objects
				- No active explosions (worm deaths could explode nearby crates or barrols)
				- No active fire particles
				
				After all the worms have exploded and things have "settled" we can continue to Crate Spawn mode
			
		7 - After Turn Mode
				After a turn is comlpete, but before the next turn, occasionally crates can spawn.
				
				A random number will be picked, and if within threshold, a random crate will spawn somewhere on the
				map, with a parachute attached.
				
				Before this mode can exit, the crate must land on the ground, and "settle". If the user presses
				the keyAction, the parachut will cancel and regular gravity will apply.
				
				This mode is also where the water will rise if sudden death is enabled
				
		8 - Game Over
				When one team is completely elimated, or the game ends in a draw, it automatically goes to Game Over mode.
				
				If there is a winner, it will be announced and their remaining worms shall dance.
				
				If there is a draw, it will be announced.
				
				After the user confirms the annoucement with any key, the game will exit.
*/
enum GameModes {gameMode_WormSelect, gameMode_Turn, gameMode_WeaponSelect, gameMode_Pause, gameMode_Cursor, gameMode_TurnEnd, gameMode_Death, gameMode_AfterTurn, gameMode_GameOver};


// global game properties

// current game mode!
char Game_mode = gameMode_WormSelect;

// used when certain game modes require the ability to "go back" to the previous mode
char Game_previousMode = gameMode_WormSelect;

// the main timer for a turn
short Game_timer=0;

// the retreat timer, if any
short Game_retreatTimer=0;

// the grace-timer for WormSelect mode
short Game_graceTimer=0;

// sudden death timer - 10 minutes before sudden death begins
long Game_suddenDeathTimer=(long)((long)((long)60*(long)100) * (long)10);

// water level for sudden death:
long Game_waterLevel = 0;

// the current team, and the current selected worm on each team:
char Game_currentTeam=1;
char Game_currentWormUp[2] = {0, 0};

// the current cursor position, facing direction, xMark spot position
short Game_cursorX = 0;
short Game_cursorY = 0;
char Game_cursirDir = 0;
short Game_xMarkSpotX = 0;
short Game_xMarkSpotY = 0;
char Game_xMarkPlaced = FALSE;
char Game_xMarkAllowedOverLand = TRUE;
char Game_cursorEndTurn = FALSE;

// function prototypes
void Game_changeMode(char);
void Game_update();
