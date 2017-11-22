
/* ----------------------------------------------------------------------------------------
	 WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SEL
   ---------------------------------------------------------------------------------------- */

// local prototypes for worm-select methods below

/**
	Selects the next available Worm on the current team, during WormSelect mode.
*/
static void nextWorm()
{
	// loop till we find a non-dead worm on the current team
	// note, this while would crash the game if a team was entirely dead...
	// but then again, the game should never enter WormSelect mode if one team is eliminated
	while(TRUE)
	{
		// increment the worm index for the current team
		Game_currentWormUp[(short)Game_currentTeam]++;
		
		// wrap if out of bounds for the worms in this round:
		if(Game_currentWormUp[(short)Game_currentTeam] >= Match_wormCount[(short)Game_currentTeam])
			Game_currentWormUp[(short)Game_currentTeam] = 0;
			
		// worms 0-7 are team 0 and 8-15 are team 1, math helps us:
		char teamWorm = (Game_currentTeam * 8) + Game_currentWormUp[(short)Game_currentTeam];
		
		// make sure he is not dead:
		if(Worm_health[(short)teamWorm]>0)
		{
			// set as current worm
			Worm_currentWorm = teamWorm;
			
			// focus the camera on the selected worm
			Camera_focusOn(&Worm_x[(short)Worm_currentWorm], &Worm_y[(short)Worm_currentWorm]);
			
			// and we out!
			return;
		}
	}// wend
}

/**
	Called on the first-frame when the Games state machine is set to WormSelect mode.
*/
static void WormSelect_enter()
{
	// toggle teams
	Game_currentTeam = (Game_currentTeam==1 ? 0 : 1);
	
	// select next worm for this team:
	nextWorm();
	
	// set the grace countdown timer and the turn timer:
	// NOTE: for now we're not using real seconds, and instead frames,
	// where ever 100 is an ingame-clock unit. This will be tweaked after
	// the game is done, to get frames-to-seconds more accurate
	Game_graceTimer = 5 * TIME_MULTIPLIER;
	Game_timer = Match_turnTime * TIME_MULTIPLIER;
}

/**
	Called every frame that the Games state machine is in WormSelect mode.
*/
static void WormSelect_update()
{
	
	// if the current match allows worm-selection, and the user pressed
	// the worm select button, we can goto the next-available worm, if any
	if(Match_allowWormSelection==TRUE && Keys_keyDown(keyWormSelect)==TRUE)
		nextWorm();
		
	// if any key was pressed other than the worm-select key, we exit this mode
	if(Keys_keyDown(keyAny)==TRUE && Keys_keyDown(keyWormSelect)==FALSE)
		Game_changeMode(gameMode_Turn);
		
	// All regular game-updates during this mode
	gameUpdates();
	
	// the game
	Draw_renderGame();
}

/**
	Called on the first-frame when the Games state machine leaves WormSelect mode.
*/
static void WormSelect_exit()
{
	// clear the grace timer, if there was any
	Game_graceTimer = 0;
}
