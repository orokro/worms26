/*
	FileData.c
	----------
	
	Routines for saving and loading game data
*/

#include "h/Match.h"
#include "h/State.h"
#include <string.h>

#define FILE_NAME "wormsdat"

/**
 * @brief This will set all the defaults for the variables if no file was found to load
 */
void FileData_setGameDefaults()
{
	// Default values taken from Match.c
	Match_wormCount[0] = 6;
	Match_wormCount[1] = 8;

	Match_gravestones[0] = 0;
	Match_gravestones[1] = 1;

	Match_wormStartHealth = 100;
	Match_allowWormSelection = TRUE;
	Match_artilleryMode = FALSE;
	Match_turnTime = 45;
	Match_strategicPlacement = TRUE;
	Match_minesEnabled = TRUE;
	Match_mineFuseLength = 3;
	Match_dudMines = TRUE;
	Match_oilDrumsEnabled = TRUE;
	Match_toolCratesEnabled = TRUE;
	Match_healthCratesEnabled = TRUE;
	Match_weaponCratesEnabled = TRUE;
	Match_mapType = MAP_SAND;
	Match_drawingFlags = DRAW_CLOUDS | DRAW_MOUNTAINS;

	// Default team and worm names
	strcpy(Match_teamNames[0], "GMILLER");
	strcpy(Match_teamNames[1], "LDEBROUX");

	strcpy(Match_wormNames[0], "MCGUYVER");
	strcpy(Match_wormNames[1], "SLINKYFISH");
	strcpy(Match_wormNames[2], "BUB");
	strcpy(Match_wormNames[3], "STUMPY");
	strcpy(Match_wormNames[4], "MISTER CHEF");
	strcpy(Match_wormNames[5], "GERTIE");
	strcpy(Match_wormNames[6], "KOWALSKI");
	strcpy(Match_wormNames[7], "MURPHY");
	strcpy(Match_wormNames[8], "ZULU");
	strcpy(Match_wormNames[9], "JEAN STAINS");
	strcpy(Match_wormNames[10], "TITCHY");
	strcpy(Match_wormNames[11], "MUSCLES");
	strcpy(Match_wormNames[12], "THE VERT");
	strcpy(Match_wormNames[13], "DARKNET");
	strcpy(Match_wormNames[14], "SMOKESTACK");
	strcpy(Match_wormNames[15], "ROPESY");
}


/**
 * @brief Saves our game match settings to a binary data file
 */
void FileData_saveData()
{
    HSym hSym = SymFind(SYMSTR(FILE_NAME));
    if (hSym) {
        SymRemove(hSym);
        HeapFree(hSym);
    }
    
    HANDLE hFile = FCreate(SYMSTR(FILE_NAME), F_WRITE);
    if(hFile == H_NULL) return;

    FWrite(&App_exitRequested, sizeof(char), 1, hFile);
    FWrite(Match_wormCount, sizeof(Match_wormCount), 1, hFile);
    FWrite(Match_gravestones, sizeof(Match_gravestones), 1, hFile);
    FWrite(&Match_wormStartHealth, sizeof(Match_wormStartHealth), 1, hFile);
    FWrite(&Match_allowWormSelection, sizeof(Match_allowWormSelection), 1, hFile);
    FWrite(&Match_artilleryMode, sizeof(Match_artilleryMode), 1, hFile);
    FWrite(&Match_turnTime, sizeof(Match_turnTime), 1, hFile);
    FWrite(&Match_strategicPlacement, sizeof(Match_strategicPlacement), 1, hFile);
    FWrite(&Match_minesEnabled, sizeof(Match_minesEnabled), 1, hFile);
    FWrite(&Match_mineFuseLength, sizeof(Match_mineFuseLength), 1, hFile);
    FWrite(&Match_dudMines, sizeof(Match_dudMines), 1, hFile);
    FWrite(&Match_oilDrumsEnabled, sizeof(Match_oilDrumsEnabled), 1, hFile);
    FWrite(&Match_toolCratesEnabled, sizeof(Match_toolCratesEnabled), 1, hFile);
    FWrite(&Match_healthCratesEnabled, sizeof(Match_healthCratesEnabled), 1, hFile);
    FWrite(&Match_weaponCratesEnabled, sizeof(Match_weaponCratesEnabled), 1, hFile);
    FWrite(&Match_mapType, sizeof(Match_mapType), 1, hFile);
    FWrite(&Match_drawingFlags, sizeof(Match_drawingFlags), 1, hFile);
    FWrite(Match_teamNames, sizeof(Match_teamNames), 1, hFile);
    FWrite(Match_wormNames, sizeof(Match_wormNames), 1, hFile);
    
    FClose(hFile);
}


/**
 * @brief Loads data from "wormsdat" or creates file if doesn't exit
 */
void FileData_loadData()
{
    HSym hSym = SymFind(SYMSTR(FILE_NAME));
    if (!hSym)
    {
        // File does not exist, set defaults and create it
        FileData_setGameDefaults();
        // The user wanted to save if it didn't exist.
        // We set App_exitRequested to FALSE for this initial save.
        App_exitRequested = FALSE;
        FileData_saveData();
        return;
    }
    
    HANDLE hFile = FOpen(SYMSTR(FILE_NAME), F_READ);
    if(hFile == H_NULL)
    {
        // Could not open file, use defaults
        FileData_setGameDefaults();
        return;
    }

    // Skip the exit status byte
    FSeek(hFile, 1, FS_SEEK_SET);

    FRead(Match_wormCount, sizeof(Match_wormCount), 1, hFile);
    FRead(Match_gravestones, sizeof(Match_gravestones), 1, hFile);
    FRead(&Match_wormStartHealth, sizeof(Match_wormStartHealth), 1, hFile);
    FRead(&Match_allowWormSelection, sizeof(Match_allowWormSelection), 1, hFile);
    FRead(&Match_artilleryMode, sizeof(Match_artilleryMode), 1, hFile);
    FRead(&Match_turnTime, sizeof(Match_turnTime), 1, hFile);
    FRead(&Match_strategicPlacement, sizeof(Match_strategicPlacement), 1, hFile);
    FRead(&Match_minesEnabled, sizeof(Match_minesEnabled), 1, hFile);
    FRead(&Match_mineFuseLength, sizeof(Match_mineFuseLength), 1, hFile);
    FRead(&Match_dudMines, sizeof(Match_dudMines), 1, hFile);
    FRead(&Match_oilDrumsEnabled, sizeof(Match_oilDrumsEnabled), 1, hFile);
    FRead(&Match_toolCratesEnabled, sizeof(Match_toolCratesEnabled), 1, hFile);
    FRead(&Match_healthCratesEnabled, sizeof(Match_healthCratesEnabled), 1, hFile);
    FRead(&Match_weaponCratesEnabled, sizeof(Match_weaponCratesEnabled), 1, hFile);
    FRead(&Match_mapType, sizeof(Match_mapType), 1, hFile);
    FRead(&Match_drawingFlags, sizeof(Match_drawingFlags), 1, hFile);
    FRead(Match_teamNames, sizeof(Match_teamNames), 1, hFile);
    FRead(Match_wormNames, sizeof(Match_wormNames), 1, hFile);

    FClose(hFile);
}

