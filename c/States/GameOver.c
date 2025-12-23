/*
	GameOver.c
	----------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the GameOver state machine specific code.
*/


/**
	Called on the first-frame when the Games state machine is set to GameOver mode.
*/
static void GameOver_enter()
{
	
}


/**
	Called every frame that the Games state machine is in GameOver mode.
*/
static void GameOver_update()
{
	// the game
	Draw_renderGame();
}


/**
	Called on the first-frame when the Games state machine leaves GameOver mode.
*/
static void GameOver_exit()
{
	
}
