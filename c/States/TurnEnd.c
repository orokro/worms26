/*
	TurnEnd.c
	---------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the TurnEnd state machine specific code.
*/


/**
	Called on the first-frame when the Games state machine is set to TurnEnd mode.
*/
static void TurnEnd_enter()
{
	Game_timer=-1;
}


/**
	Called every frame that the Games state machine is in TurnEnd mode.
*/
static void TurnEnd_update()
{
	// All regular game-updates during this mode
	gameUpdates();

	// allow movement while ending the turn, unless the current worm is dead
	if((Worm_isDead & (1<<Worm_currentWorm))==0)
		CharacterController_update();

	// the game
	Draw_renderGame();
	
	if(Game_timer<-10*TIME_MULTIPLIER)
		Game_changeMode(gameMode_Death);
}


/**
	Called on the first-frame when the Games state machine leaves TurnEnd mode.
*/
static void TurnEnd_exit()
{
	
}
