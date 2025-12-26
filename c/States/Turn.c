/*
	Turn.c
	------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the Turn state machine specific code.
*/


/**
	Called on the first-frame when the Games state machine is set to Turn mode.
*/
static void Turn_enter()
{
	cameraAutoFocus = TRUE;
}


/**
	Called every frame that the Games state machine is in Turn mode.
*/
static void Turn_update()
{
	// all key-logic and movement logic for the worm will happen in WormsUpdate

	// All regular game-updates during this mode
	gameUpdates();
	
	// the game
	Draw_renderGame();

	// handle controlling the current worm!
	CharacterController_update();
	
	// if the user pressed escape, we should goto the pause menu
	if(Keys_keyDown(keyEscape))
	{
		Game_previousMode = Game_mode;
		Game_changeMode(gameMode_Pause);
		return;
	}
	
	// if the user pressed weapon select, we should goto the weapons menu
	if(Keys_keyDown(keyWeaponsSelect))
	{
		Game_changeMode(gameMode_WeaponSelect);
		return;
	}
}


/**
	Called on the first-frame when the Games state machine leaves Turn mode.
*/
static void Turn_exit()
{
	// note, this doesn't imply that the turn is over.
	// just that we are switching to it
	cameraAutoFocus = FALSE;
}
