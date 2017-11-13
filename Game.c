// C Source File
// Created 11/12/2017; 4:26:32 PM

#include "../MainSubFolder.h"

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
	Game_suddenDeathTimer--;
	
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
	if(Game_timer<=0)
		Game_changeMode(gameMode_TurnEnd);
}


// sets all the worm's health to 1
// we don't need a separate variable to know that Sudden Death is on, we can always test against
// the timer variable being less than 0
void startSuddenDeath()
{
	// we don't even need to test for active worms, just make 'em all 0
	short i=0;
	for(i=0; i<16; i++)
		Worm_health[i]=1;
}


// this handles all the updates for the Game mode.
// this is not called during the pause menu, but everywhere else, mostly yes
void gameUpdates()
{
	// decrease game timers
	gameTimers();
	
	// update OilDrums, Crates, Mines
	OilDrums_update();
	Crates_update();
	
	// update explosions
	// NOTE: this comes last because after an explosion has had its first frame
	// it disables that bit... gotta make sure everyone else on this frame has a
	// chance to see it
	Explosion_update();
}





/* ----------------------------------------------------------------------------------------
	 WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SEL
   ---------------------------------------------------------------------------------------- */
// local prototypes for worm-select methods below
void nextWorm();

void WormSelect_enter()
{
	// toggle teams
	Game_currentTeam = (Game_currentTeam==1 ? 0 : 1);
	
	// select next worm for this team:
	nextWorm();
	
	// set the grace countdown timer and the turn timer:
	// NOTE: for now we're not using real seconds, and instead frames,
	// where ever 100 is an ingame-clock unit. This will be tweaked after
	// the game is done, to get frames-to-seconds more accurate
	Game_graceTimer = 5 * 100;
	Game_timer = Match_turnTime * 100;
}

void WormSelect_update()
{
	
	// if the current match allows worm-selection, and the user pressed
	// the worm select button, we can goto the next-available worm, if any
	if(Match_allowWormSelection==TRUE && Keys_keyDown(keyWormSelect)==TRUE)
		nextWorm();
		
	// if any key was pressed other than the worm-select key, we exit this mode
	if(Keys_keyDown(keyAny)==TRUE && Keys_keyDown(keyWormSelect)==FALSE)
		Game_changeMode(gameMode_Turn);
		
	// All regular game-updates during this mode
	gameUpdates();
}

void WormSelect_exit()
{
	// clear the grace timer, if there was any
	Game_graceTimer = 0;
}

// select the next available worm on the current team
void nextWorm()
{
	// loop till we find a non-dead worm on the current team
	// note, this while would crash the game if a team was entirely dead...
	// but then again, the game should never enter WormSelect mode if one team is eliminated
	while(TRUE)
	{
		// increment the worm index for the current team
		Game_currentWormUp[(short)Game_currentTeam]++;
		
		// wrap if out of bounds for the worms in this round:
		if(Game_currentWormUp[(short)Game_currentTeam] >= Match_wormCount[(short)Game_currentTeam])
			Game_currentWormUp[(short)Game_currentTeam] = 0;
			
		// worms 0-7 are team 0 and 8-15 are team 1, math helps us:
		char teamWorm = (Game_currentTeam * 8) * Game_currentWormUp[(short)Game_currentTeam];
		
		// make sure he is not dead:
		if(Worm_health[(short)teamWorm]>0)
		{
			// set as current worm
			Worm_currentWorm = teamWorm;
			
			// and we out!
			return;
		}
	}// wend
}






/* ----------------------------------------------------------------------------------------
	 TURN +++ TURN +++ TURN +++ TURN +++ TURN +++ TURN +++ TURN +++ TURN +++ TURN +++ TURN ++
   ---------------------------------------------------------------------------------------- */
void Turn_enter()
{
	
}

void Turn_update()
{
	// all key-logic and movement logic for the worm will happen in WormsUpdate
	
	// All regular game-updates during this mode
	gameUpdates();
	
	// if the user pressed escape, we should goto the pause menu
	if(Keys_keyDown(keyEscape)==TRUE)
	{
		Game_previousMode = Game_mode;
		Game_changeMode(gameMode_Pause);
		return;
	}
	
	// if the user pressed weapon select, we should goto the weapons menu
	if(Keys_keyDown(keyWeaponsSelect)==TRUE)
	{
		Game_changeMode(gameMode_WeaponSelect);
		return;
	}
}

void Turn_exit()
{
	// note, this doesn't imply that the turn is over.
	// just that we are switching to it
}









/* ----------------------------------------------------------------------------------------
	 WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT +++ WEAPON SELECT ++
   ---------------------------------------------------------------------------------------- */

// the x/y position of the weapon-select cursor:
char weaponSelectX=0;
char weaponSelectY=0;
char weaponFastMove=0;

void WeaponSelect_enter()
{
	// make sure our fast-move timer starts at 0
	weaponFastMove = 0;
}

void WeaponSelect_update()
{
	// for each cursor key, move the weapon selection position, when its pressed
	if(Keys_keyDown(keyLeft)==TRUE)
		weaponSelectX--;
	else if(Keys_keyDown(keyRight)==TRUE)
		weaponSelectX++;
		
	if(Keys_keyDown(keyUp)==TRUE)
		weaponSelectY--;
	else if(Keys_keyDown(keyDown)==TRUE)
		weaponSelectY++;
		
	// as long as ANY of the arrow keys are down, increment our fast-move timer:
	if(Keys_keyState(keyCursors)==TRUE)
		weaponFastMove++;
	else
		weaponFastMove = 0;
		
	// if our weapons-fast move timer is over 30 frames, we will auto-move the cursor every 5 frames
	if(weaponFastMove>=30 && weaponFastMove%5==0)
	{
		if(Keys_keyState(keyLeft)==TRUE)
			weaponSelectX--;
		else if(Keys_keyState(keyRight)==TRUE)
			weaponSelectX++;
			
		if(Keys_keyState(keyUp)==TRUE)
			weaponSelectY--;
		else if(Keys_keyState(keyDown)==TRUE)
			weaponSelectY++;
	}
	
	// finally we bound-check our weapon positions:
	if(weaponSelectX<0)
		weaponSelectX=13;
	else if(weaponSelectX>13)
		weaponSelectX=0;
		
	if(weaponSelectY<0)
		weaponSelectY=4;
	else if(weaponSelectY>4)
		weaponSelectY=0;
	
	// if the user pressed ESCAPE we should just exit the weapons menu
	if(Keys_keyDown(keyEscape)==TRUE)
	{
		Game_changeMode(Game_previousMode);
		return;
	}
	
	// if the user pressed the action key, make sure they have stock of that weapon,
	// and if so, select it and exit:
	if(Keys_keyDown(keyAction)==TRUE)
	{
		// TO-DO: implement
	}
	
	// All regular game-updates during this mode
	gameUpdates();
	
	// draw the weapons menu!
	Draw_renderWeaponsMenu(weaponSelectX, weaponSelectY);	
}

void WeaponSelect_exit()
{
	
}







/* ----------------------------------------------------------------------------------------
	 PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE +++ PAUSE ++
   ---------------------------------------------------------------------------------------- */
// there will be TWO menu items for the pause menu: continue (0) and exit (1)
char pauseMenuItem=0;

void Pause_enter()
{
	// always make sure its on continue when we enter
	pauseMenuItem=0;
}

void Pause_update()
{
	/*
	  if either up or down are pressed, we should toggle the pause menu item
	  since there are only two menu items, there is no "direction", if the top item is selected
	  pressing up would loop it to the bottom item, and pressing down would move down to the bottom item
	  thus, we can keys for either key at once by masking them together: keyUp | keyDown and then just
	  toggle the state of the variable
	*/
	if(Keys_keyDown(keyUp | keyDown)==TRUE)
		pauseMenuItem = ((pauseMenuItem==0) ? 1 : 0);
		
	// if the user presses the action key, we should either quit the game, or exit the pause menu
	if(Keys_keyDown(keyAction)==TRUE)
	{
		if(pauseMenuItem==0)
		{
			Game_changeMode(Game_previousMode);
			return;
		}else
		{
			// setting this will exit the next tick of our main loop
			GameRunning=FALSE;
			return;
		}
	}
	
	// note: there are no physics, worms, time updates, etc during pause!
	
	// draw the pause menu!
	Draw_renderPauseMenu(pauseMenuItem);

}

void Pause_exit()
{
	
}







/* ----------------------------------------------------------------------------------------
	 CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ CURSOR +++ 
   ---------------------------------------------------------------------------------------- */
/*
	For reference:
	
	Game_cursorX
	Game_cursorY
	Game_cursorDir
	Game_xMarkSpotX
	Game_xMarkSpotY
	Game_xMarkPlaced
	Game_xMarkAllowedOverLand
*/
char cursorFastMove=0;

void Cursor_enter()
{
	// when we enter this mode, allways put the cursor over the current worm:
	Game_cursorX = Worm_x[(short)Worm_currentWorm];
	Game_cursorY = Worm_y[(short)Worm_currentWorm];
	
	// there doesn't exist an xMarkSpot yet..
	Game_xMarkPlaced = FALSE;
	
	// no time on the fast-move mode yet
	cursorFastMove=0;
	
	// we should set the camera to follow the cursor:
	Camera_focusOn(&Game_cursorX, &Game_cursorY);	
}

void Cursor_update()
{
	
	// All regular game-updates during this mode
	gameUpdates();
	
	// for each cursor key, move the cursor position, when its pressed
	if(Keys_keyDown(keyLeft)==TRUE)
		Game_cursorX--;
	else if(Keys_keyDown(keyRight)==TRUE)
		Game_cursorX++;
		
	if(Keys_keyDown(keyUp)==TRUE)
		Game_cursorY--;
	else if(Keys_keyDown(keyDown)==TRUE)
		Game_cursorY++;
		
	// as long as ANY of the arrow keys are down, increment our fast-move timer:
	if(Keys_keyState(keyCursors)==TRUE)
		cursorFastMove++;
	else
		cursorFastMove = 0;
		
	// if our weapons-fast move timer is over 30 frames, we will auto-move the cursor every 5 frames
	if(cursorFastMove>=30 && cursorFastMove%5==0)
	{
		if(Keys_keyState(keyLeft)==TRUE)
			Game_cursorX--;
		else if(Keys_keyState(keyRight)==TRUE)
			Game_cursorX++;
			
		if(Keys_keyState(keyUp)==TRUE)
			Game_cursorY--;
		else if(Keys_keyState(keyDown)==TRUE)
			Game_cursorY++;
	}
	
	// finally we bound-check our cursor position:
	if(Game_cursorX<0)
		Game_cursorX=0;
	else if(Game_cursorX>319)
		Game_cursorX=319;
		
	if(Game_cursorY<0)
		Game_cursorY=0;
	else if(Game_cursorY>190)
		Game_cursorY=190;
	
	// if the user pressed ESCAPE we should just exit cursor mode
	if(Keys_keyDown(keyEscape)==TRUE)
	{
		Game_changeMode(Game_previousMode);
		return;
	}
	
	// if the user pressed the action key, we should test if it's a valid point,
	// place the xMark spot and exit...
	if(Keys_keyDown(keyAction)==TRUE)
	{
		if(Game_xMarkAllowedOverLand==TRUE || Map_testPoint(Game_cursorX, Game_cursorY)==TRUE )
		{
			Game_xMarkSpotX = Game_cursorX;
			Game_xMarkSpotY = Game_cursorY;
			Game_xMarkPlaced = TRUE;
			
			// at this point we should do whatever weaponspawning junk we need to do..
			// TO-DO: implement
			
			// depending on the type of weaponing being used
			// this should either return to TURN mode, or goto TurnEND
			Game_changeMode((Game_cursorEndTurn==TRUE) ? gameMode_TurnEnd : gameMode_Turn );
			return;
		}
	}
	
	// draw the weapons menu!
	Draw_renderWeaponsMenu(weaponSelectX, weaponSelectY);	
}

void Cursor_exit()
{
	// clear the camera from focusing on our cursor
	Camera_clearFocus();
}







/* ----------------------------------------------------------------------------------------
	 TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +
   ---------------------------------------------------------------------------------------- */
void TurnEnd_enter()
{
	
}

void TurnEnd_update()
{
	// All regular game-updates during this mode
	gameUpdates();
}

void TurnEnd_exit()
{
	
}


/* ----------------------------------------------------------------------------------------
	 DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH ++
   ---------------------------------------------------------------------------------------- */
void Death_enter()
{
	
}

void Death_update()
{
	// All regular game-updates during this mode
	gameUpdates();
}

void Death_exit()
{
	
}


/* ----------------------------------------------------------------------------------------
	 AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN ++
   ---------------------------------------------------------------------------------------- */
void AfterTurn_enter()
{
	
}

void AfterTurn_update()
{

}

void AfterTurn_exit()
{
	
}


/* ----------------------------------------------------------------------------------------
	 GAME OVER +++ GAME OVER +++ GAME OVER +++ GAME OVER +++ GAME OVER +++ GAME OVER +++ GAME
   ---------------------------------------------------------------------------------------- */
void GameOver_enter()
{
	
}

void GameOver_update()
{
	
}

void GameOver_exit()
{
	
}


