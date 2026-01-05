/*
	AfterTurn.c
	-----------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the AfterTurn state machine specific code.
*/


// after a crate is settled, wait a bit before moving the camera
char afterCrateFrames;

// true once we've attempted to spawn a crate
char crateLogicDone = FALSE;

/**
	Called on the first-frame when the Games state machine is set to AfterTurn mode.
*/
static void AfterTurn_enter()
{
	// put game timer in -1 to disable it
	Game_timer=-1;

	// reset our crate logic
	crateLogicDone = FALSE;

	// if we're in sudden death mode, raise the water some
	if(Game_suddenDeathTimer<=0)
	{
		const short weaponID = Weapons_spawn(WWaterRaiser, 160, (196-Game_waterLevel), 0, 0, 30);
		Camera_focusOn(&Weapon_x[weaponID], &Weapon_y[weaponID]);
		StatusBar_showMessage("Water Level Risen!");
	}

	// if any worms are poisoned, damage them now
	if(Worm_poisoned){
		short i;
		unsigned short mask = 1;
		for(i=0; i<MAX_WORMS; i++){
			if((Worm_poisoned & mask) && (Worm_health[i]>2))
				Worm_setHealth(i, -2, TRUE);
			mask = (unsigned short)(mask<<1);
		}
	}
}


/**
	Called every frame that the Games state machine is in AfterTurn mode.
*/
static void AfterTurn_update()
{
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

			// pick random number 1 in 3 chance to spawn a crate
			if(random(3)==0)
			{
				// randomly spawn a crate
				if(Crates_spawnCrate())
					afterCrateFrames=11;
			}
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
