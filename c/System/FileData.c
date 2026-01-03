/*
	FileData.c
	----------

	This file will handle the saving and loading of game data from a file "wormsdat"
*/

// includes
#include "Main.h"
#include "FileData.h"
#include "Match.h"
#include "Worms.h"
#include <stdio.h>
#include <string.h>
#include <statline.h>
#include <kbd.h>


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
	FILE *f = fopen("wormsdat", "rb");

	// If file does not exist, set defaults and try to create it
	if (!f)
	{
		FileData_setGameDefaults();
		FileData_saveData();
		return;
	}

	// Skip the first byte (Status byte for the wrapper program)
	fseek(f, 1, SEEK_SET);

	// Read variables in the exact same order they are saved
	// We check the result of fread to ensure the file isn't corrupt
	size_t result = 0;
	
	result += fread(Match_wormCount, sizeof(char), 2, f);
	result += fread(Match_gravestones, sizeof(char), 2, f);
	
	result += fread(&Match_wormStartHealth, sizeof(unsigned char), 1, f);
	result += fread(&Match_allowWormSelection, sizeof(char), 1, f);
	result += fread(&Match_artilleryMode, sizeof(char), 1, f);
	result += fread(&Match_turnTime, sizeof(char), 1, f);
	result += fread(&Match_strategicPlacement, sizeof(char), 1, f);
	
	result += fread(&Match_minesEnabled, sizeof(char), 1, f);
	result += fread(&Match_mineFuseLength, sizeof(char), 1, f);
	result += fread(&Match_dudMines, sizeof(char), 1, f);
	result += fread(&Match_oilDrumsEnabled, sizeof(char), 1, f);
	
	result += fread(&Match_toolCratesEnabled, sizeof(char), 1, f);
	result += fread(&Match_healthCratesEnabled, sizeof(char), 1, f);
	result += fread(&Match_weaponCratesEnabled, sizeof(char), 1, f);
	
	result += fread(&Match_mapType, sizeof(char), 1, f);
	result += fread(&Match_drawingFlags, sizeof(char), 1, f);
	
	result += fread(Match_defaultWeapons, sizeof(char), 65, f);
	result += fread(Match_teamNames, sizeof(char) * 10, 2, f);
	result += fread(Match_wormNames, sizeof(char) * 12, MAX_WORMS, f);

	fclose(f);

	// If reading failed (corrupt or short file), force defaults
	if (result != 101) {
		FileData_setGameDefaults();
		FileData_saveData();
	}
}


/**
 * @brief Saves our game match settings to a binary data file
 */
void FileData_saveData()
{
	// game executabe cannot write to the data file
}
