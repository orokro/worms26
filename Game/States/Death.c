/* ----------------------------------------------------------------------------------------
	 DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH +++ DEATH ++
   ---------------------------------------------------------------------------------------- */

void Death_enter()
{
	Game_timer=-1;
}

void Death_update()
{
	// All regular game-updates during this mode
	gameUpdates();
	
	// the game
	Draw_renderGame();
	
	if(Game_timer<-2*TIME_MULTIPLIER)
		Game_changeMode(gameMode_AfterTurn);
}

void Death_exit()
{
	
}
