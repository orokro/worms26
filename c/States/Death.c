/*
	Death.c
	-------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the Death state machine specific code.
*/

// we'll keep the list of worms to kill in this short & blow them up one at a time

// the worm we'll animating killing
static short wormToKill = -1;

// timer for kill animation
short Game_deathTimer = 0;

/**
 * @brief Figures out which worm death to animate right now
 */
static void processWormDeath(){

	// loop through the worms that are 0 health but not dead yet
	short i;
	unsigned short wormMask;
	for(i=0; i<MAX_WORMS; i++)
	{
		wormMask = (1<<i);

		// if this worm is active,  0 health but not dead yet
		if((Worm_active & wormMask) && (Worm_health[i]<=0) && ((Worm_isDead & wormMask)==0))
		{
			// focus on this worm
			Camera_focusOn(&Worm_x[i], &Worm_y[i]);

			// note the worm to kill & reset timer
			wormToKill = i;
			Game_deathTimer = 60;

			// reset minimum game timer
			Game_timer = -1;

			// only process one worm death per frame
			return;
		}

	} // next i

	// if we got this far without finding something to kill, reset the variable
	wormToKill = -1;
}


/**
	Called on the first-frame when the Games state machine is set to Death mode.
*/
static void Death_enter()
{
	Game_timer=-1;

	// reset the worm to kill & timer
	wormToKill = -1;
	Game_deathTimer = 0;

	// we'll manually focus camera for this
	cameraAutoFocus = FALSE;

	// check if we have any worms to process once on enter
	processWormDeath();
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

	// keep checking for worms to kill, while our death timer is still going if we have none
	if(wormToKill == -1)
		processWormDeath();

	// if we have a worm to kill handle it's animation logic
	if(wormToKill!=-1){

		// countdown timer
		Game_deathTimer--;

		// half way thru the timer, spawn explosion & kill the worm
		if(Game_deathTimer==30){

			// spawn explosion at worm position
			Explosion_spawn(Worm_x[wormToKill], Worm_y[wormToKill]+7, 8, 20, TRUE);

			// mark worm as dead, it will now render as a grave stone
			Worm_isDead |= (unsigned short)(1<<wormToKill);
		}

		// if timer is up, process next worm death
		if(Game_deathTimer<=0)
			processWormDeath();
	}


	// don't attempt to exit this state until minimum time passed or if we have any worms to process
	if(Game_timer<-1*TIME_MULTIPLIER && wormToKill==-1){

		// don't exit this mode until everything is settled
		// (this mode can cause explosions and physics to happen)
		if(Game_allSettled())
		{

			// if the game is over, no need to goto AfterTurn
			if(Game_checkWinConditions())
				return;
			
			// otherwise, continue to AfterTurn mode
			Game_changeMode(gameMode_AfterTurn);
		}
	}
}


/**
	Called on the first-frame when the Games state machine leaves Death mode.
*/
static void Death_exit()
{
	// clear camera focus
	Camera_clearFocus();
}
