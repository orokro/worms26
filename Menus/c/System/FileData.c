/*
	FileData.c
	----------

	This file will handle the saving and loading of game data from a file "wormsdat"
*/

// includes
#include "Main.h"
#include "FileData.h"
#include "State.h"
#include "Match.h"


/**
 * @brief This will set all the defaults for the variables if no file was found to load
 */
void FileData_setGameDefaults()
{
	// the default weapons for each worm in the match
	const char defaultWeaponStock[65] = {
		1,9,9,9,9,1,1,3,5,1,2,1,9,
		1,1,3,9,9,2,1,3,2,1,1,1,9,
		1,5,1,9,1,1,1,3,2,1,1,1,3,
		1,1,2,1,9,1,1,1,2,1,1,1,1,
		1,1,1,2,9,1,1,1,2,1,1,1,1
	};

	// default strings to fall back on
	const char defaultWormNames[MAX_WORMS][12] = {
		"MCGUYVER\0",
		"SLINKYFISH\0",
		"BUB\0",
		"STUMPY\0",
		"MISTER CHEF\0",
		"GERTIE\0",
		"KOWALSKI\0",
		"MURPHY\0",
		"ZULU\0",
		"JEAN STAINS\0",
		"TITCHY\0",
		"MUSCLES\0",
		"THE VERT\0",
		"DARKNET\0",
		"SMOKESTACK\0",
		"ROPESY\0"
	};

	// set our default worm / team settings
	Match_wormCount[0] = 6;
	Match_wormCount[1] = 6;
	Match_gravestones[0] = 0;
	Match_gravestones[1] = 1;

	// set default team names
	strcpy(Match_teamNames[0], "GMILLER");
	strcpy(Match_teamNames[1], "LDEBROUX");

	// copy default worm names
	short i;
	for(i=0; i<MAX_WORMS; i++)
		strcpy(Match_wormNames[i], defaultWormNames[i]);

	// copy weapon stock
	for(i=0; i<65; i++)
		Match_defaultWeapons[i] = defaultWeaponStock[i];

	// set our match general settings
	Match_wormStartHealth = 100;
	Match_allowWormSelection = TRUE;
	Match_strategicPlacement = FALSE;
	Match_turnTime = 45;
	Match_artilleryMode = FALSE;
	
	// set our object settings
	Match_minesEnabled = TRUE;
	Match_mineFuseLength = 3;
	Match_dudMines = TRUE;
	Match_oilDrumsEnabled = TRUE;

	// set our crate settings
	Match_toolCratesEnabled = TRUE;
	Match_healthCratesEnabled = TRUE;
	Match_weaponCratesEnabled = TRUE;

	// save map and drawing flags
	Match_mapType = MAP_SAND;
	Match_drawingFlags = DRAW_CLOUDS | DRAW_LEAVES | DRAW_MOUNTAINS;	
}


/**
 * @brief Loads data from "wormsdat" or creates file if doesn't exit
 */
void FileData_loadData()
{	
	FileData_setGameDefaults();

	// gemini here
	// load data from "wormsdat" file & populate variables variables, like in FileData_setGameDefaults
	// but from the file instead.
	// if the file doesn't exist, call FileData_setGameDefaults() to set defaults
	// and then attempt to save the file with the new defaults
	// if save fails, exit the app with an error message on status bar
	// note: we don't need to load in the first byte, which is only for the other program that reads this file.
}


/**
 * @brief Saves our game match settings to a binary data file
 */
void FileData_saveData()
{
	// gemini here
	// save current data to "wormsdat" file
	// NOTE: the first byte must be 1 if App_exitRequested==TRUE
	// so that WWP knows the game was exited properly
	// if App_exitRequested==FALSE, first byte must be 0
	// if save fails, exit the app with an error message on status bar
}
