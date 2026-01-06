/*
	State.c
	--------
	
	This is the main state-machine file for the entire app.
	
	This was originally Game.c in the Worms source code,
	but because the menus are now their own app, this file has been renamed
	to State.c since it handles the main state machine for the menus and game.
*/


// includes
#include "Main.h"
#include "Match.h"
#include "Keys.h"
#include "Draw.h"
#include "SpriteData.h"
#include "State.h"
#include "FileData.h"


// vars

// current menu mode!
char Menu_mode = menuMode_TitleScreen;

// used when certain state modes require the ability to "go back" to the previous mode
char Menu_previousMode = menuMode_TitleScreen;

// reusable vars for our main menu system
char screenIsStale = TRUE;
char menuItem =  0;

// for button animations we'll allow a transition timer
char newModeAfterTime = 0;
char State_transitionTime = 0;
char State_transitionButton = BTN_ACCEPT;

// true if the TI-Basic loop should exit
char App_exitRequested = FALSE;


/*
	In-line include the various state machine modules for our different menu modes.
*/
#include "States/TitleScreen.c"
#include "States/MainMenu.c"
#include "States/MatchMenu.c"
#include "States/WeaponSettings.c"
#include "States/MatchSettings.c"
#include "States/TeamSettings.c"
#include "States/Credits.c"
#include "States/SaveAndExit.c"
#include "States/HelpAndSettings.c"
#include "States/MapSettings.c"


// --------------------------------------------------------------------------------------------------------------------------------------



/**
 * Resets all necessary variables for the app
*/
void State_init()
{

}


/**
 * @brief Main state update method for the entire app, should be called once per frame.
 */
void State_update()
{
	// before we do anything else, we should update the states of the keys
	Keys_update();

	// if there's transition time on the clock decrement it, and possible change state
	if(State_transitionTime>0)
	{
		State_transitionTime--;
		if(State_transitionTime==0)
		{
			// call change mode with 0 timer to actually change the mode now
			State_changeMode(newModeAfterTime, 0);
		}
	}
	
	// depending on the current menu mode, different logic will be present for each mode
	switch(Menu_mode)
	{
		case menuMode_TitleScreen:
			TitleScreen_update();
			break;
		case menuMode_MainMenu:
			MainMenu_update();
			break;
		case menuMode_MatchMenu:
			MatchMenu_update();
			break;
		case menuMode_WeaponsSettings:
			WeaponSettings_update();
			break;
		case menuMode_MatchSettings:
			MatchSettings_update();
			break;
		case menuMode_TeamSettings:
			TeamSettings_update();
			break;
		case menuMode_Credits:
			Credits_update();
			break;
		case menuMode_SaveAndExit:
			SaveAndExit_update();
			break;
		case menuMode_HelpAndSettings:
			HelpAndSettings_update();
			break;
		case menuMode_MapSettings:	
			MapSettings_update();
			break;
	}	
}


/**
 * Changes the State machines mode to a new mode.
 *
 * @param newMode a char representing a main state machine mode as defined in the MenuModes enum.
 * @param timer - transition timer for button animations
*/
void State_changeMode(char newMode, char timer)
{
	// don't allow further changes if we're already in transition
	if(State_transitionTime>0)
		return;

	// if timer is not 0, set the transition time and gtfo
	if(timer>0)
	{
		newModeAfterTime = newMode;
		State_transitionTime = timer;
		return;
	}

	// call the exit method for the current mode
	switch(Menu_mode)
	{
		case menuMode_TitleScreen:
			TitleScreen_exit();
			break;
		case menuMode_MainMenu:
			MainMenu_exit();
			break;
		case menuMode_MatchMenu:
			MatchMenu_exit();
			break;
		case menuMode_WeaponsSettings:
			WeaponSettings_exit();
			break;
		case menuMode_MatchSettings:
			MatchSettings_exit();
			break;
		case menuMode_TeamSettings:
			TeamSettings_exit();
			break;
		case menuMode_Credits:
			Credits_exit();
			break;
		case menuMode_SaveAndExit:
			SaveAndExit_exit();
			break;
		case menuMode_HelpAndSettings:
			HelpAndSettings_exit();
			break;
		case menuMode_MapSettings:
			MapSettings_exit();
			break;
	}	
	
	// save our current mode as our previous mode
	Menu_previousMode = Menu_mode;
	
	// change the menu mode officially
	Menu_mode = newMode;
	
	// call the enter method for the new mode!
	switch(Menu_mode)
	{
		case menuMode_TitleScreen:
			TitleScreen_enter();
			break;
		case menuMode_MainMenu:
			MainMenu_enter();
			break;
		case menuMode_MatchMenu:
			MatchMenu_enter();
			break;
		case menuMode_WeaponsSettings:
			WeaponSettings_enter();
			break;
		case menuMode_MatchSettings:
			MatchSettings_enter();
			break;
		case menuMode_TeamSettings:
			TeamSettings_enter();
			break;
		case menuMode_Credits:
			Credits_enter();
			break;
		case menuMode_SaveAndExit:
			SaveAndExit_enter();
			break;
		case menuMode_HelpAndSettings:
			HelpAndSettings_enter();
			break;
		case menuMode_MapSettings:
			MapSettings_enter();
			break;
	}	
}
