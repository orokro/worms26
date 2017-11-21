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
	
	// the game
	Draw_renderGame();
	
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
