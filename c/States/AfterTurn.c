/*
	AfterTurn.c
	-----------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the AfterTurn state machine specific code.
*/


// after a crate is settled, wait a bit before moving the camera
char afterCrateFrames;

// during the after turn in sudden death, the water will take 10 frames to advance 10 pixels...
// this is that timer
char waterLevelTimer = 0;

// true once we've attempted to spawn a crate
char crateLogicDone = FALSE;

/**
	Called on the first-frame when the Games state machine is set to AfterTurn mode.
*/
static void AfterTurn_enter()
{
	// for sudden death, we will increase the water level after turns
	waterLevelTimer=10;

	// put game timer in -1 to disable it
	Game_timer=-1;

	// reset our crate logic
	crateLogicDone = FALSE;
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

		// if we haven't run our crate-spawn logic yet, do it now
		if(crateLogicDone==FALSE)
		{	
			crateLogicDone = TRUE;

			// randomly spawn a crate
			if(Crates_spawnCrate())
				afterCrateFrames=11;
		}
		
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
