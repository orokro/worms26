/*
	State.h
	-------
	
	This will be the main module for the state machine of the Worms Menu.

	Originally this file was Game.c in the original Worms source code,
	but now that the menus are split into their own app (and aren't a game in and of themselves),
	it's been renamed as State.h since it will just handle the main state machine.
*/
#ifndef GAME_H
#define GAME_H

// game enums
enum MenuModes {
	menuMode_TitleScreen,
	menuMode_MainMenu,
	menuMode_MatchMenu,
	menuMode_WeaponsSettings,
	menuMode_MatchSettings,
	menuMode_TeamSettings,
	menuMode_Credits,
	menuMode_SaveAndExit,
	menuMode_HelpAndSettings,
};

// for making menu screens scale (minimum 4 redraws for safety)
#define STALE 4


// state function prototypes


/**
 * Resets all necessary variables to begin a new round in the game.
*/
extern void State_init();



/**
 * Changes the State machines mode to a new mode.
 *
 * @param newMode a char representing a main state machine mode as defined in the MenuModes enum.
*/
extern void State_changeMode(char);


/**
 * @brief Main state update method for the entire app, should be called once per frame.
 */
extern void State_update();

#endif
