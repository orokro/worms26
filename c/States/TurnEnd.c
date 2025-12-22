/* ----------------------------------------------------------------------------------------
	 TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +
   ---------------------------------------------------------------------------------------- */

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
	
	// the game
	Draw_renderGame();
	
	if(Game_timer<-1*TIME_MULTIPLIER)
		Game_changeMode(gameMode_Death);
}

/**
	Called on the first-frame when the Games state machine leaves TurnEnd mode.
*/
static void TurnEnd_exit()
{
	
}
