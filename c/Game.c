// C Source File
// Created 11/12/2017; 4:26:32 PM

#include "Main.h"
#include "Worms.h"

/*
	Game
	----
	
	This file defines the various modes and global settings for the Game.
	
	Including the Game_update() function which updates Game Logic for everything in the Main loop

	Game modes:

	0 - Worm Select Mode
			In this mode, either an spinning choose-arrow or a downward-pointing arrow appears above
			the currently selected worm. This is the beginning of a turn, and the user can select the
			worm they wish to play with, if worm selection is enabled.
			
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

// current game mode!
char Game_mode = gameMode_WormSelect;

// used when certain game modes require the ability to "go back" to the previous mode
char Game_previousMode = gameMode_WormSelect;

// the main timer for a turn
short Game_timer = 0;

// the retreat timer, if any
short Game_retreatTimer = 5;

// the grace-timer for WormSelect mode
short Game_graceTimer = 5;

// sudden death timer - 10 minutes before sudden death begins
long Game_suddenDeathTimer = 60L * TIME_MULTIPLIER * 10;

// water level for sudden death:
char Game_waterLevel = 0;

// current wind speed
char Game_wind = 0;

// the current team, and the current selected worm on each team:
char Game_currentTeam = 1;
char Game_currentWormUp[2] = {0, 0};

// the current cursor position, facing direction, xMark spot position and settings
short Game_cursorX = 0;
short Game_cursorY = 0;
char Game_cursirDir = 0;
short Game_xMarkSpotX = 0;
short Game_xMarkSpotY = 0;
char Game_xMarkPlaced = FALSE;
char Game_xMarkAllowedOverLand = TRUE;
char Game_cursorEndTurn = FALSE;

// for animating jumps
char Game_wormAnimState = ANIM_NONE;
int Game_wormAnimTimer = 0;
char Game_wormFlipStartDir = 0; // 0 = right, 1 = left (matching wormMask)

// for debug, we can stop the time
char Debug_stopTime = FALSE;

/*
	the current set of weapons for the current team, in stacked order.
	
	Note: the weapons all have fixed places in the matrix for selecting weapons.
	
	However, if a worm is out of stock we shouldn't show that weapon. Further,
	we should slide whatever weapons that are instock, below it, up. This is like
	real worms. Thus, this array doesn't store weapon counts, but rather, the
	ID's of the weapons for this current teams inventory.
*/
short Game_weapInventory[5][13];

// current turn number the game is on
char Game_turn = 0;

// the weapon ID of the currently selected weapon for this worms turn
char Game_currentWeaponSelected = -1;

// the bit masked list of properties this weapon has
unsigned short Game_currentWeaponProperties = 0;

// the charge of the current weapon (if it has charge)
unsigned short Game_currentWeaponCharge = 0;

// the angle the current worm is aiming at
char Game_aimAngle=10;

/* 
	For reference, Game modes:
	
	gameMode_WormSelect
	gameMode_Turn
	gameMode_WeaponSelect
	gameMode_Pause
	gameMode_Cursor
	gameMode_TurnEnd
	gameMode_Death
	gameMode_AfterTurn
*/

void gameUpdates();

// function prototypes for our local game-mode logic methods!
/*
	Due to TIGCC's weird path rules, these files are stored in the Game/States folder on DISK
	but in TIGCC IDE they are under Header Files/States
	
	The only way I was able to include them was to move them under Headers...
	
	At least in Headers they wont compile on their own, since we just want them included in this file.
	
	Anyway, below, each of the states for the state machine is defined, and any methods or variables they need
	will be defined in their file.
	
	Each state has a State_enter, State_update, and State_exit method. Yay, state-machines!
*/
#include "States/WormSelect.c"
#include "States/Turn.c"
#include "States/WeaponSelect.c"
#include "States/Pause.c"
#include "States/Cursor.c"
#include "States/TurnEnd.c"
#include "States/Death.c"
#include "States/AfterTurn.c"
#include "States/GameOver.c"


// --------------------------------------------------------------------------------------------------------------------------------------



/*
	This is the MAIN update for the entire game! it all goes down here!

  Note that this is programmed kinda like a state-machine,
  and the Game_mode variable should only be changed with our Game_changeMode() function,
  never directly.
  
  This way we can have enter and exit methods for our modes! nifty!
*/
void Game_update()
{
	// before we do anything else, we should update the states of the keys
	Keys_update();
	
	// always update the camera, even when we arent rendering the game (easier this way)
	Camera_update();
	
	// depending on the current game mode, different logic will be present for each mode
	switch(Game_mode)
	{
		case gameMode_WormSelect:
			WormSelect_update();
			break;
		case gameMode_Turn:
			Turn_update();
			break;
		case gameMode_WeaponSelect:
			WeaponSelect_update();
			break;
		case gameMode_Pause:
			Pause_update();
			break;
		case gameMode_Cursor:
			Cursor_update();
			break;
		case gameMode_TurnEnd:
			TurnEnd_update();
			break;
		case gameMode_Death:
			Death_update();
			break;
		case gameMode_AfterTurn:
			AfterTurn_update();
			break;
		case gameMode_GameOver:
			GameOver_update();
			break;
	}	
}

// changes the game mode!
void Game_changeMode(char newMode)
{
	// call the exit method for the current mode
	switch(Game_mode)
	{
		case gameMode_WormSelect:
			WormSelect_exit();
			break;
		case gameMode_Turn:
			Turn_exit();
			break;
		case gameMode_WeaponSelect:
			WeaponSelect_exit();
			break;
		case gameMode_Pause:
			Pause_exit();
			break;
		case gameMode_Cursor:
			Cursor_exit();
			break;
		case gameMode_TurnEnd:
			TurnEnd_exit();
			break;
		case gameMode_Death:
			Death_exit();
			break;
		case gameMode_AfterTurn:
			AfterTurn_exit();
			break;
		case gameMode_GameOver:
			GameOver_exit();
			break;
	}	
	
	// save our current mode as our previous mode
	Game_previousMode = Game_mode;
	
	// change the game mode officially
	Game_mode = newMode;
	
	// call the enter method for the new mode!
	switch(Game_mode)
	{
		case gameMode_WormSelect:
			WormSelect_enter();
			break;
		case gameMode_Turn:
			Turn_enter();
			break;
		case gameMode_WeaponSelect:
			WeaponSelect_enter();
			break;
		case gameMode_Pause:
			Pause_enter();
			break;
		case gameMode_Cursor:
			Cursor_enter();
			break;
		case gameMode_TurnEnd:
			TurnEnd_enter();
			break;
		case gameMode_Death:
			Death_enter();
			break;
		case gameMode_AfterTurn:
			AfterTurn_enter();
			break;
		case gameMode_GameOver:
			GameOver_enter();
			break;
	}	
}



/* ----------------------------------------------------------------------------------------
	 GLOBAL GAME METHODS +++ GLOBAL GAME METHODS +++ GLOBAL GAME METHODS +++ GLOBAL GAME METH
   ---------------------------------------------------------------------------------------- */

/**
 * Starts the sudden death mode, by setting all worms to 1 HP
 *
 * we don't need a separate variable to know that Sudden Death is on, we can always test against
 * the timer variable being less than 0
 */
static void startSuddenDeath()
{
	// we don't even need to test for active worms, just make 'em all 1hp
	short i=0;
	for(i=0; i<16; i++)
		Worm_health[i]=1;
}

/**
 * Updates all the Games main timers each frame, should be called every frame.
 *
 * Updates the sudden death timer. (Frames till sudden death mode)
 * Updates the grace timer. (Frames till worm-select grace timer is up)
 * Updates the games main turn timer. (Frames till a turn is up)
*/
static void gameTimers()
{
	// always decreates the sudden death timer
	// Game_suddenDeathTimer--;
	
	// note: technically suddenDeathTimer will go negative, but I doubt anyone will play
	// long enough to get a negative overflow without drowning, so no need to check for negatives
	// the benefit of this, is that when it's 0 for just one frame, we can init sudden death
	if(Game_suddenDeathTimer==0)
		startSuddenDeath();
		
	// decrease whichever timer is active
	if(Game_graceTimer>0)
		Game_graceTimer--;
	else if(!Debug_stopTime)
		Game_timer--;
		
	// if we hit zero, end the turn!
	// note the states after turn will keep decremending the timers, so this will go negative
	// we only want to end the turn on the frame that it hit 0
	if(Game_timer==0)
		Game_changeMode(gameMode_TurnEnd);
}

/**
 * Handles all the main updates for the Game mode, should be called every frame.
 * this is not called during the pause menu, but everywhere else, mostly yes
 *
 * That is, weather it's a worm-select mode, turn mode, or whatever,
 * the state of the game needs to be updating everyframe.
*/
void gameUpdates()
{
	// for debug we will have hotkeys to control time
	if(Keys_keyState(keyCameraControl) && Keys_keyDown(key1))
		Debug_stopTime = !Debug_stopTime;
	if(Keys_keyState(keyCameraControl) && Keys_keyDown(key2))
		Game_timer=1;
	if(Keys_keyState(keyCameraControl) && Keys_keyDown(key3))
		Camera_focusOn(&Worm_x[(short)Worm_currentWorm], &Worm_y[(short)Worm_currentWorm]);
		
	// decrease game timers
	gameTimers();
	//return;
	
	// update explosions
	// NOTE: this comes first because after an explosion has had its first frame
	// it disables that bit... gotta make sure everyone else on this frame has a
	// chance to see it
	Explosion_update();
	
	// gravity and physics for worms!
	Worm_update();
	
	// update OilDrums, Crates, Mines, Weapons
	if(OilDrum_active)
		OilDrums_update();

	if(Crate_active)
		Crates_update();
	
	//if(Mine_active)
		Mines_update();
		
	if(Weapon_active)
		Weapons_update();

}

// returns TRUE or FALSE if all game objects (crates, mines, worms, etc) are settled and not doing anything
char Game_allSettled()
{
	/*
		all active items in the game have an unsigned short, where 1 bits are for active items
		all items in the game have an unsigned short if they're settled, where 1 bits are for settled items
		
		there are three scenarios:
			- an item is active and settled
			- an item is active and not settled
			- an item is inactive and settled or not settled
			
		we don't care about inactive items weather they are settled or not.
		
		if we AND the active items and the setttled items, we will get an unsigned short of items that are
		active and settled.
		
		If we AND the active items with the ~ inverse of the result, it should equal 0. This means, all
		active items are settled.
		
		If we do this, and there's a 1 value, that means an active item (1) was ANDED with an unsettled item (0)
		
		Thus, we can do some nice bitwise work and determine if any active items are unsettled
		
		1100	- active
		1010	- settled
		
		0101	- ~settled
		
		1100	- active
		AND
		0101	- ~settled
		----
		0100	- active and unsettled
		
		If the value of all these results OR'd together is not 0, then something somewhere is active
		Explosion_active and Mine_triggered can be OR'd onto the result, since no active explosions or triggered mines are allowed.
	*/
	
	// if this value is NOT 0, something somewhere is active, exploding, or is a triggered mine
	unsigned short unsettled = (Crate_active & (~Crate_settled)) | (Worm_active & (~Worm_settled)) | (OilDrum_active & (~OilDrum_settled)) | (Mine_active & (~Mine_settled)) | Explosion_active | Mine_triggered;
	if(unsettled)
		return FALSE;
	else
		return TRUE;
}












