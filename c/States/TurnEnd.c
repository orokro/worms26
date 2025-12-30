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
	cameraAutoFocus = TRUE;
	Game_wormAnimState = ANIM_NONE;
}


/**
	Called every frame that the Games state machine is in TurnEnd mode.
*/
static void TurnEnd_update()
{
	// All regular game-updates during this mode
	gameUpdates();

	// allow movement while ending the turn, unless the current worm is dead or if the turn end is locked
	// (it gets locked of the worm takes damage or freezes during their turn)
	if((Worm_isDead & (1<<Worm_currentWorm))==0 && (Game_stateFlags & gs_lockTurnEnd)==0)
		CharacterController_update();
	
	// the game
	Draw_renderGame();
	
	if(Game_timer<-5*TIME_MULTIPLIER)
		Game_changeMode(gameMode_Death);
}


/**
	Called on the first-frame when the Games state machine leaves TurnEnd mode.
*/
static void TurnEnd_exit()
{
	// note, this doesn't imply that the turn is over.
	// just that we are switching to it
	cameraAutoFocus = FALSE;

	// disable the lock turn 
	Game_stateFlags &= ~gs_lockTurnEnd;
}
