
/* ----------------------------------------------------------------------------------------
	 WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SELECT +++ WORM SEL
   ---------------------------------------------------------------------------------------- */

/**
 * Renders the 3 sprites used to animate the window meter, for the current wind level.
 *
 * Should only be called once at the begininng of the turn.
*/
void renderWindSprites()
{
	// can't be static cuz we'll shift these every time we run this code
	unsigned long arrows[] = {
		0b01001001001001001001001001001001,
		0b11011011011011011011011011011011};
	
	// if the wind is going to the right, we can switch the facing direction of our arrows by shifting the top row left one:
	if(Game_wind>0)
		arrows[0] = arrows[0]<<1;
		
	// build a bitmask that represents the wind strenth, starting from the middle, 16, and left or right if its left wind or right wind...
	unsigned long windBar = 0;
	short i;
	for(i=0; i<abs(Game_wind); i++)
		windBar = (unsigned long)((unsigned long)((Game_wind>0) ? windBar>>1 : windBar<<1) | 0b00000000000000001000000000000000);

	// now we can loop to copy our bar to each sprite frame, and use AND to remove the triangles from each frame
	short f;
	for(f=0; f<3; f++)
	{
		// loop over all three pixels of the sprite
		for(i=0; i<3; i++)
			windSprites[f][i] = (windBar & ~arrows[i%2]);
		
		// shift the arrows each frame in the direction of the wind
		arrows[0] = (unsigned long)((Game_wind>0) ? arrows[0]>>1 : arrows[0]<<1);
		arrows[1] = (unsigned long)((Game_wind>0) ? arrows[1]>>1 : arrows[1]<<1);
	}// next f
	
}


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
	// it's a new turn..
	Game_turn++;
	
	// toggle teams
	Game_currentTeam = (Game_currentTeam==1 ? 0 : 1);
	
	// when the team changes, temporarily (maybe perminantly) we will reset the weapon
	// for debug / sanity check
	Game_currentWeaponSelected = -1;
	Game_currentWeaponProperties = 0;
	
	// pick a new wind speed:
	Game_wind = -16+random(32);
	
	// render the new wind sprite set, so the main drawloop doesn't have to ever frame...
	renderWindSprites();
	
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
	if(Match_allowWormSelection && Keys_keyDown(keyWormSelect))
		nextWorm();
		
	// if any key was pressed other than the worm-select key, we exit this mode
	if(Keys_keyDown(keyAny) && Keys_keyDown(keyWormSelect)==FALSE)
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
