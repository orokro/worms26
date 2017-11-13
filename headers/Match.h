// Header File
// Created 11/12/2017; 5:24:15 PM

/*
	Match
	-----
	
	This header defines the default Match Settings for a Worms round.
	
	Note that, eventually this should read-in a temporary file of settings
	saved by the WWP executable which sets up the game.
*/

// the number of worms on each team, max 8
char Match_wormCount[2] = {6, 6};

// the gravestone icons to use for each time:
char Match_gravestones[2] = {0, 1};

// the heath worms should start at:
char Match_wormStartHealth = 100;

// is worm selection enabled?
char Match_allowWormSelection = TRUE;

// playing with artilary mode? (no movement allowed)
char Match_artilleryMode = FALSE;

// how long are turns, in seconds?
char Match_turnTime = 45;

// mine settings: enabled? fuse length? duds enabled?
char Match_minesEnabled = TRUE;
char Match_mineFuseLength = 3;
char Match_dudMines = TRUE;

// playing with oil barrols?
char Match_oilBarrels = TRUE;

// allow each type of crate?
char Match_toolCratesEnabled = TRUE;
char Match_healthCratesEnabled = TRUE;
char Match_weaponCratesEnabled = TRUE;

// define the weapons each team should start with
short Match_defaultWeapons[5][14] = { 
																			{0,9,9,9,9,1,1,3,5,0,2,0,9,1},
																			{0,1,3,9,9,2,1,3,2,0,1,0,9,1},
																			{0,5,0,9,1,1,0,3,2,0,0,0,3,1},
																			{0,0,2,0,0,0,0,1,2,0,0,0,0,1},
																			{0,0,0,2,9,0,0,0,0,0,0,0,0,1}
																		};