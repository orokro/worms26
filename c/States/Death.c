/*
	Death.c
	-------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the Death state machine specifc code.
*/


/**
	Called on the first-frame when the Games state machine is set to Death mode.
*/
static void Death_enter()
{
	Game_timer=-1;
}


/**
	Called every frame that the Games state machine is in Death mode.
*/
static void Death_update()
{
	// All regular game-updates during this mode
	gameUpdates();
	
	// the game
	Draw_renderGame();
	
	if(Game_timer<-1*TIME_MULTIPLIER)
		Game_changeMode(gameMode_AfterTurn);
}


/**
	Called on the first-frame when the Games state machine leaves Death mode.
*/
static void Death_exit()
{
	
}
