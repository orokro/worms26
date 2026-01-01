/*
	Game.c
	------
	
	This is the main state-machine file for the entire game.
	
	C Source File
	Created 11/12/2017; 4:26:32 PM

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
			
			During this mode, the user can put the game into alternate modes, such as cursor, weapon select, etc.
			
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
			action on the users part, such as a homing missile, return to Turn Mode.
			
			If the user presses ESCAPE during this mode, it will return to Turn Mode
			
	5 - Turn End Mode
			Turn end mode is activated by a number of conditions:
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
				- No active explosions (worm deaths could explode nearby crates or barrels)
				- No active fire particles
				
				After all the worms have exploded and things have "settled" we can continue to Crate Spawn mode
			
		7 - After Turn Mode
				After a turn is complete, but before the next turn, occasionally crates can spawn.
				
				A random number will be picked, and if within threshold, a random crate will spawn somewhere on the
				map, with a parachute attached.
				
				Before this mode can exit, the crate must land on the ground, and "settle". If the user presses
				the keyAction, the parachute will cancel and regular gravity will apply.
				
				This mode is also where the water will rise if sudden death is enabled
				
		8 - Game Over
				When one team is completely elated, or the game ends in a draw, it automatically goes to Game Over mode.
				
				If there is a winner, it will be announced and their remaining worms shall dance.
				
				If there is a draw, it will be announced.
				
				After the user confirms the announcement with any key, the game will exit.
*/


// includes
#include "Main.h"
#include "Game.h"
#include "Match.h"
#include "Draw.h"
#include "Camera.h"
#include "Keys.h"
#include "Crates.h"
#include "OilDrums.h"
#include "Mines.h"
#include "Worms.h"
#include "Weapons.h"
#include "Explosions.h"
#include "CharacterController.h"
#include "Map.h"
#include "StatusBar.h"

const SCR_RECT fullScreen = {{0, 0, 159, 99}};
 
// vars

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

// which team surrendered, if any
char Game_surrenderedTeam = -1;

// the current cursor position, facing direction, xMark spot position and settings
short Game_cursorX = 0;
short Game_cursorY = 0;
char Game_cursorDir = 0;
short Game_xMarkSpotX = 0;
short Game_xMarkSpotY = 0;
char Game_xMarkAllowedOverLand = TRUE;
short Game_weaponUsesRemaining = -1;

// for animating jumps
char Game_wormAnimState = ANIM_NONE;
int Game_wormAnimTimer = 0;
char Game_wormFlipStartDir = 0; // 0 = right, 1 = left (matching wormMask)

// list of points in the ninja-rope
short Game_ninjaRopeAnchors[32][4];
short Game_ninjaRopeAnchorCount;
short Game_ninjaRopeAngle;
short Game_ninjaRopeRotationDir = 0;
short Game_ninjaRopeRotationSpeed = 0;
short Game_ninjaRopeLength = 0;

// game state flags
unsigned short Game_stateFlags;

// for debug, we can stop the time
char Debug_stopTime = FALSE;

/*
	the current set of weapons for the current team, in stacked order.
	
	Note: the weapons all have fixed places in the matrix for selecting weapons.
	
	However, if a worm is out of stock we shouldn't show that weapon. Further,
	we should slide whatever weapons that are in stock, below it, up. This is like
	real worms. Thus, this array doesn't store weapon counts, but rather, the
	ID's of the weapons for this current teams inventory.
*/
short Game_weapInventory[5][13];

// current turn number the game is on
char Game_turn = 0;

// the weapon ID of the currently and last selected weapon for this worms turn
char Game_lastWeaponSelected = -1;
char Game_currentWeaponSelected = -1;

// the bit masked list of properties this weapon has
unsigned long Game_currentWeaponProperties = 0;

// flags for state like: target is picked, etc
unsigned short Game_currentWeaponState = 0;

// the charge of the current weapon (if it has charge)
unsigned short Game_currentWeaponCharge = 0;

// the angle the current worm is aiming at
char Game_aimAngle=10;

// jet pack fuel
unsigned char Game_jetPackFuel = MAX_JETPACK_FUEL;

char Game_debugFreeze = FALSE;

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


// function prototypes for our local game-mode logic methods!
// this has to be defined before the next set of includes or else they'll try too reference it too early - they're .c includes
void gameUpdates();


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



/**
 * Resets all necessary variables to begin a new round in the game.
*/
void Game_initRound(){

	Worm_active = 0;
	Worm_onGround = 0;
	Worm_settled = 0;
	Worm_isDead = 0;
	Weapon_active = 0;
	Worm_poisoned = 0;
	Explosion_active = 0;
	Crate_active = 0;
	OilDrum_active = 0;
	Mine_active = 0;
	Game_currentTeam = 1;
	Game_waterLevel = 0;
	Game_wormAnimState = ANIM_NONE;
	Game_stateFlags = 0;
	Game_surrenderedTeam = -1;
	Game_weaponUsesRemaining = -1;
	Game_jetPackFuel = 0;

	short i=0;
	for(i=0; i<65; i++)
	{
		Match_teamWeapons[0][i] = Match_defaultWeapons[i];
		Match_teamWeapons[1][i] = Match_defaultWeapons[i];
	}
}


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
	
	// always update the camera, even when we aren't rendering the game (easier this way)
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


/**
 * Changes the Games primary state machine to a new mode.
 *
 * @param newMode a char representing a game mode as defined in the GameModes enum.
*/
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
	// always decreases the sudden death timer
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
	// note the states after turn will keep decrementing the timers, so this will go negative
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

	// status bar update
	StatusBar_update();
	
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


/**
	* Returns TRUE or FALSE if all he game items in the game are settled and not doing anything.
	* 
	* Crates, Mines, Drums, and Worms must be settled on the ground.
	* No active mine timers allowed.
	* No active explosions.
	* No active weapons
	*
	* @return TRUE or FALSE if everything is settled or not
*/
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
		
		if we AND the active items and the settled items, we will get an unsigned short of items that are
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
	unsigned short unsettled =
		(Crate_active & (~Crate_settled))
		|
		(Worm_active & (~Worm_settled))
		|
		(OilDrum_active & (~OilDrum_settled))
		|
		(Mine_active & (~Mine_settled))
		|
		(Weapon_active>0)
		|
		Explosion_active | Mine_triggered;
	if(unsettled)
		return FALSE;
	else
		return TRUE;
}


/**
 * @brief checks if all of one teams worms are dead, or if both teams are dead
 */
char Game_checkWinConditions(){

	// build masks for each team
	const unsigned short team1Mask = 0b0000000011111111;
	const unsigned short team2Mask = 0b1111111100000000;
	
	// get alive worms for each team
	unsigned short team1Alive = Worm_active & ~Worm_isDead & team1Mask;
	unsigned short team2Alive = Worm_active & ~Worm_isDead & team2Mask;
	
	// check if either team is all dead
	// if only one team is dead, the other team wins
	if((team1Alive==0) || (team2Alive==0)){
		Game_changeMode(gameMode_GameOver);
		return TRUE;
	}
	return FALSE;
}


RaycastHit Game_raycast(short originX, short originY, short dirX, short dirY, short testWorms)
{
    RaycastHit result;
    
    // Default result: Hit nothing, end at the edge of the map/screen
    result.hitType = RAY_HIT_NOTHING;
    result.hitIndex = -1;
    result.x = originX;
    result.y = originY;

    // 1. Safety check: prevent infinite loop if direction is 0
    if(dirX == 0 && dirY == 0) return result;

    // 2. Setup Fixed Point Variables
    long curX = (long)originX << FP_SHIFT;
    long curY = (long)originY << FP_SHIFT;
    
    long stepX, stepY;
    
    // 3. Normalize direction to create steps
    // We want the "Longest" axis to step exactly 1.0 (FP_ONE) unit per loop
    // This ensures we never skip a pixel.
    
    long absDX = abs(dirX);
    long absDY = abs(dirY);

    if (absDX >= absDY)
    {
        // X is the major axis
        // If dirX is negative, step is -1.0, else 1.0
        stepX = (dirX < 0) ? -FP_ONE : FP_ONE;
        
        // Y step is the ratio (dirY / dirX) * FP_ONE
        // We calculate: (dirY * FP_ONE) / abs(dirX) 
        // We use abs(dirX) because the sign is handled by the direction of dirY naturally? 
        // Actually simplest math: stepY = (dirY << FP_SHIFT) / abs(dirX);
        // But we must correct the sign relative to the X movement.
        // Let's stick to the ratio method:
        
        stepY = (long)dirY * FP_ONE / absDX;
    }
    else
    {
        // Y is the major axis
        stepY = (dirY < 0) ? -FP_ONE : FP_ONE;
        stepX = (long)dirX * FP_ONE / absDY;
    }

    // 4. The Loop
    // We loop until we hit something or go out of bounds
    // Max loop safety is map width + height (heuristic)
    short loopCount = 0;
    short maxLoops = 300; // Adjust based on max screen diagonal

    while(loopCount < maxLoops)
    {
        // Convert Fixed Point back to Short integer for checking
        short ix = (short)(curX >> FP_SHIFT);
        short iy = (short)(curY >> FP_SHIFT);

        // A. Check Bounds
        if(ix < 0 || ix >= 320 || iy < 0 || iy >= 200) 
		{
			result.x = ix;
			result.y = iy;
			break;
		}

        // B. Check Terrain (Land)
        // Helper macro or function to check your specific buffer
        // Assuming GetPixel returns TRUE if a pixel is set (land)
        if(Map_testPoint(ix, iy)) 
        {
            result.x = ix;
            result.y = iy;
            result.hitType = RAY_HIT_LAND;
            return result;
        }

        // C. Check Worms (Optional)
        // We check bounding box: roughly +/- 4 pixels
        if(testWorms)
        {
            short w;
			unsigned short wormMask = 0;
            for(w = 0; w < MAX_WORMS; w++)
            {
				wormMask = (unsigned short)1<<(w);
				
                // Skip dead or current worm (optional: usually you can't shoot yourself)
                if((Worm_active & wormMask) == 0) 
					continue; 
                if(w == Worm_currentWorm) 
					continue;

                // Simple AABB check
                // This is very fast integer math
                if(ix >= Worm_x[w] - 4 && ix <= Worm_x[w] + 4 &&
                   iy >= Worm_y[w] - 6 && iy <= Worm_y[w] + 5)
                {
                    result.x = ix;
                    result.y = iy;
                    result.hitType = RAY_HIT_WORM;
                    result.hitIndex = w;
                    return result;
                }
            }
        }

        // Increment position
        curX += stepX;
        curY += stepY;
        loopCount++;
    }

    // If we ran out of loops, return the last known position
    result.x = (short)(curX >> FP_SHIFT);
    result.y = (short)(curY >> FP_SHIFT);
    return result;
}
