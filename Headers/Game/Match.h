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
extern char Match_wormCount[2];

// the gravestone icons to use for each time:
extern char Match_gravestones[2];

// the heath worms should start at:
extern char Match_wormStartHealth;

// is worm selection enabled?
extern char Match_allowWormSelection;

// playing with artilary mode? (no movement allowed)
extern char Match_artilleryMode;

// how long are turns, in seconds?
extern char Match_turnTime;

// mine settings: enabled? fuse length? duds enabled?
extern char Match_minesEnabled;
extern char Match_mineFuseLength;
extern char Match_dudMines;

// playing with oil barrols?
extern char Match_oilDrumsEnabled;

// allow each type of crate?
extern char Match_toolCratesEnabled;
extern char Match_healthCratesEnabled;
extern char Match_weaponCratesEnabled;

// define the weapons each team should start with
short Match_defaultWeapons[5][14];
