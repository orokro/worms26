/* ----------------------------------------------------------------------------------------
	 TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +++ TURN END +
   ---------------------------------------------------------------------------------------- */

void TurnEnd_enter()
{
	Game_timer=-1;
}

void TurnEnd_update()
{
	// All regular game-updates during this mode
	gameUpdates();
	
	// the game
	Draw_renderGame();
	
	if(Game_timer<-2*TIME_MULTIPLIER)
		Game_changeMode(gameMode_Death);
}

void TurnEnd_exit()
{
	
}
