/*
	PlaceWorms.c
	------------
	
	This file is a snippet that is included raw in Game.c
	
	This handles the PlaceWorms state machine specific code.
*/


/**
	Called on the first-frame when the Games state machine is set to PlaceWorms mode.
*/
static void PlaceWorms_enter()
{
	// when we enter this state, it's either from main.c when we first started the game,
	// or from the cursor state, after a previous worm was placed.

	// to save on rom/ram size, we will re-use the Game_jetPackFuel variable
	// as a counter to know which worm to place. We want to start with 0,
	// so we'll increment it after

	// if all worms have been placed, we can exit this mode immediately
	if (Game_jetPackFuel >= MAX_WORMS)
	{	
		// disable the Match_strategicPlacement value, so the game can proceed normally
		Match_strategicPlacement = FALSE;

		// restore the default weapon count for teleports, because we hackishly overrode it
		Match_teamWeapons[0][WTeleport] = Match_defaultWeapons[WTeleport];
		Match_teamWeapons[1][WTeleport] = Match_defaultWeapons[WTeleport];

		// all worms placed, go to worm select mode
		Game_changeMode(gameMode_WormSelect);
		return;
	}

	// we will alternate between teams
	const char teamToPlace = (Game_jetPackFuel % 2);

	/*
		now we need to get the index of the worm to place, <8 is team 0, >=8 is team 1
		0 - team 0 worm 0 index 0
		1 - team 1 worm 0 index 8
		2 - team 0 worm 1 index 1
		3 - team 1 worm 1 index 9
		...

		Calculate the worm number and the index in the Worm arrays
	*/
	const char wormNumber = Game_jetPackFuel / 2;
	const char wormIndex = wormNumber + (teamToPlace * 8);

	// we can now increment the counter b/c we computed the wormNumber/wormIndex already
	Game_jetPackFuel++;

	// teams can have asymmetric total number of worms, as stored in Match_wormCount
	// so if this wormNumber is out of bounds for this team, we can recursively call
	// this function to try again, until all worms are placed as the break condition
	if (wormNumber >= Match_wormCount[(short)teamToPlace])
	{
		// all worms for this team are placed, try again
		PlaceWorms_enter();
		return;
	}

	// spawn the worm so it's active / defaulted in the arrays
	Worm_spawnWorm(wormIndex);

	// send message to the status bar indicating which team is placing
	char messagebuffer[32];
	sprintf(messagebuffer, "%s: Place Worm %d", Match_teamNames[(short)teamToPlace], wormNumber + 1);
	StatusBar_showMessage(messagebuffer);

	// make this worm the current worm
	Worm_currentWorm = wormIndex;

	// give this team 1 teleport immediately
	Match_teamWeapons[(short)teamToPlace][WTeleport]=50;

	// set the current weapon to teleport so we can 'use' it immediately
	Game_currentWeaponSelected = WTeleport;

	// set flags for cursor mode properly
	Game_xMarkAllowedOverLand = FALSE;
	Game_currentWeaponState = 0;
	Game_currentWeaponProperties |= usesCursor;
	
	// goto cursor mode, which will do a teleport, essentially
	Game_changeMode(gameMode_Cursor);

	// note: when we return from cursor mode, we'll be back in PlaceWorms mode,
	// so this method will run again
}


/**
	Called every frame that the Games state machine is in PlaceWorms mode.
*/
static void PlaceWorms_update()
{
	

}


/**
	Called on the first-frame when the Games state machine leaves PlaceWorms mode.
*/
static void PlaceWorms_exit()
{

}
