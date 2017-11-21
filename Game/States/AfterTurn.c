/* ----------------------------------------------------------------------------------------
	 AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN ++
   ---------------------------------------------------------------------------------------- */

// during the after turn in sudden death, the water will take 10 frames to advance 10 pixels...
// this is that timer
char waterLevelTimer=0;

void AfterTurn_enter()
{
	// incase of sudden death
	waterLevelTimer=10;
	Game_timer=-1;
}

void AfterTurn_update()
{
	// if sudden death is active, we should increase the water level
	if((Game_suddenDeathTimer<=0) && waterLevelTimer>0)
	{
		Game_waterLevel++;
		waterLevelTimer--;
	}
	
	// the game
	Draw_renderGame();
	
	// All regular game-updates during this mode
	gameUpdates();
	
	if(Game_timer<-2*TIME_MULTIPLIER)
		Game_changeMode(gameMode_WormSelect);
}

void AfterTurn_exit()
{
	
}
