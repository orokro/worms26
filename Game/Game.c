// C Source File
// Created 11/12/2017; 4:26:32 PM

#include "../Headers/System/Main.h"

// declare our external/global variables
char Game_mode = gameMode_WormSelect;
char Game_previousMode = gameMode_WormSelect;
short Game_timer = 0;
short Game_retreatTimer = 5;
short Game_graceTimer = 5;
long Game_suddenDeathTimer = (long)((long)((long)60*(long)TIME_MULTIPLIER) * (long)10);
char Game_waterLevel = 0;
char Game_currentTeam = 1;
char Game_currentWormUp[2] = {0, 0};
short Game_cursorX = 0;
short Game_cursorY = 0;
char Game_cursirDir = 0;
short Game_xMarkSpotX = 0;
short Game_xMarkSpotY = 0;
char Game_xMarkPlaced = FALSE;
char Game_xMarkAllowedOverLand = TRUE;
char Game_cursorEndTurn = FALSE;
	
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
void WormSelect_enter();
void WormSelect_update();
void WormSelect_exit();
void Turn_enter();
void Turn_update();
void Turn_exit();
void WeaponSelect_enter();
void WeaponSelect_update();
void WeaponSelect_exit();
void Pause_enter();
void Pause_update();
void Pause_exit();
void Cursor_enter();
void Cursor_update();
void Cursor_exit();
void TurnEnd_enter();
void TurnEnd_update();
void TurnEnd_exit();
void Death_enter();
void Death_update();
void Death_exit();
void AfterTurn_enter();
void AfterTurn_update();
void AfterTurn_exit();
void GameOver_enter();
void GameOver_update();
void GameOver_exit();

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
void gameTimers();
void gameUpdates();
void startSuddenDeath();

// decrement's all Game Timers
void gameTimers()
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
	else
		Game_timer--;
		
	// if we hit zero, end the turn!
	// note the states after turn will keep decremending the timers, so this will go negative
	// we only want to end the turn on the frame that it hit 0
	if(Game_timer==0)
		Game_changeMode(gameMode_TurnEnd);
}


// this handles all the updates for the Game mode.
// this is not called during the pause menu, but everywhere else, mostly yes
void gameUpdates()
{
	// decrease game timers
	gameTimers();
	
	// update OilDrums, Crates, Mines, Weapons
	if(OilDrum_active)
		OilDrums_update();

	if(Crate_active>0)
		Crates_update();
	
	if(Mine_active>0)
		Mines_update();
		
	if(Weapon_active>0)
		Weapons_update();
	
	// update explosions
	// NOTE: this comes last because after an explosion has had its first frame
	// it disables that bit... gotta make sure everyone else on this frame has a
	// chance to see it
	Explosion_update();
}


// sets all the worm's health to 1
// we don't need a separate variable to know that Sudden Death is on, we can always test against
// the timer variable being less than 0
void startSuddenDeath()
{
	// we don't even need to test for active worms, just make 'em all 1hp
	short i=0;
	for(i=0; i<16; i++)
		Worm_health[i]=1;
}

/*
	Due to TIGCC's weird path rules, these files are stored in the Game/States folder on DISK
	but in TIGCC IDE they are under Header Files/States
	
	The only way I was able to include them was to move them under Headers...
	
	At least in Headers they wont compile on their own, since we just want them included in this file.
	
	Anyway, below, each of the states for the state machine is defined, and any methods or variables they need
	will be defined in their file.
	
	Each state has a State_enter, State_update, and State_exit method. Yay, state-machines!
*/
#include "..\States\WormSelect.c"
#include "..\States\Turn.c"
#include "..\States\WeaponSelect.c"
#include "..\States\Pause.c"
#include "..\States\Cursor.c"
#include "..\States\TurnEnd.c"
#include "..\States\Death.c"
#include "..\States\AfterTurn.c"
#include "..\States\GameOver.c"