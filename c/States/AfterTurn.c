/* ----------------------------------------------------------------------------------------
	 AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN +++ AFTER TURN ++
   ---------------------------------------------------------------------------------------- */

// after a crate is settled, wait a bit before moving the camera
char afterCrateFrames;

// during the after turn in sudden death, the water will take 10 frames to advance 10 pixels...
// this is that timer
char waterLevelTimer=0;

/**
	Called on the first-frame when the Games state machine is set to AfterTurn mode.
*/
static void AfterTurn_enter()
{
	// incase of sudden death
	waterLevelTimer=10;
	Game_timer=-1;
	
	// randomly spawn a crate
	if(Crates_spawnCrate())
		afterCrateFrames=10;
	
}

/**
	Called every frame that the Games state machine is in AfterTurn mode.
*/
static void AfterTurn_update()
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
	
	// if everything is settled and nothing is happening, e.g. triggered mines or explosions
	// we can move on to the next mode
	if(Game_allSettled())
	{
		// but not until we waited a bit
		afterCrateFrames--;
		if(afterCrateFrames<=0)
			Game_changeMode(gameMode_WormSelect);
	}
		
}

/**
	Called on the first-frame when the Games state machine leaves AfterTurn mode.
*/
static void AfterTurn_exit()
{
	// at the end of this, no crates can be parachuting till a new one is spawned
	parachuteCrate=-1;
}
